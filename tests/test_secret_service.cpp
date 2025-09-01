#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "secret/secret_schema.hpp"
#include "secret/secret_service.hpp"
#include <doctest/doctest.h>
#include <string>

int success_secret_password_storev_sync(const SecretSchema *, GHashTable *,
                                        const char *, const char *,
                                        const char *, GCancellable *,
                                        GError **) {
  // Just return success
  return 1;
}

char *success_secret_password_lookup_sync(const SecretSchema *schema,
                                          GCancellable *cancellable,
                                          GError **error, ...) {
  return "test_pass";
}

int success_secret_password_clear_sync(const SecretSchema *schema,
                                       GCancellable *cancellable,
                                       GError **error, ...) {
  return 0;
}
void success_secret_password_free(char *) { return; }
int fail_secret_password_storev_sync(const SecretSchema *, GHashTable *,
                                     const char *, const char *, const char *,
                                     GCancellable *, GError **error) {
  *error = g_error_new_literal(
      g_quark_from_string("my-domain"), // domain (use a unique string)
      1,                                // code
      "This is the error message"       // message
  );
  return 0;
}

char *fail_secret_password_lookup_sync(const SecretSchema *schema,
                                       GCancellable *cancellable,
                                       GError **error, ...) {
  return nullptr;
}

int fail_secret_password_clear_sync(const SecretSchema *schema,
                                    GCancellable *cancellable, GError **error,
                                    ...) {
  _GError *e = static_cast<_GError *>(malloc(sizeof(_GError)));
  *error = e;
  return 1;
}
void fail_secret_password_free(char *) { return; }

secret::secret_ops mock_succes_ops = {
    success_secret_password_storev_sync, success_secret_password_lookup_sync,
    success_secret_password_clear_sync, success_secret_password_free

};

secret::secret_ops mock_fail_ops = {
    fail_secret_password_storev_sync, fail_secret_password_lookup_sync,
    fail_secret_password_clear_sync, fail_secret_password_free

};

TEST_CASE("Secret service store function") {

  // Create a schema
  secret::secret_schema userSchema("org.example.Password",
                                   {{"number", secret::attr_type::int_type},
                                    {"string", secret::attr_type::str_type},
                                    {"even", secret::attr_type::bool_type}});

  // Create an instance
  auto user = userSchema.builder()
                  .set("number", 42)
                  .set("string", "Alice")
                  .set("even", true)
                  .build();

  secret::secret_service service(mock_succes_ops);

  service.store(*user, "the label", "mypassword");
}

TEST_CASE("Store with error handling") {

  secret::secret_schema userSchema("org.example.Password",
                                   {{"number", secret::attr_type::int_type},
                                    {"string", secret::attr_type::str_type},
                                    {"even", secret::attr_type::bool_type}});

  auto user = userSchema.builder()
                  .set("number", 7)
                  .set("string", "Bob")
                  .set("even", false)
                  .build();

  secret::secret_service service(mock_fail_ops);

  auto result = service.store(*user, "the label", "mypassword");
  CHECK(result.has_value());
}
