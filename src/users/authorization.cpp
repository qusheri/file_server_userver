#include "authorization.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

#include "../db_funcs/sql_class.hpp"

namespace file_server_userver {

namespace {

  class Signin final : public userver::server::handlers::HttpHandlerBase {
  public:
    static constexpr std::string_view kName = "handler-signin";

    using HttpHandlerBase::HttpHandlerBase;

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &) const override {
      return file_server_userver::signin(request.GetArg("username"), request.GetArg("password"));
    }
  };

} // namespace

std::string signin(std::string_view name, std::string_view password) {
  if (name.empty()) {
    throw std::out_of_range("Also input your login");
  }

  if(password.empty()){
    throw std::out_of_range("Also input your password");
  }

  SQL db;
  bool user_exists = db.validateUser(name.data(), password.data());
  if(!user_exists) return fmt::format("Sign up first");
  std::string token = db.addSession(db.userId_str(name.data()));

  return fmt::format("user have loginned with: username = {} and password = {}.\n There is your auth token: \n{}\nRemember ot copy this token. With that token you will make requests to server.\n", name, password, token);
}

void AppendSignin(userver::components::ComponentList &component_list) {
  component_list.Append<Signin>();
}

} // namespace file_server_userver
