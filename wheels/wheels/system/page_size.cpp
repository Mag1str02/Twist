#include <wheels/system/page_size.hpp>

#include <unistd.h>

namespace wheels {

class PageSizeDetector {
 public:
  PageSizeDetector() {
    page_size_ = DetectPageSize();
  }

  size_t GetPageSize() const {
    return page_size_;
  }

 private:
  size_t DetectPageSize() {
#if UNIX
    return sysconf(_SC_PAGESIZE);
#elif WINDOWS
    return 4096;
#else
  #error "Platform not supported
#endif
  }

 private:
  size_t page_size_;
};

size_t PageSize() {
  static PageSizeDetector page_size_detector;

  return page_size_detector.GetPageSize();
}

}  // namespace wheels
