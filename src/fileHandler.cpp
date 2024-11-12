#include "fileHandler.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/utils/text.hpp>
#include "db_funcs/sql_class.hpp"

#include <fstream>

namespace file_server_userver {

namespace {

class FileHandler final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-uploadFile";

  using HttpHandlerBase::HttpHandlerBase;

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override { 
    auto header = request.GetHeader(userver::http::headers::kContentType);
    const auto content_type = userver::http::ContentType(header);
    using namespace userver;
    if (content_type != "multipart/form-data") {
        request.GetHttpResponse().SetStatus(server::http::HttpStatus::kBadRequest);
        return "Expected 'multipart/form-data' content type";
    }
    const auto& image = request.GetFormDataArg("profileImage");

    const auto& fileArg = request.GetFormDataArg("profileImage");
    std::string fileName;

    if(!fileArg.filename){
      fileName = "errorFile.txt";
    }
    else fileName = *(fileArg.filename);  
    
    SQL db;
    db.addFile(fileName, "18", "52");

    const std::string savePath = fmt::format("/home/kamilg/vsc_cpp/file_server_userver/sendedFilesTest/{}", fileName);
    std::ofstream file(savePath, std::ios::binary);
    if (!file) {
        request.GetHttpResponse().SetStatus(server::http::HttpStatus::kInternalServerError);
        return "Failed to open file for writing";
    }
    // file.write(image.value.data(), image.value.size());
    // file.close();


    request.GetHttpResponse().SetContentType(http::content_type::kApplicationJson);
    return fmt::format("city=NIZHNEKAMSK, image_size={}\n", image.value.size());
  }
};

} // namespace

std::string getFile(std::string_view token) {
  if (token.empty()) {
    token = "unknown user";
  }

  return fmt::format("Hello, this is file server!\n", token);
}

void AppendGetFile(userver::components::ComponentList &component_list) {
  component_list.Append<FileHandler>();
}

} // namespace file_server_userver


// curl -v -F address='{"street": "3, Garden St", "city": "Hillsbery, UT"}' \
//           -F "profileImage=@src/file_divider/kitten.png" \
//           http://localhost:8080/uploadFile