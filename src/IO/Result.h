#pragma once
#include <expected>
#include <system_error>

namespace ii::device {
enum class ErrorCode {
  invalid_argument, not_found, permission_denied, already_exists, closed,
  unsupported, would_block, timed_out, cancelled, busy, system_error
};
struct Error final {
  ErrorCode code;
  std::error_code native;
};
template<class T> using Result = std::expected<T, Error>;
} // namespace ii::device
