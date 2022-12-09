#if !defined(INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__)
#define INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__

namespace libcurlpp {
 class Request;
 class ProgressInfo;
 class Response final : public IResponse {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Response(const TypeIdentify& identify);
  virtual ~Response();
 public:
  const TypeIdentify& Identify() const override final;
  const std::string& OriginalRequestUrl() const override final;
  const std::string& FixedRequestUrl() const override final;
  void* RoutePtr() const override final;
  const std::string& WhatRequest() const override final;
  const std::string& WhatResponse() const override final;
  const std::string& CachePathname() const override final;
  const unsigned int& CurlCode() const override final;
  const unsigned int& CurlMsg() const override final;
  const long& HttpCode() const override final;
  const std::string& Body() const override final;
  const TypeHeaders& ResponseHeaders() const override final;
  const size_t& ContentLength() const override final;
  bool ResultFinal() const override final;
  EnRequestType RequestType() const override final;
  EnRequestAction Status() const override final;
  long long TargetTotalSize() const override final;
  long long ResumeFromLarge() const override final;
  long long MaxRecvSpeedLarge() const override final;
  EnResumeFromLargeMode ResumeFromLargeMode() const override final;
 public:
  void operator<<(const Request*);
  void operator<<(const tagProgress&);
  void operator<<(const TypeHeaders&);
  void operator<<(const std::ostringstream&);
  std::shared_ptr<ProgressInfo> DownProgressInfoGet() const;
  void Body(const std::string&);
  FileCache* FileCacheGet() const;
  void RequestType(const EnRequestType&);
  void RoutePtr(void*);
  void CurlCode(const unsigned int&);
  void CurlMsg(const unsigned int&);
  void CleanCacheFile();
  bool CreateCacheFile(const std::string&, curl_off_t&);
  void Action(const EnRequestAction&);
  void WhatRequest(const std::string&);
  TypeHeaders* ResponseHeaders();
  void RequestHeaders(const TypeHeaders&);
  void RequestHeadersAppend(const std::string&);
  bool EnableCacheFile() const;
  bool WriteToCacheFile(const char*, const size_t&);
  void FixedRequestUrl(const std::string&);
  void OriginalRequestUrl(const std::string&);
  void TargetTotalSize(const long long&);
  void ResumeFromLarge(const long long&);
  void ResumeFromLargeMode(const EnResumeFromLargeMode&);
  long long LastDownSize() const;
  void MaxRecvSpeedLarge(const long long&);
 private:
  const TypeIdentify m_Identify;
  TypeHeaders m_ResponseHeaders;
  TypeHeaders m_RequestHeaders;
  std::string m_OriginalRequestUrl;
  std::string m_FixedRequestUrl;
  void* m_RoutePtr = nullptr;
  std::string m_CachePathname;
  FileCache* m_pFileCache = nullptr;
  unsigned int m_CurlCode = -1;
  unsigned int m_CurlMsg = -1;
  std::string m_WriteStream;
  size_t m_ContentLength = 0;
  std::string m_WhatResponse;
  std::string m_WhatRequest;
 private:
  std::atomic_llong m_TargetTotalSize = 0;
  std::atomic_llong m_LastDownSize = 0;
  std::atomic_llong m_LastDownTimestampMS = 0;
  std::atomic<double> m_LastDownSpeed = 0;
  std::atomic_llong m_LastDownRemainTime = 0;
  std::atomic_llong m_LastUploadSize = 0;
  std::atomic_llong m_LastUploadTimestampMS = 0;
  std::atomic_llong m_ResumeFromLarge = 0;
  std::atomic_llong m_MaxRecvSpeedLarge = 0;
  std::atomic<EnRequestAction> m_Action = EnRequestAction::Normal;
  std::atomic<EnRequestType> m_RequestType = EnRequestType::REQUEST_TYPE_GET;
  std::atomic<EnResumeFromLargeMode> m_ResumeFromLargeMode = EnResumeFromLargeMode::Addup;
 private:
  ProgressInfo* m_pProgressInfoUpload = nullptr;
  ProgressInfo* m_pProgressInfoDownload = nullptr;

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


}///namespace libcurlpp

/// /*新生®（上海）**/
/// /*2022_09_08T11:12:47.8151364Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___3D9AAFA7_34E9_4421_A70F_1D8E4230A42B__HEAD__

