#include "Response.h"

std::string Response::getStartLine() const {
  std::string startLine =
      getVersion() + " " + status_code + " " + reason + "\r\n";
  return startLine;
}

Response::Response(const unsigned int version, const std::string &status_code,
                   const std::string &reason,
                   std::map<std::string, std::string> &header,
                   std::string &body)
    : HttpMessage(version, header, body), status_code(status_code),
      reason(reason) {}

std::ostream &operator<<(std::ostream &out, const Response response) {
  // start line
  out << response.getStartLine();

  // header
  out << response.header2string();
  // blank line
  out << "\r\n";
  // body
  out << response.body;
  return out;
}

std::string Response::getExpires() {
  try{
    checkHasField("expires");
    return header["expires"];
  }catch(std::invalid_argument &e){
    return "";
  }
}

std::string Response::getLastModified() {
  try{
    checkHasField("last-modified");
    return header["last-modified"];
  }catch(std::invalid_argument &e){
    return "";
  }
}

std::string Response::message2string() {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::string Response::getStatusCode() { return status_code; }

std::string Response::getEtag() {
  try{
    checkHasField("etag");
    return header["etag"];
  }catch(std::invalid_argument &e){
    return "";
  }
}

std::string Response::getTransferEncoding() {
  try{
    checkHasField("transfer-encoding");
    return header["transfer-encoding"];
  }catch(std::invalid_argument &e){
    return "";
  }
}

std::string Response::getAge() {
  try{
    checkHasField("age");
    return header["age"];
  }catch(std::invalid_argument &e){
    return "0";
  }
}

std::string Response::getWarning() {
  try{
    checkHasField("warn-code");
    return header["warn-code"];
  }catch(std::invalid_argument &e){
    return "";
  }
}

std::string Response::getCacheable() {
  std::string cache_control = getCacheControl();
  if (cache_control != "") {
    size_t no_store_index = cache_control.find("no-store");
    size_t private_index = cache_control.find("private");
    if (no_store_index != std::string::npos ||
        private_index != std::string::npos) {
      if (no_store_index != std::string::npos) {
        header["cacheable"] = "no-store";
      } else {
        header["cacheable"] = "private";
      }
    } else {
      header["cacheable"] = "yes";
    }
  }
  return header["cacheable"];
}

std::string Response::getFirstLine() {
  std::string firstLine = getVersion() + " " + status_code + " " + reason;
  return firstLine;
}

size_t Response::getContentLen() { return body.length(); }
