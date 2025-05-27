#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <ctime>

namespace Utils {
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::string readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& content);
    std::vector<std::string> listDirectory(const std::string& path);
    std::string getCurrentTimestamp();
    std::string compress(const std::string& data);
    std::string decompress(const std::string& data);
    std::string joinPath(const std::string& path1, const std::string& path2);
}

#endif
