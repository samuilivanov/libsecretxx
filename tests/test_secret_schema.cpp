#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "secret/secret_schema.hpp"
#include <doctest/doctest.h>

using namespace secret;

TEST_CASE("Schema creation") {
  secret_schema s("org.example.Password", {{"number", attr_type::int_type},
                                           {"string", attr_type::str_type},
                                           {"even", attr_type::bool_type}});

  CHECK(s.name() == "org.example.Password");
  CHECK(s.attributes().size() == 3);
  CHECK(s.attributes()[0].name == "number");
  CHECK(s.attributes()[1].type == attr_type::str_type);
}

TEST_CASE("Instance builder sets values correctly") {
  secret_schema s("org.example.Password", {{"number", attr_type::int_type},
                                           {"string", attr_type::str_type},
                                           {"even", attr_type::bool_type}});

  auto instance = s.builder()
                      .set("number", 42)
                      .set("string", "Alice")
                      .set("even", true)
                      .build();

  CHECK(instance->values().at("number") == "42");
  CHECK(instance->values().at("string") == "Alice");
  CHECK(instance->values().at("even") == "true");
}

TEST_CASE("Builder rejects invalid keys") {
  secret_schema s("org.example.Password", {{"number", attr_type::int_type}});

  auto builder = s.builder();
  CHECK_THROWS_AS(builder.set("invalid", 1), std::invalid_argument);
}

TEST_CASE("Typed getters work correctly") {
  secret_schema s("org.example.Password", {{"number", attr_type::int_type},
                                           {"string", attr_type::str_type},
                                           {"even", attr_type::bool_type}});

  auto instance = s.builder()
                      .set("number", 7)
                      .set("string", "Bob")
                      .set("even", false)
                      .build();

  CHECK(instance->get_int("number") == 7);
  CHECK(instance->get_string("string") == "Bob");
  CHECK(instance->get_bool("even") == false);
}

TEST_CASE("C struct conversion") {
  secret_schema s("org.example.Password", {{"number", attr_type::int_type},
                                           {"string", attr_type::str_type},
                                           {"even", attr_type::bool_type}});

  auto instance = s.builder()
                      .set("number", 1)
                      .set("string", "Alice")
                      .set("even", true)
                      .build();

  SecretSchema c = instance->to_c_struct();

  CHECK(std::string(c.name) == "org.example.Password");
  CHECK(std::string(c.attributes[0].name) == "number");
  CHECK(c.attributes[0].type == SECRET_SCHEMA_ATTRIBUTE_INTEGER);
  CHECK(std::string(c.attributes[1].name) == "string");
  CHECK(c.attributes[1].type == SECRET_SCHEMA_ATTRIBUTE_STRING);
  CHECK(std::string(c.attributes[2].name) == "even");
  CHECK(c.attributes[2].type == SECRET_SCHEMA_ATTRIBUTE_BOOLEAN);
  CHECK(std::string(c.attributes[3].name) == "NULL");
}

TEST_CASE("All attributes are used and null-terminated") {
  secret_schema s("org.example.Password", {{"a", attr_type::int_type},
                                           {"b", attr_type::str_type},
                                           {"c", attr_type::bool_type}});

  SecretSchema c = s.to_c_struct();

  CHECK(std::string(c.attributes[3].name) == "NULL");
  CHECK(c.attributes[3].type == 0);
}