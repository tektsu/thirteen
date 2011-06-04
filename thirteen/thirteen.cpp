//
//  thirteen.cpp
//  thirteen
//
//  Created by Steve Baker on 4/8/11.
//  Copyright 2011 XECKO LLC. All rights reserved.
//

#include "thirteen.h"

//------------------------------------------------------------------------------
int main(int argc, char * const argv[])
{
  Options opt = GetOptions(argc, argv);

  // Log in
  SpreadsheetService service(opt.user, opt.password,
                             "xeckollc-thirteen-" + VERSION);

  // Get the spreadsheet we are interested in
  Spreadsheet spreadsheet;
  if(opt.spreadsheet_id.size()) {
    spreadsheet = service.GetSpreadsheet(opt.spreadsheet_id);
  } else {
    vector<Spreadsheet> sheets = service.GetSpreadsheets();
    int report_index = -1;
    for (int i=0; i < sheets.size(); ++i) {
      if(sheets[i].GetTitle() == opt.spreadsheet_name) {
        report_index = i;
        break;
      }
    }
    if(report_index < 0) {
      cerr << format("No spreadsheet named [%1%] found") %
           opt.spreadsheet_name << endl;
      return 1;
    }
    spreadsheet = sheets[report_index];
  }

  // Get the worksheet we are interested in. If we didn't specify one,
  // use the first one in the spreadsheet
  Worksheet worksheet;
  if(opt.worksheet_id.size()) {
    worksheet = spreadsheet.GetWorksheet(opt.worksheet_id);
  } else {
    vector<Worksheet> sheets = spreadsheet.GetWorksheets();
    if(opt.worksheet_name.size()) {
      int report_index = -1;
      for (int i=0; i < sheets.size(); ++i) {
        if(sheets[i].GetTitle() == opt.worksheet_name) {
          report_index = i;
          break;
        }
      }
      if(report_index < 0) {
        cerr << format("No worksheet named [%1%] found") %
             opt.worksheet_name << endl;
        return 1;
      }
      worksheet = sheets[report_index];
    } else {
      worksheet = sheets[0];
    }
  }

  // Get 2 columns of Cells from selected worksheet
  CellRange cells = worksheet.GetColumns(1, 2);

  // Parse the cells for the data
  map<string, int> on_air, off_air;
  State state = NONE;
  for (int row=cells.GetFirstRow(); row <= cells.GetLastRow(); ++row) {
    Cell *label_cell = cells.GetCell(row, 1);
    if(label_cell == NULL) {
      state = NONE;
      continue;
    }
    string label = label_cell->GetContent();
    if(!label.size()) {
      state = NONE;
      continue;
    }
    if(label == ON_AIR_HEADER) {
      state = ONAIR;
      continue;
    }
    if(label == OFF_AIR_HEADER) {
      state = OFFAIR;
      continue;
    }

    Cell *value_cell = cells.GetCell(row, 2);
    if(value_cell == NULL) {
      continue;
    }
    string value = value_cell->GetContent();
    int ivalue;
    try {
      ivalue = lexical_cast<int>(value.c_str());
    } catch(bad_lexical_cast &) {
      ivalue = 0;
    }
    switch (state) {
    case ONAIR:
      if(ivalue) {
        on_air[label] = ivalue;
      }
      break;

    case OFFAIR:
      if(ivalue) {
        off_air[label] = ivalue;
      }
      break;

    case NONE:
      break;
    }
  }

  // Figure the start and stop dates from the worksheet name
  string name = worksheet.GetTitle();
  re::sregex rex = re::sregex::compile("(.+) +(.+)");
  re::smatch matches;
  if(!re::regex_match(name, matches, rex)) {
    cerr << format("Worksheet name [%1%] does not like a month and year") %
         name << endl;
    return 1;
  }

  string mon, yr;
  int month, year;
  mon = matches[1];
  yr  = matches[2];
  year = lexical_cast<int>(yr.c_str());
  if(year < 2000) {
    year += 2000;
  }
  month = 0;
  string months[] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec"
  };
  for (int i=0; i<12; ++i) {
    if (iequals(mon, months[i])) {
      month=i+1;
      break;
    }
  }
  if(month < 1) {
    cerr << format("Worksheet name [%1%] does not contain a valid month") %
         name << endl;
    return 1;
  }
  date start_date(year, month, 1);
  date stop_date = start_date.end_of_month();

  stringstream start, stop;
  date_facet *facet = new date_facet("%d %b");
  start.imbue(locale(cout.getloc(), facet));
  start << start_date;
  opt.start_date = to_upper_copy(start.str());

  facet = new date_facet("%d %b %Y");
  stop.imbue(locale(cout.getloc(), facet));
  stop << stop_date;
  opt.stop_date = to_upper_copy(stop.str());

  // Print the report
  PrintReport(cout, opt, on_air, off_air);
  return 0;
}

