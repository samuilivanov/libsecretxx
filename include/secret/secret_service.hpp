#ifndef SECRET_SERVICE_HPP
#define SECRET_SERVICE_HPP

#include "secret_schema.hpp"
#include "secret_value.hpp"
#include "util/g_hash_table.hpp"
#include <algorithm>
#include <expected>
#include <string>
#include <unordered_map>
namespace secret {

struct secret_ops {
  int (*secret_password_storev_sync)(const SecretSchema *, GHashTable *,
                                     const char *, const char *, const char *,
                                     GCancellable *, GError **);

  char *(*secret_password_lookup_sync)(const SecretSchema *schema,
                                       GCancellable *cancellable,
                                       GError **error, ...);

  int (*secret_password_clear_sync)(const SecretSchema *schema,
                                    GCancellable *cancellable, GError **error,
                                    ...);
  void (*secret_password_free)(char *);
};

static secret_ops default_ops = {
    ::secret_password_storev_sync, ::secret_password_lookup_sync,
    ::secret_password_clear_sync, ::secret_password_free};

class secret_service {
public:
  secret_service(secret_ops ops = default_ops) : m_ops(ops) {}

  [[nodiscard]] std::expected<void, std::string>
  store(const base_instance &instance, const std::string &label,
        const std::string &password) const;

  [[nodiscard]] std::expected<std::string, std::string>
  retrieve(const base_instance &schema) const;
  bool remove(const base_instance &schema) const;

private:
  secret_ops m_ops;
};
} // namespace secret

#endif
