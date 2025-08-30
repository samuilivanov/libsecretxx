#ifndef SECRET_SCHEMA_HPP
#define SECRET_SCHEMA_HPP
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
#include <libsecret-1/libsecret/secret.h>
}

namespace secret {
enum class attr_type { str_type, int_type, bool_type };

struct attr {
  std::string name;
  attr_type type;
};

class base_instance; // TODO (samuil) think of a better name

class secret_schema {
public:
  secret_schema(std::string name, std::vector<attr> attribute_keys)
      : m_name(std::move(name)), m_attributes(std::move(attribute_keys)) {
  } // TODO (samuil) not really sure about this value and move idiom

  class instance_builder {
  public:
    explicit instance_builder(secret_schema *schema) : m_schema(schema) {}

    instance_builder &set(const std::string &key, const std::string &value) {
      m_values[key] = value;
      return *this;
    }

    instance_builder &set(const std::string &key, int value) {
      m_values[key] = std::to_string(value);
      return *this;
    }

    instance_builder &set(const std::string &key, bool value) {
      m_values[key] = value ? "true" : "false";
      return *this;
    }

    std::unique_ptr<base_instance> build() const;

  private:
    secret_schema *m_schema;
    std::unordered_map<std::string, std::string> m_values;
  };

  instance_builder builder() { return instance_builder(this); }

  SecretSchema &to_c_struct() const {
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
  const std::string &name() const { return m_name; }
  const std::vector<attr> &attributes() const { return m_attributes; }

private:
  std::string m_name;
  std::vector<attr> m_attributes;
};

class base_instance {
public:
  base_instance(secret_schema *schema,
                std::unordered_map<std::string, std::string> values)
      : m_schema(schema), m_values(std::move(values)) {}

  secret_schema *schema() const { return m_schema; }
  const std::unordered_map<std::string, std::string> &values() const {
    return m_values;
  }

  // the user never sees libsecret, but we can still give conversion here
  SecretSchema &to_c_struct() const { return m_schema->to_c_struct(); }

private:
  secret_schema *m_schema;
  std::unordered_map<std::string, std::string> m_values;
};

// Builder build implementation
inline std::unique_ptr<base_instance>
secret_schema::instance_builder::build() const {
  return std::make_unique<base_instance>(m_schema, m_values);
}

} // namespace secret
#endif
