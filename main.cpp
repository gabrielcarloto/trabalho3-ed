#include "Menu.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Main {
public:
  Main() {
    if (!std::filesystem::exists(filesPath)) {
      throw std::runtime_error("Nenhum arquivo disponível");
    }
  }

  void run() {
    auto filePath = filesMenu();

    if (filePath == "") {
      return;
    }

    populateListsWithFileContents(filePath);
    addMenuOptions();
  }

  struct Position {
    size_t line = 0;
    size_t column = 0;
  };

private:
  Menu menu;
  std::filesystem::path filesPath = "./arquivos/";

  std::vector<std::string> lines;
  std::vector<std::pair<std::string, std::vector<Position>>> invertedIndex;

  std::filesystem::path filesMenu() {
    std::filesystem::current_path(filesPath);
    filesPath = "./";

    std::vector<std::filesystem::directory_entry> files;
    unsigned int chosenFileIndex = 0;
    Menu menu;

    menu.setTitle("Arquivo");

    for (auto const &file : std::filesystem::directory_iterator(filesPath)) {
      if (!file.is_directory()) {
        files.push_back(file);
        menu.addOption(file.path().generic_string(),
                       [&chosenFileIndex](auto op) { chosenFileIndex = op; });
      }
    }

    menu.display();

    return files[chosenFileIndex].path();
  }

  void populateListsWithFileContents(std::filesystem::path &path) {
    std::string line;
    std::ifstream file(path);

    if (!file.is_open()) {
      throw std::runtime_error("Main::populateListsWithFileContents -> Não foi "
                               "possível abrir o arquivo");
    }

    size_t lineIndex = 1;

    while (getline(file, line)) {
      lines.push_back(line);
      std::istringstream iss(line);
      std::string word;

      size_t columnIndex = 1;

      while (iss >> word) {
        auto it = std::find_if(invertedIndex.begin(), invertedIndex.end(),
                               [&word](auto w) { return w.first == word; });

        if (it == invertedIndex.end()) {
          invertedIndex.emplace_back(word, std::vector<Position>());
          invertedIndex.back().second.push_back({lineIndex, columnIndex});
        } else {
          (*it).second.push_back({lineIndex, columnIndex});
        }

        columnIndex += word.size() + 1;
      }

      lineIndex++;
    }

    std::sort(invertedIndex.begin(), invertedIndex.end(),
              [](auto a, auto b) { return a.first < b.first; });

    file.close();
    menu.clear();
  }

  void addMenuOptions() {
    menu.setTitle("Opções");

    menu.addOption("Procurar palavra", [this](auto) { searchWord(); });
    menu.addOption("Imprimir arquivo invertido",
                   [this](auto) { printInvertedIndex(); });

    menu.loopDisplay();
  }

  void printInvertedIndex() {
    std::string strToPrint = "";

    for (auto &index : invertedIndex) {
      strToPrint += index.first + '\t';
      auto &ocurrences = index.second;

      for (auto &position : ocurrences) {
        strToPrint += std::to_string(position.line) + ':' +
                      std::to_string(position.column) + ' ';
      }

      strToPrint += '\n';
    }

    std::cout << strToPrint;
  }

  void searchWord() {
    std::string strToSearch;
    std::cout << "Digite a palavra a ser procurada: ";
    std::cin >> strToSearch;

    auto ocurrencesIndex = binarySearch(strToSearch);

    if (ocurrencesIndex < 0) {
      std::cout << "Nenhuma ocorrencia encontrada\n\n";
      return;
    }

    std::cout << "Ocorrências encontradas: ";

    for (auto &ocurrence : invertedIndex[ocurrencesIndex].second) {
      std::cout << ocurrence.line << ':' << ocurrence.column << ' ';
    }

    std::cout << "\n\n";
  }

  ssize_t binarySearch(std::string &word) {
    ssize_t start = 0, end = static_cast<ssize_t>(invertedIndex.size()) - 1;

    while (start <= end) {
      ssize_t mid = (start + end) / 2;
      auto currentWord = invertedIndex[mid].first;

      if (currentWord == word)
        return mid;

      if (currentWord < word) {
        start = mid + 1;
      } else {
        end = mid - 1;
      }
    }

    return -1;
  }
};

int main() {
  Main main;
  main.run();
  return 0;
}
