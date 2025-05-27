#include "mygit.h"
#include "sha1.h"
#include "utils.h"
#include "ui_utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <zlib.h>
#include <cstring>
#include <iomanip>

namespace fs = std::filesystem;

MyGit::MyGit() {
    repo_path = ".mygit";
    objects_path = repo_path + "/objects";
    refs_path = repo_path + "/refs";
    index_path = repo_path + "/index";
    head_path = repo_path + "/HEAD";
}

bool MyGit::init() {
    try {
        if (Utils::directoryExists(repo_path)) {
            UI::printWarning("Repository already initialized");
            return true;
        }
        
        UI::printProgress("Initializing repository structure...");
        
        if (!Utils::createDirectory(repo_path)) {
            UI::printError("Failed to create .mygit directory");
            return false;
        }
        
        if (!Utils::createDirectory(objects_path)) {
            UI::printError("Failed to create objects directory");
            return false;
        }
        
        if (!Utils::createDirectory(refs_path)) {
            UI::printError("Failed to create refs directory");
            return false;
        }
        
        Utils::writeFile(head_path, "");
        Utils::writeFile(index_path, "");
        
        // Create a .gitignore file
        std::string gitignore = "# MyGit ignore file\n*.o\n*.exe\nmygit\n.DS_Store\n";
        Utils::writeFile(".mygitignore", gitignore);
        
        return true;
    } catch (const std::exception& e) {
        UI::printError(std::string("Error initializing repository: ") + e.what());
        return false;
    }
}

std::string MyGit::storeObject(const std::string& content, const std::string& type) {
    // Create the full object with header
    std::string header = type + " " + std::to_string(content.length());
    std::string full_content = header + '\0' + content;
    
    // Calculate hash of the full content (header + null + content)
    std::string hash = sha1(full_content);
    
    // Compress the full content
    std::string compressed = Utils::compress(full_content);
    
    // If compression fails, store uncompressed
    std::string to_store = compressed.empty() ? full_content : compressed;
    
    std::string object_path = getObjectPath(hash);
    
    // Create directory structure
    std::string dir_path = object_path.substr(0, object_path.find_last_of('/'));
    Utils::createDirectory(dir_path);
    
    Utils::writeFile(object_path, to_store);
    
    return hash;
}

std::string MyGit::readObject(const std::string& hash) {
    std::string object_path = getObjectPath(hash);
    
    if (!Utils::fileExists(object_path)) {
        return "";
    }
    
    std::string stored_content = Utils::readFile(object_path);
    if (stored_content.empty()) {
        return "";
    }
    
    // Try to decompress first
    std::string decompressed = Utils::decompress(stored_content);
    
    // If decompression fails, assume it's uncompressed
    if (decompressed.empty()) {
        return stored_content;
    }
    
    return decompressed;
}

std::string MyGit::getObjectPath(const std::string& hash) {
    return objects_path + "/" + hash.substr(0, 2) + "/" + hash.substr(2);
}

std::string MyGit::hashObject(const std::string& filepath, bool write) {
    if (!Utils::fileExists(filepath)) {
        UI::printError("File not found: " + filepath);
        return "";
    }
    
    std::string content = Utils::readFile(filepath);
    
    // Create the full object format for hashing
    std::string header = "blob " + std::to_string(content.length());
    std::string full_content = header + '\0' + content;
    
    // Calculate hash
    std::string hash = sha1(full_content);
    
    if (write) {
        storeObject(content, "blob");
    }
    
    return hash;
}

bool MyGit::catFile(const std::string& flag, const std::string& hash) {
    std::string content = readObject(hash);
    if (content.empty()) {
        UI::printError("Object not found: " + hash);
        return false;
    }
    
    // Parse header
    size_t null_pos = content.find('\0');
    if (null_pos == std::string::npos) {
        UI::printError("Invalid object format");
        return false;
    }
    
    std::string header = content.substr(0, null_pos);
    std::string data = content.substr(null_pos + 1);
    
    size_t space_pos = header.find(' ');
    if (space_pos == std::string::npos) {
        UI::printError("Invalid object header");
        return false;
    }
    
    std::string type = header.substr(0, space_pos);
    std::string size_str = header.substr(space_pos + 1);
    
    if (flag == "-p") {
        std::cout << data;
    } else if (flag == "-s") {
        std::cout << size_str << std::endl;
    } else if (flag == "-t") {
        std::cout << type << std::endl;
    } else {
        UI::printError("Unknown flag: " + flag);
        return false;
    }
    
    return true;
}

