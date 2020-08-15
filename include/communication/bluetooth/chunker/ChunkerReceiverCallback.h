#pragma once

#include "communication/bluetooth/chunker/BluetoothChunkReceiver.h"

class BluetoothChunkReceiver;

class ChunkerReceiverCallback {

public:
  virtual void callback();
  void setChunker(BluetoothChunkReceiver* _chunker) {
    chunker = _chunker;
  }

protected:
  BluetoothChunkReceiver* chunker;
  
};
