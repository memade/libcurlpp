#include "stdafx.h"

namespace libcurlpp {


 void Libcurlpp::MasterControl() {
  do {
   m_RequestQ.iterate_clear(
    [&](const auto& identify, Request* task, auto& itbreak, auto& itclear) {
     switch (task->Status()) {
     case EnRequestAction::Normal: {

     }break;
     case EnRequestAction::Running: {

     }break;
     case EnRequestAction::Start: {
      task->Action(EnRequestStatus::Running);
      m_pCurlMulti->Push(dynamic_cast<curlpp::Easy*>(task));
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

 void Libcurlpp::MultiPerform() {
  std::vector<Request*> reset_down_s;
  do {
   for (Request* reset : reset_down_s) {
    reset->CleanCacheFile();
    reset->ResumeFromLarge(0);
    m_pCurlMulti->Push(dynamic_cast<curlpp::Easy*>(reset));
   }
   reset_down_s.clear();
   do {
    try {
     int nbLeft = 0;
     /* we start some action by calling perform right away */
     while (!m_pCurlMulti->Perform(&nbLeft)) {}
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
      m_pCurlMulti->FDSet(&fdread, &fdwrite, &fdexcep, &maxfd);
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
       while (!m_pCurlMulti->Perform(&nbLeft)) {}

       curlpp::IMulti::Msgs msgs = m_pCurlMulti->InfoGet();
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
         reset_down_s.emplace_back(pReqObj);
         std::cout << std::format("Todo task({}) 'ResumeFromLarge' reason reset download.", pReqObj->Identify()) << std::endl;
        }
        else {
         pReqObj->Action(EnRequestAction::Finish);
        }
        m_pCurlMulti->Pop(msg.first);
       }
      }break;
      }///switch

      if (!m_IsOpen.load())
       break;
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



}///namespace libcurlpp
