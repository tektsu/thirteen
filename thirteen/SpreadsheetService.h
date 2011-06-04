#ifndef GUARD_SpreadsheetService_h
#define GUARD_SpreadsheetService_h
//
//  SpreadsheetService.h
//  thirteen
//
//  Created by Steve Baker on 4/17/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <string>
#include <vector>
#include "Curl.h"
#include "Spreadsheet.h"

namespace Google
{
namespace Spreadsheet
{

//------------------------------------------------------------------------------
// Class SpreadsheetService
//------------------------------------------------------------------------------
class SpreadsheetService
{
  Curl::Curl *curl_;

public:
  SpreadsheetService(std::string email, std::string password,
                     std::string source);
  ~SpreadsheetService();
  std::vector<Spreadsheet> GetSpreadsheets();
  Spreadsheet GetSpreadsheet(std::string id);
};

}
}

#endif
