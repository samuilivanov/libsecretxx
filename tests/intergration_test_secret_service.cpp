#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "secret/secret_schema.hpp"
#include "secret/secret_service.hpp"
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

  const std::string password = "integration_test_pass";

  // Store the secret
  auto result = service.store(*user, "the label", password);

  CHECK(result);

  auto retrieved = service.retrieve(*user);

  CHECK(password == retrieved);

  auto removed = service.remove(*user);

  CHECK(removed);
}
