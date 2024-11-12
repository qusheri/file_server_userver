#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace file_server_userver {

std::string signin(std::string_view name, std::string_view password);

void AppendSignin(userver::components::ComponentList &component_list);

} // namespace file_server_userver
