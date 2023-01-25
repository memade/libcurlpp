#include "stdafx.h"

namespace libcurlpp {

 class ProgressInfo final : public IProgressInfo {
  friend class Request;
 public:
  ProgressInfo();
  virtual ~ProgressInfo();
 public:
  void operator=(const ProgressInfo&);
 protected:
  const double& total() const override final;
  const double& current() const override final;
  const double& speed_s() const override final;
  const double& percentage() const override final;
  const long long& time_s() const override final;
 public:
  double m_total = 0;
  double m_current = 0;
  double m_speed_s = 0;
  long long m_time_s = 0;
  double m_percentage = 0;
 };

 class Libcurlpp final : public ILibcurlpp {
  std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
 public:
  Libcurlpp();
  virtual ~Libcurlpp();
 private:
  void Init();
  void UnInit();
 public:
  bool Start() override final;
  void Stop() override final;
  void Release() const;
 public:
  void Perform(IRequest*) const override final;
  void PerformM(const std::vector<IRequest*>&) const override final;
  IRequest* CreateRequest(const TypeIdentify&) override final;
  void DestoryRequest(const TypeIdentify&) override final;
  void DestoryRequest(const std::vector<TypeIdentify>&) override final;
  IRequest* GetRequest(const TypeIdentify&) const override final;
 private:
  curlpp::IMulti* m_pCurlMulti = nullptr;
  std::atomic_bool m_IsOpen = false;
  std::atomic_bool m_Ready = false;
  shared::container::map<TypeIdentify, Request*> m_RequestQ;
  shared::container::map<TypeIdentify, Request*> m_MasterControlQ;
  void MasterControl();
  void MultiPerform();
  std::vector<std::thread> m_Threads;
 public:
  static std::string UrlFixed(const std::string& ascii_url, \
   const std::string& not_fixed_strings = R"(abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.?=&%:/\)");
  static bool HeaderParse(const std::string& head_field, TypeHeaders& out);
  static bool UpdateProgressInfo(ProgressInfo* progress_info, const double& total, const double& current, const double& prev_current, const time_t& time_interval_ms);
  static std::shared_ptr<ProgressInfo> GenerateProgressInfo(const double& total, const double& current, const double& prev_current, const time_t& time_interval_ms);
 };

 extern Libcurlpp* __gpLibcurlpp;
}///namespace libcurlpp
