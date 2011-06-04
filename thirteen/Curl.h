#ifndef GUARD_Curl_h
#define GUARD_Curl_h
//
//  curl.h
//  thirteen
//
//  Created by Steve Baker on 4/10/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <curl/curl.h>
#include <string>
#include <vector>

namespace Curl
{

struct CurlOptions {
  int header;
  int follow_location;
};

class Curl
{
  CURL *handle_;
  CurlOptions options_;
  std::string url_;
  struct curl_slist *request_headers_;
  std::string response_;

public:
  Curl();
  ~Curl();
  void SetURL(std::string url) {
    url_ = url;
  }
  const std::string& GetURL() {
    return url_;
  }
  void SetResponse(std::string response) {
    response_ = response;
  }
  void SetOptHeader(bool header) {
    options_.header = header ? 1 : 0;
  }
  bool GetOptHeader() {
    return options_.header ? true : false;
  }
  void SetOptFollowLocation(bool follow_location) {
    options_.follow_location = follow_location ? 1 : 0;
  }
  bool GetOptFollowLocation() {
    return options_.follow_location ? true : false;
  }
  const std::string& GetResponse() {
    return response_;
  }
  const std::string& Execute();
  void SetHeaders(const std::vector<std::string>& new_headers);

private:
  static int MemoryWriter(char *data, size_t size,
                          size_t nmemb, std::string *buffer);
};

}
#endif
