//
//  Worksheet.cpp
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "Curl.h"
#include "Worksheet.h"

namespace Google
{
namespace Spreadsheet
{

using rapidxml::xml_document;
using rapidxml::xml_node;
using std::string;
using std::vector;

//------------------------------------------------------------------------------
// Purpose    : Get an array of all cells in this worksheet
// Returns    : CellRange object
//------------------------------------------------------------------------------
CellRange Worksheet::GetColumns(int begin_col, int end_col)
{
  return GetCells(-1, -1, begin_col, end_col);
}

//------------------------------------------------------------------------------
// Purpose    : Get an array of all cells in this worksheet
// Returns    : CellRange object
//------------------------------------------------------------------------------
CellRange Worksheet::GetRows(int begin_row, int end_row)
{
  return GetCells(begin_row, end_row, -1, -1);
}

//------------------------------------------------------------------------------
// Purpose    : Get an array of all cells in this worksheet
// Returns    : CellRange object
//------------------------------------------------------------------------------
CellRange Worksheet::GetCells()
{
  return GetCells(-1, -1, -1, -1);
}

//------------------------------------------------------------------------------
// Purpose    : Get an array of a range of cells in this worksheet
// Parameters : begin_row - First row
//            : end_row   - Last Row
//            : begin_col - First Column
//            : end_col   - Last Column
// Returns    : CellRange object
//------------------------------------------------------------------------------
CellRange Worksheet::GetCells(int begin_row, int end_row,
                              int begin_col, int end_col)
{
  string url
  = "https://spreadsheets.google.com/feeds/cells/"
    + spreadsheet_key_ + "/" + key_ + "/private/full";
  curl_->SetURL(url);
  string response = curl_->Execute();

  // Get the response into a buffer we can modify
  vector<char> xml(response.begin(), response.end());
  xml.push_back('\0');

  // Parse the response
  xml_document<> doc;
  doc.parse<0>(&xml[0]);

  // Find the root node
  xml_node<> *root = doc.first_node("feed");

  // Determine the cell range
  if (begin_row == -1) {
    begin_row = 1;
  }
  if (end_row == -1) {
    end_row = atoi(root->first_node("gs:rowCount")->value());
  }
  if (begin_col == -1) {
    begin_col = 1;
  }
  if (end_col == -1) {
    end_col = atoi(root->first_node("gs:colCount")->value());
  }
  CellRange cells(begin_row, end_row, begin_col, end_col);

  // Fill in the cells we want
  for (xml_node<> *entry = root->first_node("entry");
       entry;
       entry = entry->next_sibling("entry")) {
    string id;
    string title;
    string content;
    string updated;
    int row = -1;
    int col = -1;
    for (xml_node<> *node = entry->first_node();
         node;
         node = node->next_sibling()) {
      if(!strcmp(node->name(), "id")) {
        id = node->value();
        continue;
      }
      if(!strcmp(node->name(), "title")) {
        title = node->value();
        continue;
      }
      if(!strcmp(node->name(), "content")) {
        content = node->value();
        continue;
      }
      if(!strcmp(node->name(), "updated")) {
        updated = node->value();
        continue;
      }
      if(!strcmp(node->name(), "gs:cell")) {
        row = atoi(node->first_attribute("row")->value());
        col = atoi(node->first_attribute("col")->value());
        continue;
      }
    }
    if(row >= begin_row && row <= end_row &&
        col >= begin_col && col <= end_col) {
      Cell *cell = new Cell(curl_, spreadsheet_key_, key_);
      cell->SetId(id);
      cell->SetTitle(title);
      cell->SetContent(content);
      cell->SetUpdated(updated);
      cell->SetRow(row);
      cell->SetCol(col);
      cells.SetCell(row, col, cell);
    }
  }

  return cells;
}

}
}