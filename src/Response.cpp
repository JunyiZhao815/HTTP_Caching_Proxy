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

std::string Response::getDate() {
  checkHasField("date");
  return header["date"];
}

std::string Response::message2string() {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}
