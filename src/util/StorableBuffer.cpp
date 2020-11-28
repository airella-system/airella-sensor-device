#include "util/StorableBuffer.h"

StorableBuffer::StorableBuffer(String _name) : name(_name) {
  DeviceContainer.storage->tryToInit();
  if(!DeviceContainer.storage->isInit()) {
    Logger::debug("[StorableBuffer::StorableBuffer()] Storage is not initialized");
    return;
  }
  FS* storage = DeviceContainer.storage->getStorage();
  if(!storage->exists("/sbuffer")) {
    storage->mkdir("/sbuffer");
  }
  if(!storage->exists("/sbuffer/" + _name)) {
    storage->mkdir("/sbuffer/" + _name);
  }
  File bufferDirectory = storage->open("/sbuffer/" + _name);

  while (true) {
    File entry =  bufferDirectory.openNextFile();
    if(!entry) break;
    String data = entry.name();
    Logger::debug(data);
    storagedFiles.add(data);
    entry.close();
  }
  String* lastFileName = storagedFiles.getLast();
  if(lastFileName == nullptr) lastFileNum = 0;
  else {
    lastFileNum = atoi((*lastFileName).c_str());
  }
  initialized = true;
}

void StorableBuffer::push(const char* type, const String& data) {
  if(!initialized) return;
  if(bufferSize == BUFFER_MAX_SIZE) {
    saveToStorege();
    bufferSize = 0;
  }
  BufferItem bufferItem;
  bufferItem.data = data;
  bufferItem.date = timeProvider.getDataTime().toISOString();
  bufferItem.type = type;
  buffer[bufferSize++] = bufferItem;
}

BufferItem StorableBuffer::pop() {
  BufferItem empty;
  if(bufferSize == 0) return empty;
  return buffer[--bufferSize];
}

bool StorableBuffer::isEmpty() {
  return bufferSize == 0;
}

bool StorableBuffer::needSync() {
  return !isEmpty() || !storagedFiles.isEmpty();
}

void StorableBuffer::saveToStorege() {
  String data = "";
  for(BufferItem item : buffer) {
    data += item.date + item.data + "|" + item.type + "\n";
  }
  bufferSize = 0;

  ++lastFileNum;
  String newFileName = String("/sbuffer/" + name + "/" + String(lastFileNum, DEC));
  DeviceContainer.storage->write(data.c_str(), newFileName.c_str());
  storagedFiles.add(newFileName);
}

void StorableBuffer::loadFromStorege(MultiValueList& list) {
  String* fileName = storagedFiles.pop();
  if(fileName == NULL) return;
  String fileContent = DeviceContainer.storage->read((*fileName).c_str());
  String data;
  
  unsigned int lineStart = 0;
  unsigned int lineStartHelper = 0;
  unsigned int lineEnd = 0;
  unsigned int typeStart = 0;
  unsigned int typeEnd = 0;
  Logger::debug(fileContent);
  for(char singleChar : fileContent) {
    lineEnd++;
    if(singleChar == '\n') {
      MultiValueNode* node = new MultiValueNode(3);
      typeStart = lineStart + 20;
      for(int i = lineStart + 19; i < lineEnd; ++i) {
        if(fileContent[i] == '|') typeEnd = i;
      }
      if(lineStart == 0) lineStartHelper = 0;
      else lineStartHelper = lineStart - 1;
      node->values[0] = new String(fileContent.substring(lineStartHelper, lineStart + 19));
      node->values[1] = new String(fileContent.substring(typeStart, typeEnd));
      node->values[2] = new String(fileContent.substring(typeEnd + 1, lineEnd - 1));

      Logger::debug(node->values[0]);
      Logger::debug(node->values[1]);
      Logger::debug(node->values[2]);

      list.add(node);
      lineStart = lineEnd + 1;
    }
  }
  // DeviceContainer.storage->remove((*fileName).c_str());
}

void StorableBuffer::sync() {
  if(!initialized) return;
  if(!Internet::isOk()) return;

  MultiValueList list;
  while (!isEmpty()) {
    BufferItem item = pop();
    MultiValueNode* node = new MultiValueNode(3);
    node->values[0] = new String(item.date);
    node->values[1] = new String(item.data);
    node->values[2] = new String(item.type);
    list.add(node);
  }
  loadFromStorege(list);

  while(!list.isEmpty()) {
    MultiValueNode* node = list.pop();

    if(Api.publishHistoricalMeasurement(node->values[2], node->values[1], node->values[0])) {
      Logger::debug("test1");
      delay(10);
      MultiValueList::clear(node, 3);
      Logger::debug("test2");
    }
    else {
      list.add(node);
    }
  }
  
  if(!list.isEmpty()) {
    String data; 
    while(!list.isEmpty()) {
      MultiValueNode* node = list.pop();
      data += *node->values[0] + *node->values[2] + "|" + *node->values[1] + "\n";
      MultiValueList::clear(node, 3);
    }
    ++lastFileNum;
    String newFileName = String("/sbuffer/" + name + "/" + String(lastFileNum, DEC));
    DeviceContainer.storage->write(data.c_str(), newFileName.c_str());
    storagedFiles.add(newFileName);
  }
}
