//
//  Spreadsheet.cpp
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "Spreadsheet.h"

namespace Google
{
namespace Spreadsheet
{

using rapidxml::xml_document;
using rapidxml::xml_node;
using std::string;
using std::vector;

//------------------------------------------------------------------------------
// Purpose    : Get a list of worksheets for this spreadsheet
// Returns    : Vector of Worksheet objects
//------------------------------------------------------------------------------
vector<Worksheet> Spreadsheet::GetWorksheets()
{
  string url =
    "https://spreadsheets.google.com/feeds/worksheets/" + key_ + "/private/full";
  curl_->SetURL(url);
  string response = curl_->Execute();

  // Get the response into a buffer we can modify
  vector<char> xml(response.begin(), response.end());
  xml.push_back('\0');

  // Parse the response
  xml_document<> doc;
  doc.parse<0>(&xml[0]);

  // Create a vector of worksheet descriptors
  vector<Worksheet> sheets;
  xml_node<> *root = doc.first_node("feed");
  for (xml_node<> *entry = root->first_node("entry");
       entry;
       entry = entry->next_sibling("entry")) {
    Worksheet sheet(curl_, key_);
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
// Purpose    : Get a specific worksheet
// Parameters : id - Worksheet Id
// Returns    : The requested worksheet
//------------------------------------------------------------------------------
Worksheet Spreadsheet::GetWorksheet(string id)
{
  string url
  = "https://spreadsheets.google.com/feeds/worksheets/"
    + key_ + "/private/full/" + id;
  curl_->SetURL(url);
  string response = curl_->Execute();

  // Get the response into a buffer we can modify
  vector<char> xml(response.begin(), response.end());
  xml.push_back('\0');

  // Parse the response
  xml_document<> doc;
  doc.parse<0>(&xml[0]);

  xml_node<> *entry = doc.first_node("entry");
  Worksheet sheet(curl_, key_);
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