//
//  CellRange.cpp
//  thirteen
//
//  Created by Steve Baker on 4/19/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <vector>
#include "CellRange.h"

namespace Google
{
namespace Spreadsheet
{

using std::vector;

//------------------------------------------------------------------------------
// Purpose    : Construct a CellRange Object
// Parameters : begin_row - First row
//            : end_row   - Last Row
//            : begin_col - First Column
//            : end_col   - Last Column
//------------------------------------------------------------------------------
CellRange::CellRange(int begin_row, int end_row, int begin_col, int end_col) :
  begin_row_(begin_row),
  end_row_(end_row),
  begin_col_(begin_col),
  end_col_(end_col),
  cells_(NULL)
{

  int rows = GetRowCount();
  int cols = GetColCount();
  cells_ = new vector< vector<Cell *> >(rows, vector<Cell *>(cols, NULL));

}

//------------------------------------------------------------------------------
// Purpose    : Destroy a CellRange Object
//------------------------------------------------------------------------------
CellRange::~CellRange()
{

  // Delete any cells we have
  for (size_t row=0; row < cells_->size(); ++row) {
    for(size_t col=0; col < (*cells_)[row].size(); ++col) {
      delete (*cells_)[row][col];
    }
  }

  // Delete the cells vector
  delete cells_;
}

}
}