#include "Response.h"

Response::Response(const unsigned int version, const std::string &status_code,
                   const std::string &reason,
                   std::map<std::string, std::string> &header,
                   std::string &body)
    : HttpMessage(version, header, body), status_code(status_code),
      reason(reason) {}

std::ostream &operator<<(std::ostream &out, const Response response) {
  // start line
  out << response.getVersion() << " " << response.status_code << " "
      << response.reason << "\r\n";

  // header
  out << response.header2string();
  // blank line
  out << "\r\n";
  // body
  out << response.body;
  return out;
}

std::string Response::getExpires() {
  checkHasField("expires");
  return header["expires"];
}

std::string Response::getLastModified() {
  checkHasField("last-modified");
  return header["last-modified"];
}

std::string Response::message2string() {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::string Response::getStatusCode() { return status_code; }

std::string Response::getEtag() {
  checkHasField("etag");
  return header["etag"];
}

std::string Response::getTransferEncoding() {
  checkHasField("transfer-encoding");
  return header["transfer-encoding"];
}

std::string Response::getAge() {
  checkHasField("age");
  return header["age"];
}

std::string Response::getWarning() {
  checkHasField("warn-code");
  return header["warn-code"];
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
