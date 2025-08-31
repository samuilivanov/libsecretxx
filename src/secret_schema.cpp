#include "secret/secret_schema.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

extern "C" {
#include <libsecret-1/libsecret/secret.h>
}

namespace secret {

secret_schema::secret_schema(std::string name, std::vector<attr> attribute_keys)
    : m_name(std::move(name)), m_attributes(std::move(attribute_keys)) {
} // TODO (samuil) not really sure about this value and move idiom

secret_schema::instance_builder &
secret_schema::instance_builder::set(const std::string &key,
                                     const char *value) {
  auto it =
      std::find_if(m_schema->attributes().begin(), m_schema->attributes().end(),
                   [&](const attr &a) { return a.name == key; });
  if (it == m_schema->attributes().end())
    throw std::invalid_argument("Unknown key: " + key);
  m_values[key] = value;
  return *this;
}

// TODO (samuil) thise set overloads should be refactored
secret_schema::instance_builder &
secret_schema::instance_builder::set(const std::string &key, int value) {
  auto it =
      std::find_if(m_schema->attributes().begin(), m_schema->attributes().end(),
                   [&](const attr &a) { return a.name == key; });
  if (it == m_schema->attributes().end())
    throw std::invalid_argument("Unknown key: " + key);
  m_values[key] = std::to_string(value);
  return *this;
}

secret_schema::instance_builder &
secret_schema::instance_builder::set(const std::string &key, bool value) {
  auto it =
      std::find_if(m_schema->attributes().begin(), m_schema->attributes().end(),
                   [&](const attr &a) { return a.name == key; });
  if (it == m_schema->attributes().end())
    throw std::invalid_argument("Unknown key: " + key);
  m_values[key] = value ? "true" : "false";
  return *this;
}

SecretSchema &secret_schema::to_c_struct() const {
  static thread_local SecretSchema c_schema{};
  std::memset(&c_schema, 0, sizeof(c_schema));

  c_schema.name = m_name.c_str();
  // c_schema.flags = 0

  size_t i = 0;
  for (; i < m_attributes.size() && i < 31; ++i) {
    c_schema.attributes[i].name = m_attributes[i].name.c_str();
    switch (m_attributes[i].type) {
    case attr_type::str_type:
      c_schema.attributes[i].type = SECRET_SCHEMA_ATTRIBUTE_STRING;
      break;
    case attr_type::int_type:
      c_schema.attributes[i].type = SECRET_SCHEMA_ATTRIBUTE_INTEGER;
      break;
    case attr_type::bool_type:
      c_schema.attributes[i].type = SECRET_SCHEMA_ATTRIBUTE_BOOLEAN;
      break;
    }
  }

  // null terminate
  c_schema.attributes[i].name = "NULL";
  c_schema.attributes[i].type = static_cast<SecretSchemaAttributeType>(0);

  return c_schema;
}

} // namespace secret
