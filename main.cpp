
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

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
vector<string> scanDirectory() {
  vector<string> files;
  for (auto &entry : fs::directory_iterator(".")) {
    if (entry.path().extension() == ".csv") {
      files.push_back(entry.path().filename().string());
    }
  }
  return files;
}
void sortDate(vector<CSVModel> &model) {
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

void calculate_stats(const vector<CSVModel> &data, int rowsProcessed,
                     int rowsSkipped) {
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
        trough_date = data[i].date; // date of the worst point so far
      }
    }
  }

  cout << "\nMean daily return : " << stats.mean_daily_return * 100 << "%"
       << "\nAnnualised vol    : " << stats.annualised_vol * 100 << "%";
  cout << "\nMax drawdown      : " << stats.max_drawdown * 100 << "%"
       << "  (trough: " << trough_date << ")\n";
}
void parseFile(const string fileName, vector<CSVModel> &data) {
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
  sortDate(data);
  calculate_stats(data, rowsProcessed, rowsSkipped);
}

int main() {
  vector<string> files = scanDirectory();
  if (files.empty()) {
    cout << "No CSV files found in current directory\n";
    return 1;
  }
  int choice = selectCSV(files);
  string filename = files[choice];
  vector<CSVModel> data{};
  parseFile(filename, data);
  return 0;
}
