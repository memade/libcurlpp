#include "stdafx.h"
#include "export.h"

#if !(ENABLE_STATIC_COMPILATION)
__shared_api_ void* __stdcall api_object_init(const void*, unsigned long) {
 void* result = nullptr;
 do {
  local::__gpHttp = new local::Http();
  if (!local::__gpHttp)
   break;
  if (!local::__gpHttp->Open())
   break;
  result = local::__gpHttp;
 } while (0);
 return result;
}

__shared_api_ void __stdcall api_object_uninit() {
 if (local::__gpHttp)
  local::__gpHttp->Close();
 SK_DELETE_PTR(local::__gpHttp);
}
#endif///#if !(ENABLE_STATIC_COMPILATION)
