#include "stdafx.h"

namespace libcurlpp {

 extern Libcurlpp* __gpLibcurlpp = nullptr;

 Libcurlpp::Libcurlpp() {
  Init();
 }

 Libcurlpp::~Libcurlpp() {
  UnInit();
 }

 void Libcurlpp::Init() {
  curlpp::initialize(CURL_GLOBAL_ALL);
  m_pCurlMulti = new curlpp::IMulti();
 }

 void Libcurlpp::UnInit() {
  m_RequestQ.iterate_clear(
   [&](const auto&, auto& task, auto&, auto& itclear) {
    m_pCurlMulti->Pop(task);
  SK_DELETE_PTR(task);
  itclear = true;
   });
  SK_DELETE_PTR(m_pCurlMulti);
  curlpp::terminate();
 }
 void Libcurlpp::Release() const {
  delete this;
 }
 bool Libcurlpp::Start() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (m_IsOpen.load())
    break;

   m_IsOpen.store(true);
   m_Threads.emplace_back([this]() {MasterControl(); });
   m_Threads.emplace_back([this]() {MultiPerform(); });
  } while (0);
  return m_IsOpen.load();
 }

 void Libcurlpp::Stop() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_IsOpen.load())
    break;
   m_IsOpen.store(false);
   for (auto& it : m_Threads)
    it.join();
   m_Threads.clear();
  } while (0);
 }

 void Libcurlpp::DestoryRequest(const TypeIdentify& identify) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   m_RequestQ.pop(identify,
    [&](const auto&, auto& task) {
     m_pCurlMulti->Pop(task);
   SK_DELETE_PTR(task);
    });
  } while (0);
 }
 void Libcurlpp::DestoryRequest(const std::vector<TypeIdentify>& ids) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  for (const auto& id : ids) {
   m_RequestQ.pop(id,
    [&](const auto&, auto& task) {
     m_pCurlMulti->Pop(task);
   SK_DELETE_PTR(task);
    });
  }
 }
 IRequest* Libcurlpp::GetRequest(const TypeIdentify& identify) const {
  IRequest* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_RequestQ.search(identify,
   [&](auto& found) {
    result = found;
   });
  return result;
 }
 IRequest* Libcurlpp::CreateRequest(const TypeIdentify& identify) {
  IRequest* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   auto found = m_RequestQ.search(identify,
    [&](const auto&, auto& reqObj) {
     result = dynamic_cast<IRequest*>(reqObj);
    });
   if (!result) {
    auto request = new Request(identify);
    result = dynamic_cast<IRequest*>(request);
    m_RequestQ.push(identify, request);
   }
  } while (0);
  return result;
 }
 void Libcurlpp::Perform(IRequest* inReqObj) const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };

  for (int i = 0; i < 2; ++i) {
   if (!inReqObj)
    break;
   auto reqObj = dynamic_cast<Request*>(inReqObj);
   if (!reqObj)
    break;

   try {
    reqObj->perform();
    reqObj->CurlCodeSet(CURLcode::CURLE_OK);
   }
   catch (curlpp::LibcurlLogicError& e) {
    reqObj->CurlCodeSet(e.whatCode());
    reqObj->What(e.what());
   }
   catch (curlpp::LibcurlRuntimeError& e) {
    reqObj->CurlCodeSet(e.whatCode());
    reqObj->What(e.what());
   }
   catch (...) {
    reqObj->CurlCodeSet(CURLcode::CURLE_AUTH_ERROR);
    reqObj->What("Comm error.");
   }

   if (reqObj->CurlCodeGet() == CURLcode::CURLE_RANGE_ERROR) {
    reqObj->CleanCacheFile();
    inReqObj->ResumeFromLarge(0);

    std::cout << "Todo 'ResumeFromLarge' reason reset download." << std::endl;
    continue;
   }

   reqObj->Finish();
   break;
  }
 }

 void Libcurlpp::PerformM(const std::vector<IRequest*>& inReqObjs) const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  auto pMulti = new curlpp::Multi();
  do {
   if (inReqObjs.empty() || !pMulti)
    break;
   for (const auto& req : inReqObjs) {
    auto req_node = dynamic_cast<curlpp::Easy*>(const_cast<IRequest*>(req));
    if (!req_node)
     continue;
    pMulti->add(req_node);
   }

   try {
    int nbLeft = 0;
    /* we start some action by calling perform right away */
    while (!pMulti->perform(&nbLeft)) {};
    do {
     if (nbLeft <= 0)
      break;
     int maxfd = 0;
     fd_set fdread;
     fd_set fdwrite;
     fd_set fdexcep;
     FD_ZERO(&fdread);
     FD_ZERO(&fdwrite);
     FD_ZERO(&fdexcep);
     /* set a suitable timeout to play around with */
     struct timeval timeout = { 1,0 };
     /* get file descriptors from the transfers */
     pMulti->fdset(&fdread, &fdwrite, &fdexcep, &maxfd);
     /* select() return code */
     int rc = ::select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
     switch (rc) {
     case -1: {/* select error */
      nbLeft = 0;
      /// TODO : Pending by 2022.920.834
     }break;
     case 0:
      [[fallthrough]];
     default: {/* timeout or readable/writable sockets */
      while (!pMulti->perform(&nbLeft)) {};

      curlpp::Multi::Msgs msgs = pMulti->info();
      for (auto& msg : msgs) {
       bool sucess = false;
       Request* pReqObj = dynamic_cast<Request*>(const_cast<curlpp::Easy*>(msg.first));
       pReqObj->CurlCodeSet(msg.second.code);
       pReqObj->CurlMsgSet(msg.second.msg);
       do {
        if (msg.second.code != CURLcode::CURLE_OK)
         break;
        if (msg.second.msg != CURLMSG::CURLMSG_DONE)
         break;
        sucess = true;
       } while (0);
       /*The mission was not successfully completed*/
       //pReqObj->
       pReqObj->Finish();

       //!@TODO : 注意这里的判断不完善
       pReqObj->Action(EnRequestAction::Finish);
      }
     }break;
     }///switch
    } while (1);
   }
   catch (curlpp::LogicError& e) {
    std::cout << e.what() << std::endl;
   }
   catch (curlpp::RuntimeError& e) {
    std::cout << e.what() << std::endl;
   }
   catch (...) {
    std::cout << "http common error." << std::endl;
   }
  } while (0);
  SK_DELETE_PTR(pMulti);
 }

 bool Libcurlpp::HeaderParse(const std::string& head_field, TypeHeaders& out) {
  bool result = false;
  do {
   if (head_field.empty())
    break;
   auto found = head_field.find("\r\n");
   if (found == std::string::npos)
    break;
   //!@ HTTP/1.1 200 OK
   //!
   //!  
   found = head_field.find("200 OK");
   if (found != std::string::npos) {
    result = true;
    break;
   }
   if (head_field.compare("\r\n") == 0) {
    result = true;
    break;
   }
   found = head_field.find(": ");
   if (found == std::string::npos)
    break;
   out.push_back(shared::IConv::ToLowerA(head_field.substr(0, head_field.size() - ::strlen("\r\n"))));
   result = true;
  } while (0);
  return result;
 }
 bool Libcurlpp::UpdateProgressInfo(ProgressInfo* progress_info, const double& total, const double& current, const double& prev_current, const time_t& time_interval_ms) {
  bool result = false;
  do {
   if (!progress_info)
    break;
   ::memset(progress_info, 0x00, sizeof(*progress_info));
   if (total <= 0 || current <= 0)
    break;
   progress_info->m_current = current;
   progress_info->m_total = total;
   progress_info->m_percentage = (current / total) * 100.0;
   auto current_increment = current - prev_current;
   if (prev_current > 0 && time_interval_ms >= 1000 && current_increment > 0) {
    auto total_s = (time_interval_ms - (time_interval_ms % 1000)) / 1000;
    progress_info->m_speed_s = current_increment / total_s;
   }
   if (progress_info->m_speed_s > 0)
    progress_info->m_time_s = __max(0, static_cast<decltype(ProgressInfo::m_time_s)>((total - current) / progress_info->m_speed_s));
   result = true;
  } while (0);
  return result;
 }
 std::shared_ptr<ProgressInfo> Libcurlpp::GenerateProgressInfo(const double& total, const double& current, const double& prev_current, const time_t& time_interval_ms) {
  std::shared_ptr<ProgressInfo> result = std::make_shared<ProgressInfo>();
  do {
   if (total <= 0 || current <= 0)
    break;
   result->m_current = current;
   result->m_total = total;
   result->m_percentage = (current / total) * 100.0;
   auto current_increment = current - prev_current;
   if (prev_current > 0 && time_interval_ms >= 1000 && current_increment > 0) {
    auto total_s = (time_interval_ms - (time_interval_ms % 1000)) / 1000;
    result->m_speed_s = current_increment / total_s;
   }
   if (result->m_speed_s > 0)
    result->m_time_s = __max(0, static_cast<decltype(ProgressInfo::m_time_s)>((total - current) / result->m_speed_s));
  } while (0);
  return result;
 }
 std::string Libcurlpp::UrlFixed(const std::string& ascii_url, \
  const std::string& not_fixed_strings /*= R"(abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.?=&%:/\)"*/) {
  std::string result;
  do {
   if (ascii_url.empty())
    break;
   std::string fixed_url = shared::Win::UrlFixedA(ascii_url);
   if (fixed_url.empty())
    break;
   const std::string utf8_url = shared::IConv::MBytesToUTF8(fixed_url);
   if (utf8_url.empty())
    break;
   std::vector<std::string> src1;
   std::for_each(utf8_url.begin(), utf8_url.end(),
    [&](const auto& ch) {
     src1.emplace_back(std::string(1, ch));
    });
   if (src1.empty())
    break;
   std::string src2{ shared::IConv::ToUpperA(shared::Win::BinToHex(utf8_url)) };
   if (src2.empty())
    break;
   std::vector<std::string> res;
   for (size_t i = 0; i < src2.size(); i += 2) {
    res.emplace_back(std::string("%").append(1, src2[i]).append(1, src2[i + 1]));
   }
   if (res.empty())
    break;
   for (size_t i = 0; i < src1.size(); ++i) {
    if (not_fixed_strings.find(src1[i]) == std::string::npos) {
     src1[i] = res[i];
    }
   }
   std::for_each(src1.begin(), src1.end(),
    [&](const auto& node) {
     result.append(node);
    });
  } while (0);
  return result;
 }

 ProgressInfo::ProgressInfo() {

 }
 ProgressInfo::~ProgressInfo() {

 }
 void ProgressInfo::operator=(const ProgressInfo& obj) {
  m_current = obj.m_current;
  m_total = obj.m_total;
  m_speed_s = obj.m_speed_s;
  m_percentage = obj.m_percentage;
  m_time_s = obj.m_time_s;
 }
 const double& ProgressInfo::total() const {
  return m_total;
 }
 const double& ProgressInfo::current() const {
  return m_current;
 }
 const double& ProgressInfo::speed_s() const {
  return m_speed_s;
 }
 const long long& ProgressInfo::time_s() const {
  return m_time_s;
 }
 const double& ProgressInfo::percentage() const {
  return m_percentage;
 }




}///namespace libcurlpp

