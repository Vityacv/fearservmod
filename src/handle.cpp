#include "pch.h"
#include "stl.h"
#include "memory.h"
#include "splice.h"
#include "fearsdk.h"
#include "feardata.h"
#include "app.h"
#include "handle.h"
#include "conv.h"

handleData *handleData::object = 0;

handleData::handleData() {}

handleData *handleData::instance() {
  if (object == 0) {
    handleData *obj = new handleData();
    object = obj;
  }

  return object;
}

unsigned stdcall handleData::handleInit(void *hModHandle) {
  handleData *hData = handleData::instance();
  hData->init(hModHandle);
  return 0;
}

void handleData::init(void *hModHandle) {
  aData.hData = this;
  aData.pSdk = &pSdk;
  aData.hInstance = hModHandle;
  aData.gEServer = (unsigned char *)(unsigned char *)GetModuleHandle(_T("engineserver.dll"));
  if(aData.gEServer){
    aData.init();
  }else{
      CloseHandle(CreateThread(0, 0x1000,
                               (LPTHREAD_START_ROUTINE)handleData::handleClientThread,
                               0, 0, 0));
  }
  
}

unsigned stdcall handleData::handleClientThread() {
  handleData *hData = handleData::instance();
  hData->aData.initClient();
  return 0;
}

handleData::~handleData() {
  freeSplice();
}