std::string MyGit::writeTree(const std::string& path) {
    std::string tree_content = "";
    
    try {
        std::vector<std::pair<std::string, std::string>> entries;
        
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().filename() == ".mygit") continue;
            
            std::string name = entry.path().filename().string();
            std::string mode, type, hash;
            
            if (entry.is_directory()) {
                mode = "040000";
                type = "tree";
                hash = writeTree(entry.path().string());
            } else {
                mode = "100644";
                type = "blob";
                hash = hashObject(entry.path().string(), true);
            }
            
            if (!hash.empty()) {
                entries.push_back({mode + " " + name, hash});
            }
        }
        
        // Sort entries by name for consistent tree hashes
        std::sort(entries.begin(), entries.end());
        
        for (const auto& entry : entries) {
            tree_content += entry.first + "\0" + entry.second;
        }
        
        return storeObject(tree_content, "tree");
    } catch (const std::exception& e) {
        UI::printError("Error writing tree: " + std::string(e.what()));
        return "";
    }
}

std::vector<TreeEntry> MyGit::readTree(const std::string& tree_hash) {
    std::vector<TreeEntry> entries;
    std::string content = readObject(tree_hash);
    
    if (content.empty()) {
        return entries;
    }
    
    // Skip header
    size_t data_start = content.find('\0');
    if (data_start == std::string::npos) {
        return entries;
    }
    
    std::string tree_data = content.substr(data_start + 1);
    
    size_t pos = 0;
    while (pos < tree_data.length()) {
        // Find space (between mode and name)
        size_t space_pos = tree_data.find(' ', pos);
        if (space_pos == std::string::npos) break;
        
        // Find null (between name and hash)
        size_t null_pos = tree_data.find('\0', space_pos);
        if (null_pos == std::string::npos) break;
        
        // Make sure we have enough data for the hash (40 characters)
        if (null_pos + 40 > tree_data.length()) break;
        
        TreeEntry entry;
        entry.mode = tree_data.substr(pos, space_pos - pos);
        entry.name = tree_data.substr(space_pos + 1, null_pos - space_pos - 1);
        entry.hash = tree_data.substr(null_pos + 1, 40);
        entry.type = (entry.mode == "040000") ? "tree" : "blob";
        
        entries.push_back(entry);
        
        // Move to next entry
        pos = null_pos + 1 + 40;
    }
    
    return entries;
}

bool MyGit::lsTree(const std::string& tree_hash, bool name_only) {
    std::vector<TreeEntry> entries = readTree(tree_hash);
    
    if (!name_only) {
        std::cout << std::endl;
    }
    
    for (const auto& entry : entries) {
        if (name_only) {
            std::cout << entry.name << std::endl;
        } else {
            UI::printTreeEntry(entry.mode, entry.type, entry.hash, entry.name);
        }
    }
    
    if (!name_only) {
        std::cout << std::endl;
        std::cout << DIM << entries.size() << " entries" << RESET << std::endl;
    }
    
    return true;
}

std::map<std::string, std::string> MyGit::readIndex() {
    std::map<std::string, std::string> index;
    
    if (!Utils::fileExists(index_path)) {
        return index;
    }
    
    std::string content = Utils::readFile(index_path);
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        
        size_t space_pos = line.find(' ');
        if (space_pos != std::string::npos) {
            std::string hash = line.substr(0, space_pos);
            std::string filename = line.substr(space_pos + 1);
            index[filename] = hash;
        }
    }
    
    return index;
}

void MyGit::writeIndex(const std::map<std::string, std::string>& index) {
    std::ostringstream oss;
    
    for (const auto& pair : index) {
        oss << pair.second << " " << pair.first << std::endl;
    }
    
    Utils::writeFile(index_path, oss.str());
}

