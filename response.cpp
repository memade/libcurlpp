#include "stdafx.h"

namespace local {

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
   Http::UpdateProgressInfo(m_pProgressInfoDownload, progress_info.total_down, progress_info.current_down,
    m_LastDownSize.load() * 1.0,
    m_LastDownTimestampMS.load() > 0 ? (progress_info.current_time_stamp - m_LastDownTimestampMS.load()) : (0));

   //!@ 是否达到1秒或以上
   if (m_pProgressInfoDownload->m_speed_s > 0 || m_LastDownTimestampMS.load() <= 0 || m_LastDownSize.load() <= 0) {
    m_LastDownSize.store(static_cast<long long>(progress_info.current_down));
    m_LastDownTimestampMS.store(progress_info.current_time_stamp);
   }

  }
  if (m_pProgressInfoUpload) {

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
 const std::string& Response::ExceptionReason() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ExceptionReason;
 }
 const long& Response::HttpCode() const {
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  return m_ResponseCode;
 }
}///namespace local