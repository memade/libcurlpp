#if !defined(INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__)
#define INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__

namespace local {

 class Response final : public IResponse, public IReqResCommData {
  friend class Request;
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Response(const TypeIdentify& identify);
  virtual ~Response();
 protected:
  const TypeIdentify& Identify() const override final;
  const std::string& OriginalRequestUrl() const override final;
  void* RoutePtr() const override final;
  const std::string& WhatRequest() const override final;
  const std::string& WhatResponse() const override final;
  const std::string& CachePathname() const override final;
  const unsigned int& CurlCode() const override final;
  const unsigned int& CurlMsg() const override final;
  const std::string& ExceptionReason() const override final;
  const long& HttpCode() const override final;
  const std::string& Body() const override final;
  const TypeHeaders& ResponseHeaders() const override final;
  const size_t& ContentLength() const override final;
 private:
  void operator<<(const Request*);


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

