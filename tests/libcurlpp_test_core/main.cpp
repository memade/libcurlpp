#include <win.h>
#include <libcurlpp.hpp>
int main(int argc, char** argv) {
#if defined(_DEBUG)
 ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
 //::_CrtSetBreakAlloc(3709);
#endif

 libcurlpp::ILibcurlpp* pLibcurl = libcurlpp::ILibcurlpp::CreateInterface(R"(D:\__Github__\Windows\projects\libcurlpp\bin\x64\Debug\libcurlpp.dll)");

 
 shared::Win::MainProcess(
  [&](const std::string& input, bool& exit) {
   if (input == "q") {
    exit = true;
   }
   else if(input == "test") {
    do {
     auto req = pLibcurl->CreateRequest(1);
     if (!req)
      break;
     req->RequestUrl(R"(https://cn.bing.com/)");
     req->FinishCb(
      [&](const libcurlpp::IResponse* resObj) {
       do {
        if (resObj->HttpCode() != (long)libcurlpp::EnHttpCodeType::Success)
         break;
        std::cout << resObj->Body() << std::endl;
       } while (0);
      });

     pLibcurl->Perform(req);
    } while (0);
   }
  });

 libcurlpp::ILibcurlpp::DestoryInterface(pLibcurl);
 return 0;
}