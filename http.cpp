#include "stdafx.h"

namespace local {
 Http* __gpHttp = nullptr;

 Http::Http() {
  Init();
 }

 Http::~Http() {
  UnInit();
 }

 void Http::Init() {
  curlpp::initialize(CURL_GLOBAL_ALL);
  __super::Init();
 }

 void Http::UnInit() {
  __super::UnInit();
  curlpp::terminate();
 }

 bool Http::Open() {
  do {
   if (m_IsOpen.load())
    break;

   m_IsOpen.store(true);
   m_Threads.emplace_back([this]() {Process(); });
   m_Threads.emplace_back([this]() {Perform(); });
  } while (0);
  return m_IsOpen.load();
 }

 void Http::Close() {
  if (!m_IsOpen.load())
   return;

  m_Requests.iterate_clear(
   [&](const auto&, auto& task, auto&, auto& itclear) {

    SK_DELETE_PTR(task);
    itclear = true;
   });

  m_IsOpen.store(false);
  for (auto& it : m_Threads)
   it.join();
  m_Threads.clear();
 }
 void Http::DestoryRequest(const IRequest* reqObj) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!reqObj)
    break;
   m_Requests.pop(reqObj->Identify(),
    [&](const auto&, auto& task) {

     SK_DELETE_PTR(task);
    });
  } while (0);
 }
 void Http::DestoryRequest(const std::vector<IRequest*>& reqObjs) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  for (const auto& reqObj : reqObjs) {
   if (!reqObj)
    continue;
   m_Requests.pop(reqObj->Identify(),
    [&](const auto&, auto& task) {

     SK_DELETE_PTR(task);
    });
  }
 }
 void Http::RegisterTaskNotifyCallback(const tfTaskNotifyCallback& notify_cb) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_TaskNotifyCallback = notify_cb;
 }
 IRequest* Http::SearchRequest(const TypeIdentify& identify) const {
  IRequest* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_Requests.search(identify,
   [&](auto& found) {
    result = found;
   });
  return result;
 }
 IRequest* Http::CreateRequest() {
  IRequest* result = nullptr;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   auto identify = GenerateIdentify();
   auto request = new Request(identify);
   m_Requests.push(identify, request);
   result = dynamic_cast<decltype(result)>(request);
  } while (0);
  return result;
 }
 void Http::Perform(IRequest* inReqObj) const {
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

 void Http::PerformM(const std::vector<IRequest*>& inReqObjs) const {
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
       auto sk = 0;
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
 void Http::Process() {
  do {
   m_Requests.iterate_clear(
    [&](const auto& identify, Request* task, auto& itbreak, auto& itclear) {
#if 0
     curlpp::options::Header head;
     task->getOpt(head);
     curlpp::options::NoBody nobody;
     task->getOpt(nobody);
     auto msg = \
      std::format("Task({}) head({}) body({}) action({}).", task->Identify(), head.getValue() ? "true" : "false", nobody.getValue() ? "true" : "false", static_cast<unsigned long long>(task->Status()));
     ::MessageBoxA(NULL, msg.c_str(), NULL, MB_TOPMOST);
#endif
     switch (task->Status()) {
     case EnRequestAction::Normal: {

     }break;
     case EnRequestAction::Running: {

     }break;
     case EnRequestAction::Start: {
      task->Action(EnRequestStatus::Running);
      Push(dynamic_cast<curlpp::Easy*>(task));
     }break;
     case EnRequestAction::Finish: {
      task->Action(EnRequestStatus::Finished);
      task->Finish();
     }break;
     case EnRequestAction::Stop: {
      task->Action(EnRequestAction::Remove);
     }break;
     case EnRequestAction::Remove: {
      itclear = true;
     }break;
     default: {
     }break;
     }
    });
   if (!m_IsOpen.load())
    break;
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (1);
 }

 void Http::Perform() {

  //!@ According to business requirements, tasks that need to be re-downloaded in the event of a specified error
  std::vector<Request*> redown_s;

  do {

   for (Request* req : redown_s) {
    req->CleanCacheFile();
    req->ResumeFromLarge(0);
    Push(dynamic_cast<curlpp::Easy*>(req));
   }
   redown_s.clear();

   do {

    try {
     int nbLeft = 0;
     /* we start some action by calling perform right away */
     while (!curlpp::IMulti::Perform(&nbLeft)) {};
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
      curlpp::IMulti::FDSet(&fdread, &fdwrite, &fdexcep, &maxfd);
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
       while (!curlpp::IMulti::Perform(&nbLeft)) {};

       curlpp::IMulti::Msgs msgs = InfoGet();
       for (std::pair<const curlpp::Easy*, curlpp::IMulti::Info>& msg : msgs) {
        Request* pReqObj = dynamic_cast<Request*>(const_cast<curlpp::Easy*>(msg.first));
        pReqObj->CurlCodeSet(msg.second.code);
        pReqObj->CurlMsgSet(msg.second.msg);
        do {
         if (msg.second.code != CURLcode::CURLE_OK)
          break;
         if (msg.second.msg != CURLMSG::CURLMSG_DONE)
          break;

         auto success = 1;
        } while (0);

        if (pReqObj->CurlCodeGet() == CURLcode::CURLE_RANGE_ERROR) {
         redown_s.emplace_back(pReqObj);
         std::cout << std::format("Todo task({}) 'ResumeFromLarge' reason reset download.", pReqObj->Identify()) << std::endl;
        }
        else {
         pReqObj->Action(EnRequestAction::Finish);
        }
        Pop(msg.first);
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

   if (!m_IsOpen.load())
    break;
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (1);
 }


  /// GenerateIdentify() is class private methods are not allowed to be locked.
  TypeIdentify Http::GenerateIdentify() const {
   TypeIdentify identify = 0;
   do {
    identify = shared::Win::Time::TimeStamp<std::chrono::microseconds>();
   } while (0);
   return identify;
  }

  bool Http::HeaderParse(const std::string & head_field, TypeHeaders & out) {
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
  bool Http::UpdateProgressInfo(ProgressInfo* progress_info, const double& total, const double& current, const double& prev_current, const time_t& time_interval_ms) {
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
  std::shared_ptr<ProgressInfo> Http::GenerateProgressInfo(const double& total, const double& current, const double& prev_current, const time_t & time_interval_ms) {
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
  std::string Http::UrlFixed(const std::string & ascii_url, \
   const std::string & not_fixed_strings /*= R"(abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-.?=&%:/\)"*/) {
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
  void ProgressInfo::operator=(const ProgressInfo & obj) {
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



}///namespace local