//------------------------------------------------------------------------------
// Purpose    : Parse the command line and config file
// Parameters : argc, argv
// Returns    : Filled-in Options structure
//------------------------------------------------------------------------------
Options GetOptions(int argc, char * const argv[])
{

  // Read command line options
  Options opt;
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", "Show this message")
  ("callsign", po::value<string>(&opt.callsign), "Sender's Callsign")
  ("sequence", po::value<int>(&opt.sequence)->default_value(1),
   "Message Sequence Number")
  ("user", po::value<string>(&opt.user), "Google User Name")
  ("password", po::value<string>(&opt.password), "Google Password")
  ("from", po::value<string>(&opt.from), "FM Addee")
  ("to", po::value<string>(&opt.to), "TO Addee")
  ("info", po::value<string>(&opt.info), "INFO Addee")
  ("subject", po::value<string>(&opt.subject), "Message Subject")
  ("spreadsheet-name", po::value<string>(&opt.spreadsheet_name),
   "Spreadsheet Name")
  ("spreadsheet-id", po::value<string>(&opt.spreadsheet_id), "Spreadsheet ID")
  ("worksheet-name", po::value<string>(&opt.worksheet_name), "Worksheet Name")
  ("worksheet-id", po::value<string>(&opt.worksheet_id), "Worksheet ID")
  ;

  // Config file
  string config_file = ExpandDirectory("~/.thirteenrc");
  fs::path config_path(config_file);

  // Load configuration
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if(fs::exists(config_path)) {
      ifstream config_stream(config_file.c_str());
      po::store(po::parse_config_file(config_stream, desc), vm);
    }
  } catch(exception& e) {
    cerr << "Unable to parse command line: " << e.what() << endl;
    exit(1);
  }
  po::notify(vm);
  if (vm.count("help")) {
    cout << desc << "\n";
    exit(1);
  }

  // Test for errors
  vector<string> errors;
  if(!(opt.spreadsheet_name.size() || opt.spreadsheet_id.size())) {
    errors.push_back("Either a spreadsheet name or a spreadsheet id must be specified");
  }
  if(!opt.user.size()) {
    errors.push_back("No Google Docs user specified");
  }
  if(!opt.password.size()) {
    errors.push_back("No Google Docs password specified");
  }
  if(!opt.from.size()) {
    errors.push_back("No FM addee specified");
  }
  if(!opt.to.size()) {
    errors.push_back("No TO addee specified");
  }
  if(!opt.info.size()) {
    errors.push_back("No INFO addee specified");
  }
  if(!opt.subject.size()) {
    errors.push_back("No subject specified");
  }
  if(errors.size()) {
    cout << "Error:" << endl;
    BOOST_FOREACH(string error, errors) {
      cout << "  " << error << endl;
    }
    exit(1);
  }

  return opt;

}

//------------------------------------------------------------------------------
// Purpose    : Print the participation lines
// Parameters : participation - The map of callsigns and hours
//------------------------------------------------------------------------------
void PrintReport(ostream& out, const Options& opt,
                 const map<string, int>& on_air,
                 const map<string, int>& off_air)
{

  // Calculate the date time group
  stringstream dtg;
  time_facet *dtg_facet = new time_facet("R %d%H%MZ %b %Y");
  dtg.imbue(locale(cout.getloc(), dtg_facet));
  dtg << second_clock::universal_time();

  // Print the report
  out << format("DE %1% %2$03d") % opt.callsign % opt.sequence << endl;
  out << to_upper_copy(dtg.str()) << endl;
  out << format("FM %1%") % opt.from << endl;
  out << format("TO %1%") % opt.to << endl;
  out << format("INFO %1%") % opt.info << endl;
  out << "BT" << endl;
  out << "UNCLAS" << endl;
  out << format("SUBJ: %1% %2% THRU %3%")
      % opt.subject % opt.start_date % opt.stop_date << endl;
  out << "1. ON THE AIR PARTICIPATION" << endl;
  out << "READ IN TWO COLUMNS" << endl;
  PrintParticipation(out, on_air);
  out << "2. OFF THE AIR PARTICIPATION" << endl;
  out << "READ IN TWO COLUMNS" << endl;
  PrintParticipation(out, off_air);
  out << "BT" << endl;
  out << "NNNN" << endl;
}

//------------------------------------------------------------------------------
// Purpose    : Print the participation lines
// Parameters : participation - The map of callsigns and hours
//------------------------------------------------------------------------------
void PrintParticipation(ostream& out, const map<string, int>& participation)
{

  // Create a vector of callsigns
  vector<string> callsigns;
  for(map<string, int>::const_iterator it = participation.begin();
      it != participation.end(); ++it) {
    callsigns.push_back((*it).first);
  }

  // Print the data
  sort(callsigns.begin(), callsigns.end(), CompareCallsigns);
  for(vector<string>::iterator vt = callsigns.begin();
      vt != callsigns.end(); ++vt) {
    string callsign = *vt;
    //    out << setw(7) << left << setfill(' ') << callsign
    //    << setw(3) << right << participation.at(callsign) << endl;
    out << format("%1$-7s %2$3s") % callsign % participation.at(callsign)
        << endl;
  }
}

//------------------------------------------------------------------------------
// Purpose    : Compare MARS callsigns
// Parameters : a, b - The callsigns to compare
// Return     : true if a < b
// Note       : This differs from a standard compare in that string length is
//            : taken into account. A sort using this function will result in
//            : strings sorted by length, then alphabetically within eacg group.
//------------------------------------------------------------------------------
bool CompareCallsigns(string a, string b)
{
  if(a.size() != b.size()) {
    return a.size() < b.size();
  }
  return a < b;
}

//------------------------------------------------------------------------------
// Purpose    : Expand a directory name if needed
// Parameters : directory - The directory to expand
// Returns    : The expanded directory name
//------------------------------------------------------------------------------
string ExpandDirectory(const char *directory)
{

  string expanded_directory = directory;
  if (expanded_directory.compare(0, 2, "~/") != 0) {
    return expanded_directory;
  }

  struct passwd* passwd = getpwuid(geteuid());
  if (!passwd) {
    return expanded_directory;
  }

  expanded_directory = passwd->pw_dir;
  expanded_directory.append(directory+1);
  return expanded_directory;
}