bool MyGit::add(const std::vector<std::string>& files) {
    std::map<std::string, std::string> index = readIndex();
    int added_count = 0;
    
    for (const std::string& file : files) {
        if (file == ".") {
            UI::printProgress("Adding all files...");
            try {
                for (const auto& entry : fs::recursive_directory_iterator(".")) {
                    if (entry.is_regular_file() && 
                        entry.path().string().find(".mygit") == std::string::npos) {
                        std::string filepath = entry.path().string();
                        std::string hash = hashObject(filepath, true);
                        if (!hash.empty()) {
                            index[filepath] = hash;
                            added_count++;
                        }
                    }
                }
            } catch (const std::exception& e) {
                UI::printError(std::string("Error adding files: ") + e.what());
                return false;
            }
        } else {
            std::string hash = hashObject(file, true);
            if (!hash.empty()) {
                index[file] = hash;
                UI::printSuccess("Added: " + file);
                added_count++;
            } else {
                UI::printError("Failed to add file: " + file);
                return false;
            }
        }
    }
    
    writeIndex(index);
    
    if (added_count > 0) {
        UI::printInfo(std::to_string(added_count) + " file(s) added to staging area");
    }
    
    return true;
}

std::string MyGit::commit(const std::string& message) {
    // Create tree from staged files
    std::string tree_hash = writeTree();
    if (tree_hash.empty()) {
        UI::printError("Failed to create tree");
        return "";
    }
    
    // Get parent commit
    std::string parent_hash = getCurrentCommit();
    
    // Create commit object
    std::string commit_message = message.empty() ? "Default commit message" : message;
    std::string timestamp = Utils::getCurrentTimestamp();
    
    std::ostringstream commit_content;
    commit_content << "tree " << tree_hash << std::endl;
    if (!parent_hash.empty()) {
        commit_content << "parent " << parent_hash << std::endl;
    }
    commit_content << "author MyGit <mygit@example.com> " << timestamp << std::endl;
    commit_content << "committer MyGit <mygit@example.com> " << timestamp << std::endl;
    commit_content << std::endl;
    commit_content << commit_message << std::endl;
    
    std::string commit_hash = storeObject(commit_content.str(), "commit");
    
    // Update HEAD
    updateHead(commit_hash);
    
    return commit_hash;
}

std::string MyGit::getCurrentCommit() {
    if (!Utils::fileExists(head_path)) {
        return "";
    }
    
    std::string head_content = Utils::readFile(head_path);
    // Remove trailing whitespace
    head_content.erase(head_content.find_last_not_of(" \n\r\t") + 1);
    return head_content;
}

void MyGit::updateHead(const std::string& commit_hash) {
    Utils::writeFile(head_path, commit_hash);
}

bool MyGit::log() {
    std::string current_hash = getCurrentCommit();
    
    if (current_hash.empty()) {
        UI::printInfo("No commits found");
        return true;
    }
    
    UI::printSeparator();
    
    bool first = true;
    while (!current_hash.empty()) {
        std::string commit_content = readObject(current_hash);
        if (commit_content.empty()) {
            break;
        }
        
        // Parse commit
        size_t data_start = commit_content.find('\0');
        if (data_start != std::string::npos) {
            commit_content = commit_content.substr(data_start + 1);
        }
        
        std::istringstream iss(commit_content);
        std::string line;
        std::string tree_hash, parent_hash, author, committer, message;
        
        while (std::getline(iss, line)) {
            if (line.empty()) {
                std::getline(iss, message);
                break;
            } else if (line.substr(0, 5) == "tree ") {
                tree_hash = line.substr(5);
            } else if (line.substr(0, 7) == "parent ") {
                parent_hash = line.substr(7);
            } else if (line.substr(0, 7) == "author ") {
                author = line.substr(7);
            } else if (line.substr(0, 10) == "committer ") {
                committer = line.substr(10);
            }
        }
        
        // Display enhanced commit info
        if (!first) {
            std::cout << YELLOW << "│" << RESET << std::endl;
        }
        
        std::cout << YELLOW << DOT << " " << BRIGHT_YELLOW << "commit " << current_hash << RESET;
        if (first) {
            std::cout << " " << BRIGHT_CYAN << "(HEAD -> master)" << RESET;
        }
        std::cout << std::endl;
        
        if (!parent_hash.empty()) {
            std::cout << YELLOW << "│" << RESET << " " << DIM << "Parent: " << parent_hash.substr(0, 7) << RESET << std::endl;
        }
        
        std::cout << YELLOW << "│" << RESET << " " << "Author: " << BRIGHT_WHITE << author << RESET << std::endl;
        std::cout << YELLOW << "│" << RESET << " " << "Date:   " << DIM << committer << RESET << std::endl;
        std::cout << YELLOW << "│" << RESET << std::endl;
        std::cout << YELLOW << "│" << RESET << "     " << message << std::endl;
        
        current_hash = parent_hash;
        first = false;
    }
    
    std::cout << std::endl;
    UI::printSeparator();
    
    return true;
}

