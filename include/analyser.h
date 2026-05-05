#pragma once

#include <string>
#include <vector>
using namespace std;
struct Statistics {
  double mean_daily_return{};
  double annualised_vol{};
  double max_drawdown{};
};
struct CSVModel {
  string date{};
  string open{};
  string high{};
  string low{};
  string close{};
  string volume{};
};

vector<string> scanDirectory();
void printDate(vector<CSVModel> &model);
int selectCSV(const vector<string> &files);
void writeCSV(vector<pair<string, double>> rolling_vol, const string &symbol);
void calculate_stats(const vector<CSVModel> &data, int rowsProcessed,
                     const string &symbol);
void parseFile(const string fileName, vector<CSVModel> &data, string &symbol);
