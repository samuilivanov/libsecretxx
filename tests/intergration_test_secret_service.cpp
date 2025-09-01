// #include "secret/secret_schema.hpp"
// #include "secret/secret_service.hpp"
// #include <doctest/doctest.h>
// #include <string>

// TEST_CASE("Integration: store and retrieve secret") {
//   using namespace secret;

//   // Define schema
//   secret_schema userSchema("org.example.Password",
//                            {{"number", attr_type::int_type},
//                             {"string", attr_type::str_type},
//                             {"even", attr_type::bool_type}});

//   auto user = userSchema.builder()
//                   .set("number", 123)
//                   .set("string", "Alice")
//                   .set("even", true)
//                   .build();

//   secret_service service;

//   const std::string password = "integration_test_pass";

//   // Store the secret
//   CHECK_NOTHROW(service.store(*user, "the label", password));

//   // Lookup it immediately
//   GError *error = nullptr;
//   gchar *retrieved = secret_password_lookup_sync(
//       &user->to_c_struct(), NULL, &error, "number", 123, "string", "nine", "even", FALSE, NULL);

//   REQUIRE(retrieved != nullptr);
//   CHECK(password == retrieved);

//   if (retrieved)
//     secret_password_free(retrieved);

//   if (error)
//     g_error_free(error);

// }
