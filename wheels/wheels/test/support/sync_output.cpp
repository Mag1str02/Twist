#include <wheels/test/support/sync_output.hpp>

namespace wheels {

SyncOutput& SyncCout() {
  static SyncOutput cout(std::cout);
  return cout;
}

SyncOutput& SyncCerr() {
  static SyncOutput cerr(std::cerr);
  return cerr;
}

}  // namespace wheels
