//
//  thriteen.h
//  thirteen
//
//  Created by Steve Baker on 4/24/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/local_time/local_time_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <exception>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <ostream>
#include <pwd.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include "SpreadsheetService.h"
#include "CellRange.h"

namespace po=boost::program_options;
namespace fs=boost::filesystem;
namespace re=boost::xpressive;

using boost::bad_lexical_cast;
using boost::format;
using boost::gregorian::date;
using boost::gregorian::date_facet;
using boost::gregorian::from_string;
using boost::iequals;
using boost::lexical_cast;
using boost::posix_time::second_clock;
using boost::posix_time::time_facet;
using boost::to_upper_copy;
using Google::Spreadsheet::CellRange;
using Google::Spreadsheet::Cell;
using Google::Spreadsheet::SpreadsheetService;
using Google::Spreadsheet::Spreadsheet;
using Google::Spreadsheet::Worksheet;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::ifstream;
using std::left;
using std::locale;
using std::map;
using std::ostream;
using std::right;
using std::setfill;
using std::setw;
using std::stringstream;
using std::string;
using std::uppercase;
using std::vector;

// Program Options
struct Options {
  std::string user;                   // Google username
  std::string password;               // Google password
  std::string callsign;               // Sender callsign
  int sequence;                       // Message sequence number
  std::string from;                   // FM address
  std::string to;                     // TO address
  std::string info;                   // INFO address
  std::string subject;                // Message subject
  std::string spreadsheet_name;       // Name of spreadsheet to use
  std::string spreadsheet_id;         // Id of spreadsheet to use
  std::string worksheet_name;         // Name of worksheet to use
  std::string worksheet_id;           // Id of worksheet to use
  std::string start_date;             // Start date for subject
  std::string stop_date;              // Stop date for subject
};

// Function declarations
bool CompareCallsigns(string a, string b);
Options GetOptions(int argc, char * const argv[]);
void PrintReport(ostream& out, const Options& opt,
                 const map<string, int>& on_air,
                 const map<string, int>& off_air);
void PrintParticipation(ostream& out, const map<string, int>& participation);
string ExpandDirectory(const char *directory);

// States for the state machine
enum State {NONE, ONAIR, OFFAIR};

// Version Number
const std::string VERSION = "2.0";

// Headings
const std::string ON_AIR_HEADER  = "ON AIR PARTICIPATION";
const std::string OFF_AIR_HEADER = "OFF AIR PARTICIPATION";

