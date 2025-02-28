#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "hello.hpp"
#include "new_command.hpp"
#include "fileHandler.hpp"
#include "users/authorization.hpp"
#include "users/registration.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::clients::dns::Component>()
                            .Append<userver::server::handlers::TestsControl>();

  file_server_userver::AppendHello(component_list);
  file_server_userver::AppendNewCommand(component_list);
  file_server_userver::AppendGetFile(component_list);
  file_server_userver::AppendSignup(component_list);
  file_server_userver::AppendSignin(component_list);
  
  return userver::utils::DaemonMain(argc, argv, component_list);
}
