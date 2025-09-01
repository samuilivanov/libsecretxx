#ifndef SECRET_SERVICE_HPP
#define SECRET_SERVICE_HPP

#include "secret_schema.hpp"
#include "secret_value.hpp"
#include <algorithm>
#include <optional>
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

  std::optional<std::string> store(const base_instance &instance,
                                   const std::string &label,
                                   const std::string &password) {
    GError *error = nullptr;
    SecretSchema &c_schema = instance.to_c_struct();

    GHashTable *attrs = g_hash_table_new(g_str_hash, g_str_equal);

    for (const auto &[key, val] : instance.values()) {
      // Find the attribute type from the schema
      auto it = std::find_if(instance.schema()->attributes().begin(),
                             instance.schema()->attributes().end(),
                             [&](const attr &a) { return a.name == key; });
      if (it == instance.schema()->attributes().end())
        continue; // or throw

      switch (it->type) {
      case attr_type::str_type:
        g_hash_table_insert(attrs, g_strdup(key.c_str()),
                            g_strdup(val.c_str()));
        break;
      case attr_type::int_type:
        g_hash_table_insert(attrs, g_strdup(key.c_str()),
                            GINT_TO_POINTER(std::stoi(val)));
        break;
      case attr_type::bool_type:
        g_hash_table_insert(attrs, g_strdup(key.c_str()),
                            GINT_TO_POINTER(val == "true" ? 1 : 0));
        break;
      }
    }

    gboolean success = m_ops.secret_password_storev_sync(
        &c_schema,
        attrs,                     // 2nd argument = GHashTable*
        SECRET_COLLECTION_DEFAULT, // 3rd = collection
        label.c_str(),             // 4th = label
        password.c_str(),          // 5th = password
        nullptr,                   // 6th = GCancellable*
        &error                     // 7th = GError**
    );

    if (!success) {
      std::string msg = error ? error->message : "unknown error";
      if (error)
        g_error_free(error);
      return msg;
    }
    return std::nullopt;
  }

  void retrieve(const base_instance &schema);
  void remove(const base_instance &schema);
  bool exists(const base_instance &schema) const;

private:
  secret_ops m_ops;
};
} // namespace secret

#endif
