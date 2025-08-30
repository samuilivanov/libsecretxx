#ifndef SECRET_SERVICE_HPP
#define SECRET_SERVICE_HPP

#include "secret_schema.hpp"
#include "secret_value.hpp"
#include <string>
#include <unordered_map>
extern "C" {
#include <libsecret-1/libsecret/secret.h>
}
/*
secret::service s;
s.store_secret("org.example.MyApp.Password",
               {{"username","alice"}, {"hostname","example.com"}},
               secret::value("mypassword"));*/

namespace secret {
class secret_service {
  void store(const base_instance &schema, secret_value &&secret) {
    auto s = schema.schema();
    GError *error = NULL;

    /*
     * The variable argument list is the attributes used to later
     * lookup the password. These attributes must conform to the schema.
     */
    secret_password_store_sync(
        schema.to_c_struct(), SECRET_COLLECTION_DEFAULT, "The label", "the password",
        NULL, &error, "number", 9, "string", "nine", "even", FALSE, NULL);

    if (error != NULL) {
      /* ... handle the failure here */
      g_error_free(error);
    } else {
      /* ... do something now that the password has been stored */
    }
  }
  void retrieve(const base_instance &schema);
  void remove(const base_instance &schema);
  bool exists(const base_instance &schema) const;
};
} // namespace secret

#endif
