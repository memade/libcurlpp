#include "stdafx.h"

namespace libcurlpp {

 Response::Response(const TypeIdentify& identify) :
  m_Identify(identify),
  m_pProgressInfoUpload(new ProgressInfo()),
  m_pProgressInfoDownload(new ProgressInfo()) {
 }

 Response::~Response() {
  SK_DELETE_PTR(m_pFileCache);
  SK_DELETE_PTR(m_pProgressInfoUpload);
  SK_DELETE_PTR(m_pProgressInfoDownload);
 }
 bool Response::ResultFinal() const {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (m_CurlCode != CURLcode::CURLE_OK)
    break;
   if (m_ResponseCode != 200 && m_ResponseCode != 206)
    break;
   result = true;
  } while (0);
  return result;
 }
 std::shared_ptr<ProgressInfo> Response::DownProgressInfoGet() const {
  std::shared_ptr<ProgressInfo> result;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  result = std::make_shared<ProgressInfo>(*m_pProgressInfoDownload);
  return result;
 }
 void Response::operator<<(const tagProgress& progress_info) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (m_pProgressInfoDownload) {
   Libcurlpp::UpdateProgressInfo(m_pProgressInfoDownload, progress_info.total_down, progress_info.current_down,
    m_LastDownSize.load() * 1.0,
    m_LastDownTimestampMS.load() > 0 ? (progress_info.current_time_stamp - m_LastDownTimestampMS.load()) : (0));

   //!@ 是否达到1秒或以上
   if (m_pProgressInfoDownload->m_speed_s > 0 || m_LastDownTimestampMS.load() <= 0 || m_LastDownSize.load() <= 0) {
    m_LastDownSize.store(static_cast<long long>(progress_info.current_down));
    m_LastDownTimestampMS.store(progress_info.current_time_stamp);

    m_LastDownSpeed.store(m_pProgressInfoDownload->m_speed_s);
    m_LastDownRemainTime.store(m_pProgressInfoDownload->m_time_s);
   }

   if (m_pProgressInfoDownload->m_speed_s <= 0 && m_LastDownSpeed.load() > 0) {
    m_pProgressInfoDownload->m_speed_s = m_LastDownSpeed.load();
    m_pProgressInfoDownload->m_time_s = m_LastDownRemainTime.load();
   }
  }
  if (m_pProgressInfoUpload) {

  }
 }
 EnRequestAction Response::Status() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_Action.load();
 }
 const std::string& Response::FixedRequestUrl() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_FixedRequestUrl;
 }
 void Response::Action(const EnRequestAction& action) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_Action.store(action);
 }
 long long Response::TargetTotalSize() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_TargetTotalSize.load();
 }
 long long Response::ResumeFromLarge() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ResumeFromLarge.load();
 }
 void Response::ResumeFromLarge(const long long& size) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_ResumeFromLarge.store(size);
 }
 void Response::TargetTotalSize(const long long& total) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_TargetTotalSize.store(total);
 }
 void Response::FixedRequestUrl(const std::string& url) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_FixedRequestUrl = url;
 }
 void Response::OriginalRequestUrl(const std::string& url) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_OriginalRequestUrl = url;
 }
 TypeHeaders* Response::ResponseHeaders() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return &m_ResponseHeaders;
 }
 bool Response::WriteToCacheFile(const char* data, const size_t& size) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!m_pFileCache)
    break;
   result = m_pFileCache->Write(data, size);
  } while (0);
  return result;
 }
 bool Response::EnableCacheFile() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pFileCache ? true : false;
 }
 void Response::RequestHeadersAppend(const std::string& header) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (header.empty())
    break;
   auto found = std::find(m_RequestHeaders.begin(), m_RequestHeaders.end(), header);
   if (found != m_RequestHeaders.end())
    m_RequestHeaders.erase(found);
   m_RequestHeaders.emplace_back(header);
  } while (0);
 }
 void Response::RequestHeaders(const TypeHeaders& headers) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_RequestHeaders = headers;
 }
 void Response::WhatRequest(const std::string& what) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_WhatRequest = what;
 }
 bool Response::CreateCacheFile(const std::string& file_pathname, curl_off_t& resume_from_large) {
  bool result = false;
  resume_from_large = 0;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (file_pathname.empty())
    break;
   SK_DELETE_PTR(m_pFileCache);
   m_pFileCache = new FileCache(file_pathname);
   if (!m_pFileCache)
    break;
   auto current_size = m_pFileCache->FileSizeGet();
   m_ResumeFromLarge.store(static_cast<decltype(m_ResumeFromLarge)>(current_size));
   m_CachePathname = file_pathname;
   resume_from_large = m_ResumeFromLarge.load();
   result = true;
  } while (0);
  return result;
 }
 void Response::operator<<(const std::ostringstream& stream) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_WriteStream = stream.str();
 }
 void Response::MaxRecvSpeedLarge(const long long& large) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_MaxRecvSpeedLarge.store(large);
 }
 long long Response::MaxRecvSpeedLarge() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_MaxRecvSpeedLarge.load();
 }
 long long Response::LastDownSize() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_LastDownSize.load();
 }
 void Response::ResumeFromLargeMode(const EnResumeFromLargeMode& mode) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_ResumeFromLargeMode.store(mode);
 }
 EnResumeFromLargeMode Response::ResumeFromLargeMode() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ResumeFromLargeMode.load();
 }
 void Response::CleanCacheFile() {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (m_pFileCache)
   m_pFileCache->Reset();
  m_ResumeFromLarge.store(0);
  m_TargetTotalSize.store(0);
  m_LastDownSize.store(0);
  m_LastDownTimestampMS.store(0);
  m_LastDownSpeed.store(0);
  m_LastDownRemainTime.store(0);
 }
 void Response::CurlCode(const unsigned int& code) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_CurlCode = code;
 }
 void Response::CurlMsg(const unsigned int& code) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_CurlMsg = code;
 }
 void Response::RoutePtr(void* ptr) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_RoutePtr = ptr;
 }
 EnRequestType Response::RequestType() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_RequestType.load();
 }
 void Response::RequestType(const EnRequestType& type) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_RequestType.store(type);
 }
 FileCache* Response::FileCacheGet() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_pFileCache;
 }
 void Response::Body(const std::string& stream) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_WriteStream = stream;
 }
 void Response::operator<<(const TypeHeaders& headers) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  m_ResponseHeaders = headers;

  const char* found = R"(content-length: )";
  const size_t found_size = ::strlen(found);
  for (const auto& headstr : m_ResponseHeaders) {
   auto find = ::StrStrIA(headstr.c_str(), found);
   if (!find)
    continue;
   m_ContentLength = ::strtoul(find + found_size, nullptr, 10);
   break;
  }
 }
 void Response::operator<<(const Request* easy) {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  if (!easy)
   return;
  try {
   m_EffectiveUrl = curlpp::infos::EffectiveUrl::get(*easy);
   m_ResponseCode = curlpp::infos::ResponseCode::get(*easy);
   m_HttpConnectCode = curlpp::infos::HttpConnectCode::get(*easy);
   m_FileTime = curlpp::infos::FileTime::get(*easy);
   m_TotalTime = curlpp::infos::TotalTime::get(*easy);
   m_NameLookupTime = curlpp::infos::NameLookupTime::get(*easy);
   m_ConnectTime = curlpp::infos::ConnectTime::get(*easy);
   m_PreTransferTime = curlpp::infos::PreTransferTime::get(*easy);
   m_StartTransferTime = curlpp::infos::StartTransferTime::get(*easy);
   m_RedirectTime = curlpp::infos::RedirectTime::get(*easy);
   m_RedirectCount = curlpp::infos::RedirectCount::get(*easy);
   m_SizeUpload = curlpp::infos::SizeUpload::get(*easy);
   m_SizeDownload = curlpp::infos::SizeDownload::get(*easy);
   m_SpeedDownload = curlpp::infos::SpeedDownload::get(*easy);
   m_SpeedUpload = curlpp::infos::SpeedUpload::get(*easy);
   m_HeaderSize = curlpp::infos::HeaderSize::get(*easy);
   m_RequestSize = curlpp::infos::RequestSize::get(*easy);
   m_SslVerifyResult = curlpp::infos::SslVerifyResult::get(*easy);
   m_SslEngines = curlpp::infos::SslEngines::get(*easy);
   m_ContentLengthDownload = curlpp::infos::ContentLengthDownload::get(*easy);
   m_ContentLengthUpload = curlpp::infos::ContentLengthUpload::get(*easy);
   m_ContentType = curlpp::infos::ContentType::get(*easy);
   m_HttpAuthAvail = curlpp::infos::HttpAuthAvail::get(*easy);
   m_ProxyAuthAvail = curlpp::infos::ProxyAuthAvail::get(*easy);
   m_OsErrno = curlpp::infos::OsErrno::get(*easy);
   m_NumConnects = curlpp::infos::NumConnects::get(*easy);
   m_CookieList = curlpp::infos::CookieList::get(*easy);
  }
  catch (curlpp::LibcurlRuntimeError& e) {
   m_WhatResponse = e.what();
  }
  catch (curlpp::LibcurlLogicError& e) {
   m_WhatResponse = e.what();
  }
  catch (curlpp::LogicError& e) {
   m_WhatResponse = e.what();
  }
  catch (...) {
   m_WhatResponse = "Comm error.";
  }
 }
 const TypeIdentify& Response::Identify() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_Identify;
 }
 const size_t& Response::ContentLength() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ContentLength;
 }
 const TypeHeaders& Response::ResponseHeaders() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ResponseHeaders;
 }
 const std::string& Response::Body() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_WriteStream;
 }
 const std::string& Response::OriginalRequestUrl() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_OriginalRequestUrl;
 }
 void* Response::RoutePtr() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_RoutePtr;
 }
 const std::string& Response::WhatResponse() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_WhatResponse;
 }
 const std::string& Response::WhatRequest() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_WhatRequest;
 }
 const std::string& Response::CachePathname() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_CachePathname;
 }
 const unsigned int& Response::CurlCode() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_CurlCode;
 }
 const unsigned int& Response::CurlMsg() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_CurlMsg;
 }
 const long& Response::HttpCode() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ResponseCode;
 }
}///namespace libcurlpp
