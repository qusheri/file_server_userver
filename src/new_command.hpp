#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace file_server_userver {

std::string NewCommand(std::string_view name, std::string_view age);

void AppendNewCommand(userver::components::ComponentList &component_list);

} // namespace file_server_userver
