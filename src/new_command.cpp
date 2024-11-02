#include "new_command.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

namespace file_server_userver {

namespace {

class newcommand final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-newcommand";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    return file_server_userver::NewCommand(request.GetArg("name"), request.GetArg("age"));
  }
};

} // namespace

std::string NewCommand(std::string_view name, std::string_view age) {
  if (name.empty()) {
    name = "unknown user";
  }

  if(age.empty()){
    age = "52";
  }
  
  return fmt::format("its working, {} with age {}\n", name, age);
}

void AppendNewCommand(userver::components::ComponentList &component_list) {
  component_list.Append<newcommand>();
}

} // namespace file_server_userver
