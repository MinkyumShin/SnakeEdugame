#pragma once
#include <string>
#include <fstream>
#include <vector>

std::vector<std::string> readFileLines(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) 
        return {};

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) 
        lines.push_back(line);

    file.close();
    return lines;
}

bool saveFile(const std::string &filePath, const std::string &content) {
    std::ofstream file(filePath);
    if (!file.is_open()) 
        return false;

    file << content;
    file.close();
    return true;
}

bool appendFile(const std::string &filePath, const std::string &data) {
    std::ofstream file(filePath, std::ios::app);
    if (!file.is_open()) 
        return false;

    file << data << "\n";
    file.close();
    return true;
}