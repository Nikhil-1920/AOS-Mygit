#ifndef MYGIT_H
#define MYGIT_H

#include <string>
#include <vector>
#include <map>

struct TreeEntry {
    std::string mode;
    std::string type;
    std::string hash;
    std::string name;
};

struct CommitInfo {
    std::string hash;
    std::string tree_hash;
    std::string parent_hash;
    std::string message;
    std::string timestamp;
    std::string author;
};

class MyGit {
private:
    std::string repo_path;
    std::string objects_path;
    std::string refs_path;
    std::string index_path;
    std::string head_path;
    
    std::string storeObject(const std::string& content, const std::string& type);
    std::string readObject(const std::string& hash);
    std::string getObjectPath(const std::string& hash);
    std::vector<TreeEntry> readTree(const std::string& tree_hash);
    void restoreTree(const std::string& tree_hash, const std::string& path = ".");
    std::string getCurrentCommit();
    void updateHead(const std::string& commit_hash);
    std::map<std::string, std::string> readIndex();
    void writeIndex(const std::map<std::string, std::string>& index);

public:
    MyGit();
    
    // Commands
    bool init();
    std::string hashObject(const std::string& filepath, bool write = false);
    bool catFile(const std::string& flag, const std::string& hash);
    std::string writeTree(const std::string& path = ".");
    bool lsTree(const std::string& tree_hash, bool name_only = false);
    bool add(const std::vector<std::string>& files);
    std::string commit(const std::string& message = "");
    bool log();
    bool checkout(const std::string& commit_hash);
    
    // New enhanced methods
    bool status();
    bool show(const std::string& commit_hash);
    bool diff();
    
private:
    // Helper methods for enhanced UI
    std::vector<std::string> getUntrackedFiles();
    std::vector<std::string> getStagedFiles();
    std::vector<std::string> getModifiedFiles();
};

#endif