#ifndef SECRET_VALUE_HPP
#define SECRET_VALUE_HPP

#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

namespace secret {

class secret_value {
public:
  explicit secret_value(const std::string &pwd) {
    init_from(pwd.c_str(), pwd.size() + 1); // include NUL terminator
  }

  explicit secret_value(const char *pwd) {
    init_from(pwd, std::strlen(pwd) + 1);
  }

  secret_value(const secret_value &) = delete;
  secret_value &operator=(const secret_value &) = delete;

  secret_value(secret_value &&other) noexcept { move_from(std::move(other)); }

  secret_value &operator=(secret_value &&other) noexcept {
    if (this != &other) {
      clear();
      move_from(std::move(other));
    }
    return *this;
  }

  ~secret_value() { clear(); }

  const char *c_str() const noexcept { return buffer_; }

  bool empty() const noexcept { return !buffer_ || *buffer_ == '\0'; }

private:
  char *buffer_ = nullptr;
  size_t size_ = 0;

  void init_from(const char *data, size_t len);

  void clear();

  void move_from(secret_value &&other) noexcept;
};
} // namespace secret
#endif
