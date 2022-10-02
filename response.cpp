#include "stdafx.h"

namespace malware {
 namespace http {

  Response::Response() {
  }

  Response::~Response() {
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
    What = e.what();
   }
   catch (curlpp::LibcurlLogicError& e) {
    What = e.what();
   }
   catch (curlpp::LogicError& e) {
    What = e.what();
   }
   catch (...) {
    What = "Comm error.";
   }
  }

  const std::string& Response::OriginalRequestUrl() const {
   std::lock_guard<std::mutex> lock{ *m_Mutex };
   return m_OriginalRequestUrl;
  }
 }///namespace http
}///namespace malware