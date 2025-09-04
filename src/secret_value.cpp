#include "secret/secret_value.hpp"

namespace secret {
void secret_value::init_from(const char *data, size_t len) {
  buffer_ = static_cast<char *>(std::malloc(len));
  if (!buffer_) {
    throw std::runtime_error("malloc failed");
  }
  if (mlock(buffer_, len) != 0) {
    std::free(buffer_);
    buffer_ = nullptr;
    throw std::runtime_error("mlock failed — check RLIMIT_MEMLOCK");
  }

  size_ = len;
  std::memcpy(buffer_, data, len);
}

void secret_value::clear() {
  if (!buffer_)
    return;

  std::memset(buffer_, 0, size_);
//   munlock(buffer_, size_);
  std::free(buffer_);
  buffer_ = nullptr;
  size_ = 0;
}

void secret_value::move_from(secret_value &&other) noexcept {
  buffer_ = other.buffer_;
  size_ = other.size_;
  other.buffer_ = nullptr;
  other.size_ = 0;
}
} // namespace secret
