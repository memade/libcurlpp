#if !defined(INC_H___DE900CDD_AFE5_4FB6_BE64_FA1E0D31E988__HEAD__)
#define INC_H___DE900CDD_AFE5_4FB6_BE64_FA1E0D31E988__HEAD__

namespace local {
 class Request final : public IRequest, public curlpp::Easy, public IReqResCommData {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Request(const TypeIdentify&);
  virtual ~Request();
 private:
  void __Default();
 protected:
  const TypeIdentify& Identify() const override final;
  void Default() override final;
  void Verbose(const bool&) override final;
  void Header(const bool&) override final;
  void RequestType(const EnRequestType&) override final;
  void RequestUrl(const std::string&) override final;
  void HeadersSet(const TypeHeaders&) override final;
  bool HeadersAdd(const std::string&) override final;
  void RoutePtr(void*) override final;
  void* RoutePtr() const override final;
  void FinishCb(const tfFinishCb&) override final;
  /// TODO : Uploading data is not supported
  void ProgressCb(const tfProgressCb&) override final;
  void EnableWriteStream(const bool&) override final;
  bool CachePathname(const std::string&) override final;
  void MaxRecvSpeedLarge(const long long&) override final;
 public:
  void Action(const EnRequestAction&) override final;
  const EnRequestStatus& Status() const override final;
 public:
  const EnRequestAction& Action() const;
  long long MaxRecvSpeedLarge() const;
  void Status(const EnRequestStatus&);
  void What(const std::string&);
  void CurlCodeSet(const CURLcode&);
  void CurlMsgSet(const CURLMSG&);
  //!@ Finish() append Action = Stop~
  void Finish();
 protected:
  tfFinishCb m_FinishCb = nullptr;
  tfProgressCb m_ProgressCb = nullptr;
  std::ostringstream m_WriteStreamBuffer;
  EnRequestAction m_Action = EnRequestAction::Normal;
  EnRequestStatus m_Status = EnRequestStatus::Normal;
  EnRequestType m_RequestType = EnRequestType::REQUEST_TYPE_GET;
  TypeHeaders m_RequestHeadersCache;
  std::atomic_llong m_LastDownSize = 0;
  std::atomic_llong m_LastDownTimestampMS = 0;
  std::atomic_llong m_LastUploadSize = 0;
  std::atomic_llong m_LastUploadTimestampMS = 0;
  std::atomic_llong m_ResumeFromLarge = 0;
  std::atomic_llong m_MaxRecvSpeedLarge = 0;
 };


}///namespace local

/// /*新生®（上海）**/
/// /*2022_09_08T11:13:10.7275602Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___DE900CDD_AFE5_4FB6_BE64_FA1E0D31E988__HEAD__

