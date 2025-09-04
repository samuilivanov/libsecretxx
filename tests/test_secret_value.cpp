#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "secret/secret_value.hpp"
#include <cstring>
#include <doctest/doctest.h>

TEST_CASE("secret_value stores and retrieves password") {
  const char *pwd = "super_secret_pass";
  secret::secret_value s(pwd);

  CHECK(std::strcmp(s.c_str(), pwd) == 0);
  CHECK(!s.empty());
}

TEST_CASE("secret_value copies input correctly") {
  std::string pwd = "another_pass";
  secret::secret_value s(pwd);

  // Change the original string — secret_value should not be affected
  pwd[0] = 'X';
  CHECK(std::strcmp(s.c_str(), "another_pass") == 0);
}

TEST_CASE("secret_value can be moved") {
  secret::secret_value s1("move_pass");
  const char *ptr = s1.c_str();

  secret::secret_value s2(std::move(s1));
  CHECK(s1.c_str() == nullptr);
  CHECK(std::strcmp(s2.c_str(), "move_pass") == 0);
  CHECK(s2.c_str() == ptr); // underlying buffer moved
}

TEST_CASE("secret_value move assignment works") {
  secret::secret_value s1("first");
  secret::secret_value s2("second");

  s2 = std::move(s1);
  CHECK(std::strcmp(s2.c_str(), "first") == 0);
  CHECK(s1.c_str() == nullptr);
}
