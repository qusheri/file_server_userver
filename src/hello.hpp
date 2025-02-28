#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace file_server_userver {

std::string SayHelloTo(std::string_view name);

void AppendHello(userver::components::ComponentList &component_list);

} // namespace file_server_userver
