#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <cmath>

namespace fs = std::filesystem;

const int number_of_messages = 1000000;
int missed_logs_cnt = 0;

void drop_log(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }
    std::unordered_map<int, int> count_map;

    std::string line;
    std::regex number_pattern(R"(\b(\d+)\b$)"); // Match a number at the end of a line
    std::smatch match;

    while (std::getline(file, line)) {
        if (std::regex_search(line, match, number_pattern)) {
            try {
                int number = std::stoi(match[1].str());
                if (number >= 0 && number < number_of_messages) {
                    count_map[number]++;
                }
            }
            catch (...) {
                // Ignore lines that don't convert cleanly
            }
        }
    }
    for (int i = 0; i < number_of_messages; ++i) {
        int count = count_map[i];
        if (count != 5) {
            missed_logs_cnt += std::abs(count - 5);
        }
    }
}

void parse_log_files(const std::string& main_folder) {
    for (const auto& entry : fs::recursive_directory_iterator(main_folder)) {
        if (entry.is_regular_file() && entry.path().filename().string().ends_with("drop_rate.log")) {
            std::cout << "file was found: " << entry.path() << std::endl;
            drop_log(entry.path().string());
        }
    }
}

int main() {
    std::string main_folder = "D:/Iteration2/ColloLog";
    parse_log_files(main_folder);

    std::ofstream output("D:/Iteration2/ColloLog/drop_rate.txt", std::ios::app);
    if (output.is_open()) {
        output << missed_logs_cnt << std::endl;
    }
    else {
        std::cerr << "Failed to write to output file." << std::endl;
    }
    std::cout << "found: " << missed_logs_cnt;
    return 0;
}
