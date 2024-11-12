#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace file_server_userver {

std::string getFile(std::string_view token);

void AppendGetFile(userver::components::ComponentList &component_list);

} // namespace file_server_userver