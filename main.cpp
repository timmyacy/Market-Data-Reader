#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "analyser.h"

int main(int argc, char **argv) {

  int i;
  string symbol = "";
  vector<CSVModel> data{};
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) == "--symbol" && i + 1 < argc) {
      symbol = argv[i + 1];
      break;
    }
  }

  if (!symbol.empty()) {
    string filename = symbol + ".csv";
    parseFile(filename, data, symbol);
  } else {
    vector<string> files = scanDirectory();
    if (files.empty()) {
      cout << "No CSV files found in current directory\n";
      return 1;
    }
    int choice = selectCSV(files);
    string filename = files[choice];
    parseFile(filename, data, symbol);
  }
  return 0;
}
