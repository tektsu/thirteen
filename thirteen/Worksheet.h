#ifndef GUARD_Worksheet_h
#define GUARD_Worksheet_h
//
//  Worksheet.h
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <string>
#include <vector>
#include "Curl.h"
#include "CellRange.h"

namespace Google
{
namespace Spreadsheet
{

//------------------------------------------------------------------------------
// Class Worksheet
//------------------------------------------------------------------------------
class Worksheet
{
  Curl::Curl *curl_;
  std::string id_;
  std::string title_;
  std::string updated_;
  std::string key_;
  std::string spreadsheet_key_;

public:
  Worksheet() : curl_(NULL), spreadsheet_key_("") {};
  Worksheet(Curl::Curl *curl, std::string spreadsheet_key)
    : curl_(curl), spreadsheet_key_(spreadsheet_key) {};
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

  CellRange GetCells();
  CellRange GetCells(int begin_row, int end_row, int begin_col, int end_col);
  CellRange GetColumns(int begin_col, int end_col);
  CellRange GetRows(int begin_row, int end_row);
};

}
}

#endif