void MyGit::restoreTree(const std::string& tree_hash, const std::string& path) {
    std::vector<TreeEntry> entries = readTree(tree_hash);
    
    for (const auto& entry : entries) {
        // Only skip build artifacts and the executable itself
        if (entry.name == "mygit" ||
            entry.name == "makefile" ||
            entry.name == "Makefile" ||
            entry.name == ".mygit" ||
            entry.name.find(".o") != std::string::npos) {  // Object files
            continue;
        }
        
        std::string full_path;
        if (path == ".") {
            full_path = entry.name;
        } else {
            full_path = path + "/" + entry.name;
        }
        
        if (entry.type == "tree") {
            Utils::createDirectory(full_path);
            restoreTree(entry.hash, full_path);
        } else {
            // This is a blob
            std::string blob_content = readObject(entry.hash);
            if (!blob_content.empty()) {
                // Skip header
                size_t data_start = blob_content.find('\0');
                if (data_start != std::string::npos) {
                    std::string file_content = blob_content.substr(data_start + 1);
                    Utils::writeFile(full_path, file_content);
                }
            }
        }
    }
}

bool MyGit::checkout(const std::string& commit_hash) {
    std::string commit_content = readObject(commit_hash);
    if (commit_content.empty()) {
        UI::printError("Commit not found: " + commit_hash);
        return false;
    }
    
    // Parse commit to get tree hash
    size_t data_start = commit_content.find('\0');
    if (data_start != std::string::npos) {
        commit_content = commit_content.substr(data_start + 1);
    }
    
    std::istringstream iss(commit_content);
    std::string line;
    std::string tree_hash;
    
    while (std::getline(iss, line)) {
        if (line.substr(0, 5) == "tree ") {
            tree_hash = line.substr(5);
            // Remove any trailing whitespace from tree hash
            tree_hash.erase(tree_hash.find_last_not_of(" \n\r\t") + 1);
            break;
        }
    }
    
    if (tree_hash.empty()) {
        UI::printError("Invalid commit format: no tree found");
        return false;
    }
    
    // Clear current directory (except .mygit AND important files)
    try {
        for (const auto& entry : fs::directory_iterator(".")) {
            std::string filename = entry.path().filename().string();
            
            // IMPORTANT: Don't delete these files!
            if (filename == ".mygit" ||
                filename == "mygit" ||           // The executable
                filename == "makefile" ||        // Makefile
                filename == "Makefile" ||        // Alternative capitalization
                filename == "src" ||             // Source directory
                filename == "test.sh" ||         // Test script
                filename.find(".o") != std::string::npos) { // Object files
                continue;
            }
            
            // DELETE everything else including .txt files
            // so they can be properly restored
            fs::remove_all(entry.path());
        }
    } catch (const std::exception& e) {
        UI::printError("Error clearing directory: " + std::string(e.what()));
        return false;
    }
    
    // Restore tree
    restoreTree(tree_hash, ".");
    
    // Update HEAD
    updateHead(commit_hash);
    
    UI::printSuccess("HEAD is now at " + commit_hash.substr(0, 7));
    
    // Show commit message
    iss.clear();
    iss.str(commit_content);
    std::string message;
    
    while (std::getline(iss, line)) {
        if (line.empty()) {
            std::getline(iss, message);
            std::cout << DIM << "Commit message: " << message << RESET << std::endl;
            break;
        }
    }
    
    return true;
}

