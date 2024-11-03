#include "fileHandler.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

namespace file_server_userver {

namespace {

class FileHandler final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-fileHandler";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {
    return file_server_userver::getFile(request.GetArg("name"));
  }
};

} // namespace

std::string getFile(std::string_view name) {
  if (name.empty()) {
    name = "unknown user";
  }

  return fmt::format("Hello, this is file server!\n", name);
}

void AppendGetFile(userver::components::ComponentList &component_list) {
  component_list.Append<FileHandler>();
}

} // namespace file_server_userver
