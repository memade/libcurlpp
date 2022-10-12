#include "stdafx.h"

namespace local {

 Request::Request(const TypeIdentify& identify) :
  IReqResCommData(identify) {
  __Default();
 }

 Request::~Request() {
  SK_DELETE_PTR(m_pFileCache);
 }
 void Request::__Default() {
  setOpt(new curlpp::options::Verbose(true));
  /// 以秒为单位：
  /// curl_setopt($ch, CURLOPT_TIMEOUT, 1);
  /// 以毫秒为单位：在CURL7.16.2中被加入，从PHP5.2.3起可使用
  /// curl_setopt($ch, CURLOPT_NOSIGNAL, true);
  /// curl_setopt($ch, CURLOPT_TIMEOUT_MS, 200);
  /// 以毫秒为单位：在CURL7.16.2中被加入，从PHP5.2.3起可使用
  setOpt(new curlpp::options::NoSignal(true));
  /// 
  setOpt(new curlpp::options::SslVerifyHost(0));
  setOpt(new curlpp::options::SslVerifyPeer(0));
  /// 如果设置的URL不存在的话，服务器返回404错误，但是程序发现不了错误，还是会下载这个404页面。
  /// 这时需要设置CURLOPT_FAILONERROR属性，当HTTP返回值大于等于400的时候
  setOpt(new curlpp::options::FailOnError(true));
  /// 使用默认发起连接超时
  setOpt(new curlpp::options::ConnectTimeout(0));
  /// 使用默认超时
  setOpt(new curlpp::options::Timeout(0));
  /// 不启用头回调
  setOpt(new curlpp::options::Header(false));
  /// 是有正文Body
  setOpt(new curlpp::options::NoBody(true));
  /// 是GET请求
  setOpt(new curlpp::options::HttpGet(true));
  m_RequestType = EnRequestType::REQUEST_TYPE_GET;
  /// 不是POST请求
  setOpt(new curlpp::options::Post(false));
  /// 不启用进度回调
  setOpt(new curlpp::options::NoProgress(false));
  /// 接收缓冲
  setOpt(new curlpp::options::WriteStream(&m_WriteStreamBuffer));
 }
 const TypeIdentify& Request::Identify() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_Identify;
 }
 void Request::Default() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  __Default();
 }
 void Request::RoutePtr(void* ptr) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_RoutePtr = ptr;
 }
 void* Request::RoutePtr() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_RoutePtr;
 }
 void Request::Verbose(const bool& verbose) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  setOpt(new curlpp::options::Verbose(verbose));
 }
 void Request::CurlCodeSet(const CURLcode& code) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_CurlCode = static_cast<unsigned int>(code);
 }
 void Request::CurlMsgSet(const CURLMSG& code) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_CurlMsg = static_cast<unsigned int>(code);
 }
 bool Request::CachePathname(const std::string& cahce_pathname) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   SK_DELETE_PTR(m_pFileCache);
   m_pFileCache = new FileCache(cahce_pathname);
   if (!m_pFileCache)
    break;
   m_ResumeFromLarge.store(static_cast<decltype(m_ResumeFromLarge)>(m_pFileCache->FileSizeGet()));
   setOpt(new curlpp::options::ResumeFromLarge(static_cast<curl_off_t>(m_ResumeFromLarge.load())));
   m_CachePathname = cahce_pathname;
   result = true;
  } while (0);
  return result;
 }
 void Request::Header(const bool& header) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  setOpt(new curlpp::options::Header(header));
  if (header) {
   setOpt(new curlpp::options::HeaderFunction(
    [this](char* buffer, size_t size, size_t items)->size_t {
     size_t result = size * items;
     do {
      if (!buffer || result <= 0) {
       m_Action = EnRequestAction::Stop;
       m_ExceptionReason = R"(Header write callback exception.)";
       break;
      }
      if (!Http::HeaderParse(std::string(buffer, result), m_ResponseHeaders)) {
       m_ExceptionReason = R"(Header parse exception.)";
       m_Action = EnRequestAction::Stop;
       break;
      }
     } while (0);
     return result;
    }));
  }

 }
 void Request::HeadersSet(const TypeHeaders& headers) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_RequestHeadersCache = headers;

 }
 bool Request::HeadersAdd(const std::string& kv) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (kv.empty())
    break;
   auto found = std::find(m_RequestHeadersCache.begin(), m_RequestHeadersCache.end(), kv);
   if (found != m_RequestHeadersCache.end())
    m_RequestHeadersCache.erase(found);
   m_RequestHeadersCache.emplace_back(kv);

   result = true;
  } while (0);
  return result;
 }
 void Request::EnableWriteStream(const bool& flag) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (flag)
   setOpt(new curlpp::options::WriteStream(&m_WriteStreamBuffer));
  else
   setOpt(new curlpp::options::WriteFunction(
    [this](char* buffer, size_t size, size_t items)->size_t {
     size_t result = size * items;
     do {
      if (!buffer || result <= 0) {
       m_Action = EnRequestAction::Stop;
       m_ExceptionReason = R"(Write callback exception.)";
       break;
      }

      //!@ 如里有缓存文件需求
      do {
       if (!m_pFileCache)
        break;



      } while (0);
      if (m_pFileCache) {
       if (!m_pFileCache->Write(buffer, result)) {
        m_ExceptionReason = R"(Write callback exception in 'cache module'.)";
        result = 0;
        break;
       }
      }
     } while (0);
     return result;
    }));
 }
 void Request::RequestType(const EnRequestType& requestType) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  switch (requestType) {
  case EnRequestType::REQUEST_TYPE_HEAD: {
   setOpt(new curlpp::options::NoBody(true));
   setOpt(new curlpp::options::HttpGet(false));
   setOpt(new curlpp::options::Post(false));
  }break;
  case EnRequestType::REQUEST_TYPE_GET: {
   setOpt(new curlpp::options::NoBody(false));
   setOpt(new curlpp::options::HttpGet(true));
   setOpt(new curlpp::options::Post(false));
  }break;
  case EnRequestType::REQUEST_TYPE_POST: {

  }break;
  case EnRequestType::REQUEST_TYPE_PUT: {

  }break;
  default: {

  }break;
  }
  m_RequestType = requestType;
 }
 void Request::RequestUrl(const std::string& url) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_OriginalRequestUrl = url;
  auto fix_url = Http::UrlFixed(url);
  setOpt(new curlpp::options::Url(fix_url));
 }
 void Request::What(const std::string& what) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_WhatRequest = what;
 }
 void Request::Action(const EnRequestAction& action) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_Action = action;
 }
 const EnRequestAction& Request::Action() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_Action;
 }
 const EnRequestStatus& Request::Status() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_Status;
 }
 void Request::Status(const EnRequestStatus& status) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_Status = status;
 }
 void Request::ProgressCb(const tfProgressCb& progress_cb) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_ProgressCb = progress_cb;
  if (!m_ProgressCb)
   return;
  setOpt(new curlpp::options::NoProgress(false));
  /*If your callback function returns CURL_PROGRESSFUNC_CONTINUE it will cause libcurl to continue executing the default progress function.
 Returning any other non-zero value from this callback will cause libcurl to abort the transfer and return CURLE_ABORTED_BY_CALLBACK.
 If you transfer data with the multi interface, this function will not be called during periods of idleness unless you call the appropriate libcurl function that performs transfers.
 CURLOPT_NOPROGRESS must be set to 0 to make this function actually get called.*/
  setOpt(new curlpp::options::ProgressFunction(
   [this](double dltotal, double dlnow, double ultotal, double ulnow)->int {
    const time_t current_time_stamp_ms = shared::Win::Time::TimeStamp<std::chrono::milliseconds>();
    int result = 0;
    do {
     if (dltotal <= 0 && dlnow <= 0)
      break;
     decltype(dltotal) route_dltotal = m_ResumeFromLarge.load() + dltotal;
     decltype(dlnow) route_dlnow = m_ResumeFromLarge.load() + dlnow;

     auto dlProgressInfo = Http::GenerateProgressInfo(
      route_dltotal,
      route_dlnow,
      m_LastDownSize.load() * 1.0,
      m_LastDownTimestampMS.load() > 0 ? (current_time_stamp_ms - m_LastDownTimestampMS.load()) : (0));

     //!@ 是否达到1秒或以上
     if (dlProgressInfo->m_speed_s > 0 || m_LastDownTimestampMS.load() <= 0 || m_LastDownSize.load() <= 0) {
      m_LastDownSize.store(static_cast<long long>(route_dlnow));
      m_LastDownTimestampMS.store(current_time_stamp_ms);
     }

     ProgressActionType action = m_ProgressCb(dlProgressInfo ? dlProgressInfo.get() : nullptr, nullptr);
     result = static_cast<int>(action);
     if (action == ProgressActionType::Break) {
      m_Action = EnRequestAction::Stop;
      m_ExceptionReason = R"(Progress callback assign break.)";
     }
    } while (0);
    return result;
   }
  ));
 }
 long long Request::MaxRecvSpeedLarge() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_MaxRecvSpeedLarge.load();
 }
 void Request::MaxRecvSpeedLarge(const long long& speed) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  setOpt(new curlpp::options::MaxRecvSpeedLarge(speed));
  m_MaxRecvSpeedLarge.store(speed);
 }
 void Request::FinishCb(const tfFinishCb& finish_cb) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_FinishCb = finish_cb;
 }
 void Request::Finish() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_FinishCb)
    break;
   m_WriteStream = m_WriteStreamBuffer.str();
   const char* found = R"(content-length: )";
   const size_t found_size = ::strlen(found);
   for (const auto& headstr : m_ResponseHeaders) {
    auto find = ::StrStrIA(headstr.c_str(), found);
    if (!find)
     continue;
    m_ContentLength = ::strtoul(find + found_size, nullptr, 10);
    break;
   }
   auto response = std::make_shared<Response>(m_Identify);
   /*response->OriginalRequestUrl(m_OriginalRequestUrl);
   response->PerformExceptionReason(m_ExceptionReason);
   response->WhatRequestSet(m_ErrorWhat);*/
   *response << this;
   //!@ Append res data.
   //! 
   //response->

   m_FinishCb(dynamic_cast<IResponse*>(response.get()));
  } while (0);
 }

}///namespace local