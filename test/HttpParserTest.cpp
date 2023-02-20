#include "HttpParserTest.h"

void HttpParserTest::onRun() {
  test_parse_request();
  test_parse_response();
  test_equal_operator_request();
}

void HttpParserTest::test_equal_operator_request() {
  std::string request_msg =
      "GET /hello.htm HTTP/1.1\r\n"
      "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
      "Host: www.tutorialspoint.com\r\n"
      "Accept-Language: en-us\r\n"
      "Accept-Encoding: gzip, deflate\r\n"
      "Connection: Keep-Alive\r\n"
      "\r\n";
  HttpParser parser_1;
  Request *request_1 =
      parser_1.parseRequest(request_msg.c_str(), request_msg.length());

  HttpParser parser_2;
  Request *request_2 =
      parser_2.parseRequest(request_msg.c_str(), request_msg.length());
  assert(request_1 != request_2);
  assert(*request_1 == *request_2);
}

void HttpParserTest::test_parse_request() {
  // test parse GET request
  {
    HttpParser httpParser;
    std::string message =
        "GET /hello.htm HTTP/1.1\r\n"
        "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"
        "Host: www.tutorialspoint.com\r\n"
        "Accept-Language: en-us\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";

    size_t split_index = 47;
    std::string part_1 = message.substr(0, split_index);
    Request *ret = httpParser.parseRequest(part_1.c_str(), part_1.length());
    assert(ret == NULL);

    std::string part_2 = message.substr(split_index);
    ret = httpParser.parseRequest(part_2.c_str(), part_2.length());
    std::stringstream ss;
    ss << *ret;
    assert(ss.str().length() == message.length());
    assert(ret->getHost().first == "www.tutorialspoint.com");
    assert(ret->getHost().second == "80");
  }
}

void HttpParserTest::test_parse_response() {
  {
    HttpParser httpParser;
    std::string message =
        "HTTP/1.1 200 OK\r\n"
        "Age: 207498\r\n"
        "Cache-Control: max-age=604800\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Date: Sat, 20 Mar 2021 23:24:40 GMT\r\n"
        "Etag: \"3147526947+ident\"\r\n"
        "Expires: Sat, 27 Mar 2021 23:24:40 GMT\r\n"
        "Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT\r\n"
        "Server: ECS (bsa/EB15)\r\n"
        "Vary: Accept-Encoding\r\n"
        "Content-Length: 1256\r\n"
        "\r\n"
        "<!doctype html>\n<html>\n<head>\n    <title>Example "
        "Domain</title>\n\n  "
        "  <meta charset=\"utf-8\" />\n    <meta http-equiv=\"Content-type\" "
        "content=\"text/html; charset=utf-8\" />\n    <meta name=\"viewport\" "
        "content=\"width=device-width, initial-scale=1\" />\n    <style "
        "type=\"text/css\">\n    body {\n        background-color: #f0f0f2;\n  "
        "  "
        "    margin: 0;\n        padding: 0;\n        font-family: "
        "-apple-system, system-ui, BlinkMacSystemFont, \"Segoe UI\", \"Open "
        "Sans\", \"Helvetica Neue\", Helvetica, Arial, sans-serif;\n        \n "
        "  "
        " }\n    div {\n        width: 600px;\n        margin: 5em auto;\n     "
        "  "
        " padding: 2em;\n        background-color: #fdfdff;\n        "
        "border-radius: 0.5em;\n        box-shadow: 2px 3px 7px 2px "
        "rgba(0,0,0,0.02);\n    }\n    a:link, a:visited {\n        color: "
        "#38488f;\n        text-decoration: none;\n    }\n    @media "
        "(max-width: "
        "700px) {\n        div {\n            margin: 0 auto;\n            "
        "width: auto;\n        }\n    }\n    </style>    "
        "\n</head>\n\n<body>\n<div>\n    <h1>Example Domain</h1>\n    <p>This "
        "domain is for use in illustrative examples in documents. You may use "
        "this\n    domain in literature without prior coordination or asking "
        "for "
        "permission.</p>\n    <p><a "
        "href=\"https://www.iana.org/domains/example\">More "
        "information...</a></p>\n</div>\n</body>\n</html>\n";

    size_t split_len = 128;
    size_t pre_index = 0;
    std::string parsing = message.substr(pre_index, split_len);
    Response *res = httpParser.parseResponse(parsing.c_str(), parsing.length());
    while (res == NULL) {
      pre_index += split_len;
      parsing = message.substr(pre_index, split_len);
      res = httpParser.parseResponse(parsing.c_str(), parsing.length());
    }

    std::stringstream ss;
    ss << *res;
    assert(ss.str().length() == message.length());
    assert(*res == *res);
  }
}
