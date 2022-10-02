#include "stdafx.h"
#include "export.h"

#if !(ENABLE_STATIC_COMPILATION)
__shared_api_ void* __stdcall api_object_init(const void*, unsigned long) {
 void* result = nullptr;
 do {
  malware::http::__gpHttp = new malware::http::Http();
  if (!malware::http::__gpHttp)
   break;
  if (!malware::http::__gpHttp->Open())
   break;
  result = malware::http::__gpHttp;
 } while (0);
 return result;
}

__shared_api_ void __stdcall api_object_uninit() {
 if (malware::http::__gpHttp)
  malware::http::__gpHttp->Close();
 SK_DELETE_PTR(malware::http::__gpHttp);
}
#endif///#if !(ENABLE_STATIC_COMPILATION)
