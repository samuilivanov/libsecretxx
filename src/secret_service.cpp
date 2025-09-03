#include "secret/secret_service.hpp"

namespace secret {
[[nodiscard]] std::expected<void, std::string>
secret_service::store(const base_instance &instance, const std::string &label,
                      const std::string &password) {
  GError *error = nullptr;
  SecretSchema &c_schema = instance.to_c_struct();
  util::g_hash_table attrs;

  for (const auto &[key, val] : instance.values()) {
    // Find the attribute type from the schema
    auto it = std::find_if(instance.schema()->attributes().begin(),
                           instance.schema()->attributes().end(),
                           [&](const attr &a) { return a.name == key; });
    if (it == instance.schema()->attributes().end())
      continue; // or throw

    std::string val_str;
    switch (it->type) {
    case attr_type::str_type:
      val_str = val;
      break;
    case attr_type::int_type:
      val_str = std::to_string(std::stoi(val));
      break;
    case attr_type::bool_type:
      val_str = (val == "true" ? "1" : "0");
      break;
    }
    attrs.insert(key, val_str);
  }

  gboolean success = m_ops.secret_password_storev_sync(
      &c_schema,
      attrs.get(),               // 2nd argument = GHashTable*
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
    return std::unexpected{msg};
  }
  return {};
}

std::expected<std::string, std::string>
secret_service::retrieve(const base_instance &instance) const {
  GError *error = nullptr;
  SecretSchema &c_schema = instance.to_c_struct();
  util::g_hash_table attrs;

  for (const auto &[key, val] : instance.values()) {
    // Find the attribute type from the schema
    auto it = std::find_if(instance.schema()->attributes().begin(),
                           instance.schema()->attributes().end(),
                           [&](const attr &a) { return a.name == key; });
    if (it == instance.schema()->attributes().end())
      continue; // or throw

    std::string val_str;
    switch (it->type) {
    case attr_type::str_type:
      val_str = val;
      break;
    case attr_type::int_type:
      val_str = std::to_string(std::stoi(val));
      break;
    case attr_type::bool_type:
      val_str = (val == "true" ? "1" : "0");
      break;
    }
    attrs.insert(key, val_str);
  }
  auto password =
      secret_password_lookupv_sync(&c_schema, attrs.get(), nullptr, &error);

  if (error) {
    std::string msg = error ? error->message : "unknown error";
    g_error_free(error);
    return std::unexpected{msg};
  }
  std::string r = password;
  secret_password_free(password);
  return {r};
}

} // namespace secret
