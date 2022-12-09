#include "stdafx.h"
#include "export.h"

__shared_api_ void* __stdcall api_object_init(const void*, unsigned long) {
 void* result = nullptr;
 do {
  libcurlpp::__gpLibcurlpp = new libcurlpp::Libcurlpp();
  if (!libcurlpp::__gpLibcurlpp)
   break;
  result = libcurlpp::__gpLibcurlpp;
 } while (0);
 return result;
}

__shared_api_ void __stdcall api_object_uninit() {
 if (libcurlpp::__gpLibcurlpp)
  libcurlpp::__gpLibcurlpp->Stop();
 SK_DELETE_PTR(libcurlpp::__gpLibcurlpp);
}

