#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace file_server_userver {

std::string signup(std::string_view name, std::string_view password);

void AppendSignup(userver::components::ComponentList &component_list);

} // namespace file_server_userver
