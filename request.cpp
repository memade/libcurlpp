#include "stdafx.h"
namespace libcurlpp {

 Request::Request(const TypeIdentify& identify) {

  m_pResponse = new Response(identify);

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
  m_pResponse->RequestType(EnRequestType::REQUEST_TYPE_GET);
  /// 不是POST请求
  setOpt(new curlpp::options::Post(false));
  /// 不启用进度回调
  setOpt(new curlpp::options::NoProgress(false));
  /// 接收缓冲
  setOpt(new curlpp::options::WriteStream(&m_WriteStreamBuffer));
 }

 Request::~Request() {
  SK_DELETE_PTR(m_pResponse);
 }
 const TypeIdentify& Request::Identify() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->Identify();
 }
 void Request::RoutePtr(void* ptr) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->RoutePtr(ptr);
 }
 void* Request::RoutePtr() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->RoutePtr();
 }
 void Request::Verbose(const bool& verbose) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  setOpt(new curlpp::options::Verbose(verbose));
 }
 const unsigned int& Request::CurlCodeGet() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->CurlCode();
 }
 const unsigned int& Request::CurlMsgGet() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->CurlMsg();
 }

 void Request::CurlCodeSet(const CURLcode& code) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->CurlCode(static_cast<unsigned int>(code));
 }
 void Request::CurlMsgSet(const CURLMSG& code) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->CurlMsg(static_cast<unsigned int>(code));
 }
 void Request::CleanCacheFile() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->CleanCacheFile();
 }

 bool Request::CachePathname(const std::string& cache_pathname) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  curl_off_t resume_from_large = 0;
  do {
   if (!m_pResponse->CreateCacheFile(cache_pathname, resume_from_large))
    break;
   result = true;
  } while (0);
  setOpt(new curlpp::options::ResumeFromLarge(resume_from_large));
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
     m_pResponse->Action(EnRequestAction::Stop);
     m_pResponse->WhatRequest(R"(Header write callback exception.)");
     break;
    }
    if (!Libcurlpp::HeaderParse(std::string(buffer, result), *m_pResponse->ResponseHeaders())) {
     m_pResponse->WhatRequest(R"(Header parse exception.)");
     m_pResponse->Action(EnRequestAction::Stop);
     break;
    }
   } while (0);
   return result;
    }));
  }

 }
 void Request::HeadersSet(const TypeHeaders& headers) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->RequestHeaders(headers);

 }
 void Request::HeadersAdd(const std::string& kv) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->RequestHeadersAppend(kv);
 }
 void Request::EnableWriteStream(const bool& flag) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (flag) {
   setOpt(new curlpp::options::WriteStream(&m_WriteStreamBuffer));
  }
  else {
   setOpt(new curlpp::options::WriteFunction(
    [this](char* buffer, size_t size, size_t items)->size_t {
     size_t result = size * items;
   do {
    if (!buffer || result <= 0) {
     m_pResponse->Action(EnRequestAction::Stop);
     m_pResponse->WhatRequest(R"(Write callback exception.)");
     break;
    }

    do {
     if (!m_pResponse->EnableCacheFile())
      break;
     if (!m_pResponse->WriteToCacheFile(buffer, result)) {
      m_pResponse->WhatRequest(R"(Write callback exception in 'cache module'.)");
      result = 0;
      break;
     }
    } while (0);

   } while (0);
   return result;
    }));
  }

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
  m_pResponse->RequestType(requestType);
 }
 void Request::RequestUrl(const std::string& url) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  auto fix_url = Libcurlpp::UrlFixed(url);
  setOpt(new curlpp::options::Url(fix_url));
  m_pResponse->OriginalRequestUrl(url);
  m_pResponse->FixedRequestUrl(fix_url);
 }
 void Request::What(const std::string& what) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->WhatRequest(what);
 }
 void Request::Action(const EnRequestAction& action) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->Action(action);
 }
 EnRequestStatus Request::Status() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->Status();
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
   if (m_pResponse->TargetTotalSize() <= 0)
    m_pResponse->TargetTotalSize(static_cast<long long>(dltotal));
   decltype(dltotal) route_dltotal = m_pResponse->ResumeFromLarge() + dltotal;
   decltype(dlnow) route_dlnow = m_pResponse->ResumeFromLarge() + dlnow;

   tagProgress progress_info;
   progress_info.current_down = route_dlnow;
   progress_info.total_down = route_dltotal;
   progress_info.current_time_stamp = current_time_stamp_ms;
   *m_pResponse << progress_info;

   ProgressActionType action = m_ProgressCb(m_pResponse->DownProgressInfoGet().get(), nullptr);
   result = static_cast<int>(action);
   if (action == ProgressActionType::Break) {
    m_pResponse->Action(EnRequestAction::Stop);
    m_pResponse->WhatRequest(R"(Progress callback assign break.)");
   }
  } while (0);
  return result;
   }
  ));
 }
 void Request::ResumeFromLargeMode(const EnResumeFromLargeMode& mode) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->ResumeFromLargeMode(mode);
 }
 long long Request::TargetTotalSize() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->TargetTotalSize();
 }
 long long Request::LastDownSize() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->LastDownSize();
 }
 void Request::ResumeFromLarge(const long long& resume_from_large_size) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_pResponse->ResumeFromLarge(resume_from_large_size);
  setOpt(new curlpp::options::ResumeFromLarge(static_cast<curl_off_t>(resume_from_large_size)));
 }
 long long Request::ResumeFromLarge() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->ResumeFromLarge();
 }
 long long Request::MaxRecvSpeedLarge() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pResponse->MaxRecvSpeedLarge();
 }
 void Request::MaxRecvSpeedLarge(const long long& speed) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  setOpt(new curlpp::options::MaxRecvSpeedLarge(speed));
  m_pResponse->MaxRecvSpeedLarge(speed);
 }
 void Request::FinishCb(const tfFinishCb& finish_cb) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_FinishCb = finish_cb;
 }
 const IResponse* Request::ResponseGet() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return dynamic_cast<IResponse*>(m_pResponse);
 }
 void Request::Finish() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_FinishCb)
    break;
   *m_pResponse << m_WriteStreamBuffer;
   *m_pResponse << this;
   m_FinishCb(dynamic_cast<IResponse*>(m_pResponse));
  } while (0);
  m_pResponse->Action(EnRequestAction::Stop);
 }

}///namespace libcurlpp