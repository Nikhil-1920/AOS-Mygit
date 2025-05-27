#include "utils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <cstring>

namespace fs = std::filesystem;

namespace Utils {
    bool fileExists(const std::string& path) {
        return fs::exists(path) && fs::is_regular_file(path);
    }
    
    bool directoryExists(const std::string& path) {
        return fs::exists(path) && fs::is_directory(path);
    }
    
    bool createDirectory(const std::string& path) {
        try {
            return fs::create_directories(path);
        } catch (...) {
            return false;
        }
    }
    
    std::string readFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return "";
        
        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
    
    bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;
        
        file << content;
        return true;
    }
    
    std::vector<std::string> listDirectory(const std::string& path) {
        std::vector<std::string> files;
        try {
            for (const auto& entry : fs::directory_iterator(path)) {
                files.push_back(entry.path().filename().string());
            }
        } catch (...) {}
        return files;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    std::string compress(const std::string& data) {
        if (data.empty()) return "";
        
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK) {
            return "";
        }
        
        zs.next_in = (Bytef*)data.data();
        zs.avail_in = data.size();
        
        int ret;
        char outbuffer[32768];
        std::string compressed;
        
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            
            ret = deflate(&zs, Z_FINISH);
            
            if (compressed.size() < zs.total_out) {
                compressed.append(outbuffer, zs.total_out - compressed.size());
            }
        } while (ret == Z_OK);
        
        deflateEnd(&zs);
        
        if (ret != Z_STREAM_END) {
            return "";
        }
        
        return compressed;
    }
    
    std::string decompress(const std::string& data) {
        if (data.empty()) return "";
        
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        
        if (inflateInit(&zs) != Z_OK) {
            return "";
        }
        
        zs.next_in = (Bytef*)data.data();
        zs.avail_in = data.size();
        
        int ret;
        char outbuffer[32768];
        std::string decompressed;
        
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            
            ret = inflate(&zs, Z_NO_FLUSH);
            
            if (decompressed.size() < zs.total_out) {
                decompressed.append(outbuffer, zs.total_out - decompressed.size());
            }
            
            if (ret == Z_STREAM_END) break;
        } while (ret == Z_OK);
        
        inflateEnd(&zs);
        
        if (ret != Z_STREAM_END) {
            return "";
        }
        
        return decompressed;
    }
    
    std::string joinPath(const std::string& path1, const std::string& path2) {
        if (path1.empty()) return path2;
        if (path2.empty()) return path1;
        
        if (path1.back() == '/' || path2.front() == '/') {
            return path1 + path2;
        } else {
            return path1 + "/" + path2;
        }
    }
}
