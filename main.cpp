
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;
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
void sortData(vector<CSVModel> &model) {
  cout << model.front().date << "\n" << model.back().date;
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
      model.volume = token;
      getline(ss, token, ',');
      model.open = token;
      getline(ss, token, ',');
      model.high = token;
      getline(ss, token, ',');
      model.low = token;
      rowsProcessed++;
      data.push_back(model);
    }
  }
  sortData(data);
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
