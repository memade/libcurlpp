#if !defined(INC_H___D8AD52B7_1F82_4C4E_996B_BFFF032DE88F__HEAD__)
#define INC_H___D8AD52B7_1F82_4C4E_996B_BFFF032DE88F__HEAD__

namespace local {

 class IReqResCommData : public shared::IRuntimeRecorder {
 public:
  IReqResCommData(const TypeIdentify& identify) : m_Identify(identify) {}
 public:
  const TypeIdentify m_Identify;
  TypeHeaders m_ResponseHeaders;
  std::string m_OriginalRequestUrl;
  void* m_RoutePtr = nullptr;
  std::string m_CachePathname;
  FileCache* m_pFileCache = nullptr;
  unsigned int m_CurlCode = -1;
  unsigned int m_CurlMsg = -1;
  std::string m_ExceptionReason;
  std::string m_WriteStream;
  size_t m_ContentLength = 0;
  std::string m_WhatResponse;
  std::string m_WhatRequest;
 };

}///namespace local

/// /*新生®（上海）**/
/// /*2022_09_20T01:16:27.9186966Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___D8AD52B7_1F82_4C4E_996B_BFFF032DE88F__HEAD__


