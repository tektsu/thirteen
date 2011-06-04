#ifndef GUARD_Spreadsheet_h
#define GUARD_Spreadsheet_h
//
//  Spreadsheet.h
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <string>
#include <vector>
#include "Curl.h"
#include "Worksheet.h"

namespace Google
{
namespace Spreadsheet
{

//------------------------------------------------------------------------------
// Class Spreadsheet
//------------------------------------------------------------------------------
class Spreadsheet
{
  Curl::Curl *curl_;
  std::string id_;
  std::string title_;
  std::string updated_;
  std::string key_;

public:
  Spreadsheet() : curl_(NULL) {}
  Spreadsheet(Curl::Curl *curl) : curl_(curl) {};
  void SetId(std::string id) {
    id_ = id;
  }
  const std::string& GetId() {
    return id_;
  }
  void SetTitle(std::string title) {
    title_ = title;
  }
  const std::string& GetTitle() {
    return title_;
  }
  void SetUpdated(std::string updated) {
    updated_ = updated;
  }
  const std::string& GetUpdated() {
    return updated_;
  }
  void SetKey(std::string key) {
    key_ = key;
  }
  const std::string& GetKey() {
    return key_;
  }

  std::vector<Worksheet> GetWorksheets();
  Worksheet GetWorksheet(std::string id);
};

}
}

#endif
