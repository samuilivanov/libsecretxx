#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "secret/secret_schema.hpp"
#include "secret/secret_service.hpp"
#include "secret/secret_value.hpp"
#include <doctest/doctest.h>
#include <string>

TEST_CASE("Integration: store and retrieve secret") {

  // Define schema
  secret::secret_schema userSchema("org.example.Password",
                                   {{"number", secret::attr_type::int_type},
                                    {"string", secret::attr_type::str_type}});

  auto user =
      userSchema.builder().set("number", 123).set("string", "teststd").build();

  secret::secret_service service;

  secret::secret_value password{"integration_test_pass"};

  auto result = service.store(*user, "the label", password);

  CHECK(result);

  auto retrieved = service.retrieve(*user);

  CHECK(std::strcmp(password.c_str(), retrieved->c_str()) ==
        0); // passes if strings are equal

  auto removed = service.remove(*user);

  CHECK(removed);
}
