#if !defined(INC_H___D8AD52B7_1F82_4C4E_996B_BFFF032DE88F__HEAD__)
#define INC_H___D8AD52B7_1F82_4C4E_996B_BFFF032DE88F__HEAD__

namespace local {

 struct tagProgress {
  double total_down;
  double current_down;
  double total_upload;
  double current_upload;

  long long current_time_stamp;
  tagProgress() { ::memset(this, 0x00, sizeof(*this)); }
  void operator=(const tagProgress& obj) { ::memcpy(this,&obj,sizeof(*this)); }
 };

}///namespace local

/// /*新生®（上海）**/
/// /*2022_09_20T01:16:27.9186966Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___D8AD52B7_1F82_4C4E_996B_BFFF032DE88F__HEAD__


