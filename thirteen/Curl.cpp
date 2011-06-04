//
//  curl.cpp
//  thirteen
//
//  Created by Steve Baker on 4/10/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <string>
#include <vector>
#include "curl.h"

namespace Curl
{

using std::string;
using std::vector;

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------
Curl::Curl() : handle_(NULL), url_(""), request_headers_(NULL)
{

  handle_ = curl_easy_init(); // Begin the session

  // Set options
  options_.header = 0;
  options_.follow_location = 1;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Curl::~Curl()
{
  curl_easy_cleanup(handle_);
}

//------------------------------------------------------------------------------
// Purpose    : Execute a curl call
// Parameters : None
// Returns    : Response body
//------------------------------------------------------------------------------
const string& Curl::Execute()
{
  string buffer;

  if(handle_) {
    curl_easy_setopt(handle_, CURLOPT_HEADER, options_.header);
    curl_easy_setopt(handle_, CURLOPT_FOLLOWLOCATION, options_.follow_location);
    curl_easy_setopt(handle_, CURLOPT_URL, url_.c_str());
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, MemoryWriter);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &buffer);
    if(request_headers_ != NULL) {
      curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, request_headers_);
    }
    CURLcode result = curl_easy_perform(handle_);
    if (result != CURLE_OK) {
      buffer = "";
    }
  }

  response_ = buffer;
  return response_;
}

//------------------------------------------------------------------------------
// Purpose    : Set headers
// Parameters : new_headers - A vector of new header strings
// Returns    : void
//------------------------------------------------------------------------------
void Curl::SetHeaders(const vector<string>& new_headers)
{
  for (int i=0; i<new_headers.size(); ++i) {
    request_headers_ =
      curl_slist_append(request_headers_, new_headers[i].c_str());
  }
}

//------------------------------------------------------------------------------
// Purpose    : Collect response in a buffer
// Parameters : data   - Pointer to the received data
//            : size   - The number of characters
//            : nmemb  - The size of each character
//            : buffer - Pointer to the destination buffer
// Returns    : Number of bytes processed
//------------------------------------------------------------------------------
int Curl::MemoryWriter(char *data, size_t size, size_t nmemb, string *buffer)
{
  if(buffer == NULL) {
    return 0;
  }
  buffer->append(data, size * nmemb);
  return static_cast<int>(size * nmemb);
}

}