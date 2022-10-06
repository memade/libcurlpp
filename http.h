#include "stdafx.h"

namespace local {

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

 class Http final : public IHttpApi, public curlpp::IMulti {
 public:
  Http();
  virtual ~Http();
 private:
  void Init() override final;
  void UnInit() override final;
 public:
  bool Open();
  void Close();
 protected:
  void Perform(IRequest*) const override final;
  void PerformM(const std::vector<IRequest*>&) const override final;
  IRequest* CreateRequest() override final;
  void DestoryRequest(const IRequest*) override final;
  void DestoryRequest(const std::vector<IRequest*>&) override final;
  IRequest* SearchRequest(const TypeIdentify&) const override final;
 private:
  /// Generate request task identify
  TypeIdentify GenerateIdentify() const;
 private:
  std::atomic_bool m_IsOpen = false;
  std::atomic_bool m_Ready = false;
  shared::container::map<TypeIdentify, Request*> m_Requests;
  void Process();
  void Perform();
  std::vector<std::thread> m_Threads;
 public:
  static std::string UrlFixed(const std::string& ascii_url, \
   const std::string& not_fixed_strings = R"(abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.?=&%:/\)");
  static bool HeaderParse(const std::string& head_field, TypeHeaders& out);
  static std::shared_ptr<ProgressInfo> GenerateProgressInfo(const double& total, const double& current, const double& prev_current, const time_t& time_interval_ms);
 };



 extern Http* __gpHttp;
}///namespace local