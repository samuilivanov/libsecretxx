#ifndef SECRET_SCHEMA_HPP
#define SECRET_SCHEMA_HPP
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
  secret_schema(std::string name, std::vector<attr> attribute_keys);

  class instance_builder {
  public:
    explicit instance_builder(secret_schema *schema) : m_schema(schema) {}

    instance_builder &set(const std::string &key, const char *value);
    // TODO (samuil) thise set overloads should be refactored
    instance_builder &set(const std::string &key, int value);
    instance_builder &set(const std::string &key, bool value);

    std::unique_ptr<base_instance> build() const;

  private:
    secret_schema *m_schema;
    std::unordered_map<std::string, std::string> m_values;
  };

  instance_builder builder() { return instance_builder(this); }

  SecretSchema &to_c_struct() const;
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
  std::string get_string(const std::string &key) const {
    return m_values.at(key);
  }

  int get_int(const std::string &key) const {
    return std::stoi(m_values.at(key));
  }

  bool get_bool(const std::string &key) const {
    const auto &val = m_values.at(key);
    return val == "true";
  }

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
