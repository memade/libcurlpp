#if !defined(INC_H___3C08D747_5FEF_4BDA_B8F4_8422961B32E0__HEAD__)
#define INC_H___3C08D747_5FEF_4BDA_B8F4_8422961B32E0__HEAD__

namespace malware {
 namespace http {

  class FileCache final {
   std::shared_ptr<std::mutex> m_Mutex = std::make_shared<std::mutex>();
  public:
   FileCache(const std::string&);
   virtual ~FileCache();
  public:
   bool Write(const char*, const long long&);
   std::streampos FileSizeGet() const;
  private:
   const std::string m_FilePathname;
  };


 }///namespace http
}///namespace malware

/// /*新生®（上海）**/
/// /*2022_09_19T00:53:35.2892746Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___3C08D747_5FEF_4BDA_B8F4_8422961B32E0__HEAD__
