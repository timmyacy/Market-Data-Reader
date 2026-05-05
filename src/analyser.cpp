#include "analyser.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

vector<string> scanDirectory() {
  vector<string> files;
  for (auto &entry : fs::directory_iterator(".")) {
    if (entry.path().extension() == ".csv") {
      files.push_back(entry.path().filename().string());
    }
  }
  return files;
}

void printDate(vector<CSVModel> &model) {
  cout << " The dates are from " << model.back().date << " to "
       << model.front().date;
}

int selectCSV(const vector<string> &files) {
  int selected = 0;
  int total = files.size();

  while (true) {
    cout << "\033[2J\033[H";
    cout << "Select CSV file:\n\n";
    for (int i = 0; i < total; i++) {
      cout << (i == selected ? " > " : "   ") << files[i] << "\n";
    }

    system("stty raw -echo");
    char c = getchar();
    system("stty cooked echo");

    if (c == '\033') {
      getchar();
      char arrow = getchar();
      if (arrow == 'A')
        selected = max(0, selected - 1);
      if (arrow == 'B')
        selected = min(total - 1, selected + 1);
    }
    if (c == '\n' || c == '\r')
      break;
  }
  return selected;
}

void writeCSV(vector<pair<string, double>> rolling_vol, const string &symbol) {
  ofstream out(symbol + " rolling_vol.csv");
  out << "Date, Rolling Volatility\n";
  for (auto &[date, vol] : rolling_vol)
    out << date << "," << vol << "\n";
}

void calculate_stats(const vector<CSVModel> &data, int rowsProcessed,
                     const string &symbol) {
  Statistics stats{};
  vector<double> daily_returns;

  for (int i = 1; i < (int)data.size(); i++) {
    if (!data[i].open.empty() && !data[i - 1].close.empty()) {
      double today_price = stod(data[i].open);
      double yesterday_price = stod(data[i - 1].close);
      daily_returns.push_back((today_price - yesterday_price) /
                              yesterday_price);
    }
  }

  int n = (int)daily_returns.size();

  double sum{};
  for (double r : daily_returns) {
    sum += r;
  }
  stats.mean_daily_return = sum / n;

  double variance{};
  for (double r : daily_returns) {
    variance += pow(r - stats.mean_daily_return, 2);
  }
  stats.annualised_vol = sqrt(variance / (n - 1)) * sqrt(252);

  double peak = stod(data[0].close);
  string trough_date{};

  for (int i = 1; i < (int)data.size(); i++) {
    if (!data[i].close.empty()) {
      double close = stod(data[i].close);
      if (close > peak)
        peak = close;
      double dd = (peak - close) / peak;
      if (dd > stats.max_drawdown) {
        stats.max_drawdown = dd;
        trough_date = data[i].date;
      }
    }
  }

  cout << "\nMean daily return : " << stats.mean_daily_return * 100 << "%"
       << "\nAnnualised vol    : " << stats.annualised_vol * 100 << "%";
  cout << "\nMax drawdown      : " << stats.max_drawdown * 100 << "%"
       << "  (trough: " << trough_date << ")\n";

  vector<pair<string, double>> rolling_vol;

  for (int i = 19; i < (int)daily_returns.size(); i++) {
    double w_mean{};
    for (int j = i - 19; j <= i; j++) {
      w_mean += daily_returns[j];
    }
    w_mean /= 20.0;

    double w_var{};
    for (int j = i - 19; j <= i; j++) {
      w_var += pow(daily_returns[j] - w_mean, 2);
    }
    w_var /= 19.0;

    double vol = sqrt(w_var) * sqrt(252);
    rolling_vol.push_back({data[i + 1].date, vol});
  }
  writeCSV(rolling_vol, symbol);
}

void parseFile(const string fileName, vector<CSVModel> &data, string &symbol) {
  ifstream file(fileName);
  int rowsProcessed{0};
  int rowsSkipped{0};

  if (!file.is_open()) {
    cout << "Failed to open file";
    return;
  }
  if (file.is_open()) {
    string line;
    while (getline(file, line)) {
      stringstream ss(line);
      string token;
      CSVModel model{};
      if (!isdigit(line[0]) && line[0] != '-') {
        rowsSkipped++;
        continue;
      }
      getline(ss, token, ',');
      model.date = token;

      getline(ss, token, ',');
      model.close = token;
      getline(ss, token, ',');
      model.high = token;
      getline(ss, token, ',');
      model.low = token;
      getline(ss, token, ',');
      model.open = token;
      getline(ss, token, ',');
      model.volume = token;
      rowsProcessed++;
      data.push_back(model);
    }
  }
  printDate(data);
  calculate_stats(data, rowsProcessed, symbol);
}
