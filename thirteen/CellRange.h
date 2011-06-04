#ifndef GUARD_CellRange_h
#define GUARD_CellRange_h
//
//  CellRange.h
//  thirteen
//
//  Created by Steve Baker on 4/19/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <vector>
#include "Cell.h"

namespace Google
{
namespace Spreadsheet
{

//------------------------------------------------------------------------------
// Class CellRange
//------------------------------------------------------------------------------
class CellRange
{
  int begin_row_, end_row_, begin_col_, end_col_;
  std::vector< std::vector<Cell *> > *cells_;

public:
  CellRange(int begin_row, int end_row, int begin_col, int end_col);
  ~CellRange();
  void SetCell(int row, int col, Cell *cell) {
    (*cells_)[row-begin_row_][col-begin_col_] = cell;
  }
  Cell *GetCell(int row, int col) {
    return (*cells_)[row-begin_row_][col-begin_col_];
  }
  int GetRowCount() {
    return 1+end_row_-begin_row_;
  }
  int GetColCount() {
    return 1+end_col_-begin_col_;
  }
  int GetFirstRow() {
    return begin_row_;
  }
  int GetLastRow() {
    return end_row_;
  }
  int GetFirstCol() {
    return begin_col_;
  }
  int GetLastCol() {
    return end_col_;
  }
};

}
}

#endif