// Status command implementation
bool MyGit::status() {
    if (!Utils::directoryExists(repo_path)) {
        UI::printError("Not a MyGit repository (or any of the parent directories)");
        return false;
    }
    
    std::cout << BOLD << "On branch " << GREEN << "master" << RESET << std::endl;
    
    std::string current = getCurrentCommit();
    if (current.empty()) {
        std::cout << std::endl;
        std::cout << "No commits yet" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Get file states
    auto staged = getStagedFiles();
    auto modified = getModifiedFiles();
    auto untracked = getUntrackedFiles();
    
    if (!staged.empty()) {
        std::cout << GREEN << "Changes to be committed:" << RESET << std::endl;
        std::cout << DIM << "  (use \"mygit reset HEAD <file>...\" to unstage)" << RESET << std::endl;
        std::cout << std::endl;
        for (const auto& file : staged) {
            UI::printFileStatus('A', file);
        }
        std::cout << std::endl;
    }
   
    if (!modified.empty()) {
        std::cout << RED << "Changes not staged for commit:" << RESET << std::endl;
        std::cout << DIM << "  (use \"mygit add <file>...\" to update what will be committed)" << RESET << std::endl;
        std::cout << std::endl;
        for (const auto& file : modified) {
            UI::printFileStatus('M', file);
        }
        std::cout << std::endl;
    }
   
    if (!untracked.empty()) {
        std::cout << "Untracked files:" << std::endl;
        std::cout << DIM << "  (use \"mygit add <file>...\" to include in what will be committed)" << RESET << std::endl;
        std::cout << std::endl;
        for (const auto& file : untracked) {
            UI::printFileStatus('?', file);
        }
        std::cout << std::endl;
    }
    
    if (staged.empty() && modified.empty() && untracked.empty()) {
        std::cout << "nothing to commit, working tree clean" << std::endl;
    }
    
    return true;
}

// Show command (like git show)
bool MyGit::show(const std::string& commit_hash) {
    std::string commit_content = readObject(commit_hash);
    if (commit_content.empty()) {
        UI::printError("Commit not found: " + commit_hash);
        return false;
    }
    
    // Parse and display commit details
    size_t data_start = commit_content.find('\0');
    if (data_start != std::string::npos) {
        commit_content = commit_content.substr(data_start + 1);
    }
    
    std::cout << BRIGHT_YELLOW << "commit " << commit_hash << RESET << std::endl;
    
    std::istringstream iss(commit_content);
    std::string line;
    std::string tree_hash;
    
    while (std::getline(iss, line)) {
        if (line.empty()) {
            std::cout << std::endl;
            std::string message;
            std::getline(iss, message);
            std::cout << "    " << message << std::endl;
            break;
        } else if (line.substr(0, 5) == "tree ") {
            tree_hash = line.substr(5);
            std::cout << DIM << line << RESET << std::endl;
        } else {
            std::cout << line << std::endl;
        }
    }
    
    std::cout << std::endl;
    UI::printSeparator();
    std::cout << BOLD << "Files in this commit:" << RESET << std::endl;
    
    // Show tree contents
    lsTree(tree_hash, false);
    
    return true;
}

// Diff command (basic implementation)
bool MyGit::diff() {
    UI::printInfo("Showing differences between working directory and staging area...");
    UI::printSeparator();
    
    auto modified = getModifiedFiles();
    
    if (modified.empty()) {
        std::cout << "No changes detected" << std::endl;
        return true;
    }
    
    for (const auto& file : modified) {
        std::cout << BOLD << "diff --mygit a/" << file << " b/" << file << RESET << std::endl;
        std::cout << YELLOW << "--- a/" << file << RESET << std::endl;
        std::cout << GREEN << "+++ b/" << file << RESET << std::endl;
        
        // This is a simplified diff - in real implementation you'd show actual line differences
        std::cout << CYAN << "@@ file modified @@" << RESET << std::endl;
        UI::printFileStatus('M', file);
    }
    
    return true;
}

// Helper methods
std::vector<std::string> MyGit::getUntrackedFiles() {
    std::vector<std::string> untracked;
    auto index = readIndex();
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string path = entry.path().string();
                if (path.find(".mygit") == std::string::npos && 
                    path.find(".o") == std::string::npos &&
                    path != "./mygit" &&
                    index.find(path) == index.end()) {
                    untracked.push_back(path);
                }
            }
        }
    } catch (...) {}
    
    return untracked;
}

std::vector<std::string> MyGit::getStagedFiles() {
    std::vector<std::string> staged;
    auto index = readIndex();
    
    for (const auto& pair : index) {
        staged.push_back(pair.first);
    }
    
    return staged;
}

std::vector<std::string> MyGit::getModifiedFiles() {
    std::vector<std::string> modified;
    auto index = readIndex();
   
    for (const auto& pair : index) {
        std::string current_hash = hashObject(pair.first, false);
        if (current_hash != pair.second) {
            modified.push_back(pair.first);
        }
    }
   
    return modified;
}