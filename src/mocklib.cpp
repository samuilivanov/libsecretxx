#include "secret/secret.hpp"
// TODO (samuil) DELME
void test() {

  secret::secret_schema userSchema("org.example.Password",
                                   {{"number", secret::attr_type::int_type},
                                    {"string", secret::attr_type::str_type},
                                    {"even", secret::attr_type::bool_type}});

  std::unique_ptr<secret::base_instance> user = userSchema.builder()
                                                    .set("number", 1)
                                                    .set("string", "Alice")
                                                    .set("even", true)
                                                    .build();

  // User doesn’t see libsecret API
  // but inside library we can do:
  user->schema()->attributes();
  SecretSchema cStruct = user->to_c_struct();
}
