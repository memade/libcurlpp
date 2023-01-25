#if !defined(INC_H___DE900CDD_AFE5_4FB6_BE64_FA1E0D31E988__HEAD__)
#define INC_H___DE900CDD_AFE5_4FB6_BE64_FA1E0D31E988__HEAD__

namespace libcurlpp {
 class Request final : public IRequest, public curlpp::Easy {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Request(const TypeIdentify&);
  virtual ~Request();
 protected:
  void Verbose(const bool&) override final;
  void Header(const bool&) override final;
  void RequestType(const EnRequestType&) override final;
  void RequestUrl(const std::string&) override final;
  void HeadersSet(const TypeHeaders&) override final;
  void HeadersAdd(const std::string&) override final;
  void RoutePtr(void*) override final;
  void* RoutePtr() const override final;
  void FinishCb(const tfFinishCb&) override final;
  /// TODO : Uploading data is not supported
  void ProgressCb(const tfProgressCb&) override final;
  void EnableWriteStream(const bool&) override final;
  bool CachePathname(const std::string&) override final;
  void MaxRecvSpeedLarge(const long long&) override final;
  void ResumeFromLargeMode(const EnResumeFromLargeMode&) override final;
  const IResponse* ResponseGet() const override final;
 public:
  const TypeIdentify& Identify() const override final;
  long long LastDownSize() const override final;
  void ResumeFromLarge(const long long&) override final;
  long long ResumeFromLarge() const override final;
  void Action(const EnRequestAction&) override final;
  EnRequestStatus Status() const override final;
  long long TargetTotalSize() const override final;
  void CleanCacheFile();
 public:
  long long MaxRecvSpeedLarge() const;
  void What(const std::string&);
  void CurlCodeSet(const CURLcode&);
  void CurlMsgSet(const CURLMSG&);
  const unsigned int& CurlCodeGet() const;
  const unsigned int& CurlMsgGet() const;
  //!@ Finish() append Action = Stop~
  void Finish();
 protected:
  tfFinishCb m_FinishCb = nullptr;
  tfProgressCb m_ProgressCb = nullptr;
  std::ostringstream m_WriteStreamBuffer;
 private:
  Response* m_pResponse = nullptr;
 };


}///namespace libcurlpp


/// /*新生®（上海）**/
/// /*2022_09_08T11:13:10.7275602Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___DE900CDD_AFE5_4FB6_BE64_FA1E0D31E988__HEAD__

