#ifndef GUARD_Cell_h
#define GUARD_Cell_h
//
//  Cell.h
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <iostream>
#include <string>
#include "Curl.h"

namespace Google
{
namespace Spreadsheet
{

//------------------------------------------------------------------------------
// Class Cell
//------------------------------------------------------------------------------
class Cell
{
  Curl::Curl *curl_;
  std::string id_;
  std::string title_;
  int row_;
  int col_;
  std::string updated_;
  std::string content_;
  std::string key_;
  std::string worksheet_key_;
  std::string spreadsheet_key_;

public:
  Cell() : curl_(NULL) {}
  Cell(Curl::Curl *curl, std::string spreadsheet_key, std::string worksheet_key)
    : curl_(curl), spreadsheet_key_(spreadsheet_key),
      worksheet_key_(worksheet_key) {};
  ~Cell() {}
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
  void SetRow(int row) {
    row_ = row;
  }
  int GetRow() {
    return row_;
  }
  void SetCol(int col) {
    col_ = col;
  }
  int GetCol() {
    return col_;
  }
  void SetContent(std::string content) {
    content_ = content;
  }
  const std::string& GetContent() {
    return content_;
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
};

#endif

}
}