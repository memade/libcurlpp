#include "stdafx.h"

namespace local {

 FileCache::FileCache(const std::string& filePathname) :
  m_FilePathname(filePathname) {

 }

 FileCache::~FileCache() {

 }
 bool FileCache::Write(const char* data, const long long& size) {
  bool result = false;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!data || size <= 0)
    break;
   if (!shared::Win::File::WriteAddto(m_FilePathname, std::string(data, static_cast<size_t>(size))))
    break;
   result = true;
  } while (0);
  return result;
 }
 std::streampos FileCache::FileSizeGet() const {
  std::streampos result = 0;
  std::lock_guard<std::mutex> lock{ *m_Mutex };
  do {
   if (!shared::Win::AccessA(m_FilePathname))
    break;
   result = shared::Win::File::ReadFileSizeA(m_FilePathname);
  } while (0);
  return result;
 }

}///namespace local