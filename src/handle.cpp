#include "pch.h"
#include "stl.h"
#include "memory.h"
#include "splice.h"
#include "fearsdk.h"
#include "feardata.h"
#include "app.h"
#include "handle.h"

handleData *handleData::object = 0;

handleData::handleData() {}

handleData *handleData::instance() {
  if (object == 0) {
    handleData *obj = new handleData();
    object = obj;
  }

  return object;
}

unsigned stdcall handleData::handleThread(void *hModHandle) {
  handleData *hData = handleData::instance();
  hData->init(hModHandle);
  return 0;
}

void handleData::init(void *hModHandle) {
  aData.hData = this;
  aData.pSdk = &pSdk;
  aData.hInstance = hModHandle;
  aData.init();
}

handleData::~handleData() {
  freeSplice();
}
