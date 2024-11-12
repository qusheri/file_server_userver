#include "registration.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

#include "../db_funcs/sql_class.hpp"

#include "iostream"

namespace file_server_userver {

namespace {

  class Signup final : public userver::server::handlers::HttpHandlerBase {
  public:
    static constexpr std::string_view kName = "handler-signup";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &) const override {
      return file_server_userver::signup(request.GetArg("username"), request.GetArg("password"));
    }
  };

} // namespace

std::string signup(std::string_view name, std::string_view password) {
  if (name.empty()) {
    throw std::out_of_range("Also input your username");
  }

  if(password.empty()){
    throw std::out_of_range("Also input your password");
  }
  SQL db;
  db.addUser(name.data(), password.data());

  return fmt::format("user have registered with: username = {} and password = {}\n", name, password);
}

void AppendSignup(userver::components::ComponentList &component_list) {
  component_list.Append<Signup>();
}

} // namespace file_server_userver
