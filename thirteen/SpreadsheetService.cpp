//
//  SpreadsheetService.cpp
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "SpreadsheetService.h"

namespace Google
{
namespace Spreadsheet
{

using rapidxml::xml_document;
using rapidxml::xml_node;
using std::exception;
using std::istringstream;
using std::ostringstream;
using std::pair;
using std::string;
using std::vector;

const int auth_offset = 5;
const int line_buffer_size = 500;

//------------------------------------------------------------------------------
// Purpose    : Log in to Google
// Parameters : email    - The email address to log in as
//            : password - The account password
//            : source   - The application logging on
// Returns    : Authorization string
//------------------------------------------------------------------------------
SpreadsheetService::SpreadsheetService(string email, string password,
                                       string source)
{

  const string address = "https://www.google.com/accounts/ClientLogin";
  const string account_type = "GOOGLE";

  curl_ = new Curl::Curl();

  // Get the strings from the url
  ostringstream url;
  url << address
      << "?accountType=" << account_type
      << "&Email=" << email
      << "&Passwd=" << password
      << "&service=wise&source=" << source;
  curl_->SetURL(url.str());
  string response = curl_->Execute();

  // Parse the result for the authorization
  string auth;
  istringstream result(response);
  char line[line_buffer_size];
  while(result.getline(line, line_buffer_size)) {
    if(!strncmp(line, "Auth=", auth_offset)) {
      auth = line+auth_offset;
      break;
    }
  }

  // Put the authorization in the headers
  vector<string> headers;
  headers.push_back("GData-Version: 2.0");
  headers.push_back(string("Authorization: GoogleLogin auth=" + auth).c_str());
  curl_->SetHeaders(headers);
}

//------------------------------------------------------------------------------
// Purpose    : Desctructor
//------------------------------------------------------------------------------
SpreadsheetService::~SpreadsheetService()
{
  delete curl_;
}

//------------------------------------------------------------------------------
// Purpose    : Get a list of spreadsheets available
// Returns    : vector<Spreadsheet> of available spreadsheets
//------------------------------------------------------------------------------
vector<Spreadsheet> SpreadsheetService::GetSpreadsheets()
{
  string url =
    "https://spreadsheets.google.com/feeds/spreadsheets/private/full";
  curl_->SetURL(url);
  string response = curl_->Execute();

  // Get the response into a buffer we can modify
  vector<char> xml(response.begin(), response.end());
  xml.push_back('\0');

  // Parse the response
  xml_document<> doc;
  doc.parse<0>(&xml[0]);

  // Create a vector of spreadsheet descriptors
  vector<Spreadsheet> sheets;
  xml_node<> *root = doc.first_node("feed");
  for (xml_node<> *entry = root->first_node("entry");
       entry;
       entry = entry->next_sibling("entry")) {
    Spreadsheet sheet(curl_);
    for (xml_node<> *node = entry->first_node();
         node;
         node = node->next_sibling()) {
      if(!strcmp(node->name(), "id")) {
        sheet.SetId(node->value());
        size_t found=sheet.GetId().rfind("/");
        if (found!=string::npos) {
          sheet.SetKey(sheet.GetId().substr(found+1));
        }
        continue;
      }
      if(!strcmp(node->name(), "title")) {
        sheet.SetTitle(node->value());
        continue;
      }
      if(!strcmp(node->name(), "updated")) {
        sheet.SetUpdated(node->value());
        continue;
      }
    }
    sheets.push_back(sheet);
  }

  return sheets;
}

//------------------------------------------------------------------------------
// Purpose    : Get a specific spreadsheet
// Parameters : id - Spreadsheet Id
// Returns    : The requested spreadsheet
//------------------------------------------------------------------------------
Spreadsheet SpreadsheetService::GetSpreadsheet(string id)
{
  Spreadsheet sheet(curl_);
  string url =
    "https://spreadsheets.google.com/feeds/spreadsheets/private/full/" + id;
  curl_->SetURL(url);
  string response = curl_->Execute();

  // Get the response into a buffer we can modify
  vector<char> xml(response.begin(), response.end());
  xml.push_back('\0');

  // Parse the response
  xml_document<> doc;
  doc.parse<0>(&xml[0]);


  xml_node<> *entry = doc.first_node("entry");
  for (xml_node<> *node = entry->first_node();
       node;
       node = node->next_sibling()) {
    if(!strcmp(node->name(), "id")) {
      sheet.SetId(node->value());
      size_t found=sheet.GetId().rfind("/");
      if (found!=string::npos) {
        sheet.SetKey(sheet.GetId().substr(found+1));
      }
      continue;
    }
    if(!strcmp(node->name(), "title")) {
      sheet.SetTitle(node->value());
      continue;
    }
    if(!strcmp(node->name(), "updated")) {
      sheet.SetUpdated(node->value());
      continue;
    }
  }

  return sheet;
}

}
}