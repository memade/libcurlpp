#if !defined(INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__)
#define INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__

namespace local {

 class Response final : public IResponse, public ReqResCommData {
  friend class Request;
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Response();
  virtual ~Response();
 protected:
  const std::string& OriginalRequestUrl() const override final;
 private:
  void operator<<(const Request*);
  std::string What;
  std::string WhatRequest;
  std::string m_OriginalRequestUrl;
  std::string m_PerformExceptionReason;
  //!@ extern
  __int64 m_ContentLength = 0;
  std::string m_ContentBody;
  std::string m_ContentHead;
  unsigned int m_CurlCode = -1;
  unsigned int m_CurlMsg = -1;

  __int64 m_ResumeFromLarge = 0;
  std::uint64_t m_TimeoutResponseMS = 0;
  std::string m_EffectiveUrl;
  long m_ResponseCode = 0;
  long m_HttpConnectCode = 0;
  long m_FileTime = 0;
  double m_TotalTime = 0;
  double m_NameLookupTime = 0;
  double m_ConnectTime = 0;
  double m_PreTransferTime = 0;
  double m_StartTransferTime = 0;
  double m_RedirectTime = 0;
  long m_RedirectCount = 0;
  double m_SizeUpload = 0;
  double m_SizeDownload = 0;
  double m_SpeedDownload = 0;
  double m_SpeedUpload = 0;
  long m_HeaderSize = 0;
  long m_RequestSize = 0;
  long m_SslVerifyResult = 0;
  std::list<std::string> m_SslEngines;
  double m_ContentLengthDownload = 0;
  double m_ContentLengthUpload = 0;
  std::string m_ContentType;
  long m_HttpAuthAvail = 0;
  long m_ProxyAuthAvail = 0;
  long m_OsErrno = 0;
  long m_NumConnects = 0;
  std::list<std::string> m_CookieList;
 };


}///namespace local

/// /*新生®（上海）**/
/// /*2022_09_08T11:12:47.8151364Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__

