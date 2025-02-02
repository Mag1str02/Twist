#include <wheels/system/env.hpp>

namespace wheels {

////////////////////////////////////////////////////////////////////////////////

std::optional<std::string> GetEnvVar(const char* name) {
  char* value = std::getenv(name);
  if (value) {
    return std::string(value);
  } else {
    return std::nullopt;
  }
}

std::string GetEnvVarOr(const char* name, const std::string or_value) {
  return GetEnvVar(name).value_or(or_value);
}

////////////////////////////////////////////////////////////////////////////////

std::optional<std::string> GetUser() {
  return GetEnvVar("USER");
}

#define TRY_GET(value, name)  \
  if (!value.has_value()) {   \
    value = GetEnvVar(#name); \
  }

std::optional<std::string> GetTempPath() {
  std::optional<std::string> tmp_path;

  TRY_GET(tmp_path, TMPDIR)
  TRY_GET(tmp_path, TMP)
  TRY_GET(tmp_path, TEMP)
  TRY_GET(tmp_path, TEMPDIR)

  return tmp_path;
}

}  // namespace wheels
