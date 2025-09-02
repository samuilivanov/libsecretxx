#ifndef G_HASH_TABLE
#define G_HASH_TABLE
#include <glib-2.0/glib.h>
#include <utility>

namespace secret::util {

class g_hash_table {
public:
  g_hash_table()
      : table_(g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free)) {
  }

  ~g_hash_table() {
    if (table_)
      g_hash_table_destroy(table_);
  }

  // non-copyable
  g_hash_table(const g_hash_table &) = delete;
  g_hash_table &operator=(const g_hash_table &) = delete;

  // movable
  g_hash_table(g_hash_table &&other) noexcept : table_(other.table_) {
    other.table_ = nullptr;
  }

  g_hash_table &operator=(g_hash_table &&other) noexcept {
    if (this != &other) {
      if (table_)
        g_hash_table_destroy(table_);
      table_ = other.table_;
      other.table_ = nullptr;
    }
    return *this;
  }

  // access
  GHashTable *get() const { return table_; }

  // transfer ownership (after libsecret consumes it)
  GHashTable *release() {
    GHashTable *tmp = table_;
    table_ = nullptr;
    return tmp;
  }

  void insert(const std::string &key, const std::string &val) {
    g_hash_table_insert(table_, g_strdup(key.c_str()), g_strdup(val.c_str()));
  }

private:
  GHashTable *table_;
};
} // namespace secret::util

#endif