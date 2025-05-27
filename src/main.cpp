#include "mygit.h"
#include "ui_utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

void printUsage() {
    UI::printHeader();
    
    std::cout << BOLD << "USAGE:" << RESET << std::endl;
    std::cout << "  mygit <command> [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << BOLD << "COMMANDS:" << RESET << std::endl;
    
    std::cout << BRIGHT_GREEN << "  Repository Management:" << RESET << std::endl;
    std::cout << "    " << CYAN << "init" << RESET << "                     Initialize a new repository" << std::endl;
    std::cout << "    " << CYAN << "status" << RESET << "                   Show working tree status" << std::endl;
    
    std::cout << std::endl;
    std::cout << BRIGHT_GREEN << "  File Operations:" << RESET << std::endl;
    std::cout << "    " << CYAN << "add" << RESET << " <files...>           Add files to staging area" << std::endl;
    std::cout << "    " << CYAN << "commit" << RESET << " [-m <message>]    Create a commit" << std::endl;
    std::cout << "    " << CYAN << "checkout" << RESET << " <hash>          Checkout a commit" << std::endl;
    
    std::cout << std::endl;
    std::cout << BRIGHT_GREEN << "  History & Information:" << RESET << std::endl;
    std::cout << "    " << CYAN << "log" << RESET << "                      Show commit history" << std::endl;
    std::cout << "    " << CYAN << "show" << RESET << " <hash>              Show commit details" << std::endl;
    std::cout << "    " << CYAN << "diff" << RESET << "                     Show changes" << std::endl;
    
    std::cout << std::endl;
    std::cout << BRIGHT_GREEN << "  Low-level Commands:" << RESET << std::endl;
    std::cout << "    " << CYAN << "hash-object" << RESET << " [-w] <file>  Compute object hash" << std::endl;
    std::cout << "    " << CYAN << "cat-file" << RESET << " <flag> <hash>   Display object content" << std::endl;
    std::cout << "    " << CYAN << "write-tree" << RESET << "               Write tree object" << std::endl;
    std::cout << "    " << CYAN << "ls-tree" << RESET << " [--name-only] <hash>  List tree contents" << std::endl;
    
    std::cout << std::endl;
    std::cout << BOLD << "OPTIONS:" << RESET << std::endl;
    std::cout << "    " << DIM << "-h, --help               Show this help message" << RESET << std::endl;
    std::cout << "    " << DIM << "-v, --version            Show version information" << RESET << std::endl;
    
    std::cout << std::endl;
    std::cout << BOLD << "EXAMPLES:" << RESET << std::endl;
    std::cout << DIM << "    mygit init" << RESET << std::endl;
    std::cout << DIM << "    mygit add file.txt" << RESET << std::endl;
    std::cout << DIM << "    mygit commit -m \"Initial commit\"" << RESET << std::endl;
    std::cout << DIM << "    mygit log" << RESET << std::endl;
    
    std::cout << std::endl;
    UI::printSeparator();
    std::cout << DIM << "For more information, visit: https://github.com/yourusername/mygit" << RESET << std::endl;
}

void printVersion() {
    UI::printHeader();
    std::cout << "MyGit version 1.0.0" << std::endl;
    std::cout << DIM << "Built on: " << __DATE__ << " " << __TIME__ << RESET << std::endl;
    std::cout << DIM << "Compatible with Git object format" << RESET << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    MyGit git;
    std::string command = argv[1];
    
    // Handle help and version flags
    if (command == "-h" || command == "--help" || command == "help") {
        printUsage();
        return 0;
    }
    
    if (command == "-v" || command == "--version" || command == "version") {
        printVersion();
        return 0;
    }
    
    if (command == "init") {
        bool result = git.init();
        if (result) {
            UI::printSuccess("Initialized empty MyGit repository in .mygit/");
            std::cout << DIM << "Ready to track your changes!" << RESET << std::endl;
        }
        return result ? 0 : 1;
    }
    else if (command == "status") {
        return git.status() ? 0 : 1;
    }
    else if (command == "hash-object") {
        if (argc < 3) {
            UI::printError("Usage: mygit hash-object [-w] <file>");
            return 1;
        }
        
        bool write = false;
        std::string filename;
        
        if (argc == 4 && std::string(argv[2]) == "-w") {
            write = true;
            filename = argv[3];
        } else {
            filename = argv[2];
        }
        
        std::string hash = git.hashObject(filename, write);
        if (hash.empty()) {
            return 1;
        }
        std::cout << BRIGHT_YELLOW << hash << RESET << std::endl;
        return 0;
    }
    else if (command == "cat-file") {
        if (argc != 4) {
            UI::printError("Usage: mygit cat-file <flag> <hash>");
            return 1;
        }
        
        return git.catFile(argv[2], argv[3]) ? 0 : 1;
    }
    else if (command == "write-tree") {
        UI::printProgress("Writing tree object...");
        std::string tree_hash = git.writeTree();
        if (tree_hash.empty()) {
            return 1;
        }
        std::cout << BRIGHT_YELLOW << tree_hash << RESET << std::endl;
        return 0;
    }
    else if (command == "ls-tree") {
        if (argc < 3) {
            UI::printError("Usage: mygit ls-tree [--name-only] <hash>");
            return 1;
        }
        
        bool name_only = false;
        std::string tree_hash;
        
        if (argc == 4 && std::string(argv[2]) == "--name-only") {
            name_only = true;
            tree_hash = argv[3];
        } else {
            tree_hash = argv[2];
        }
        
        return git.lsTree(tree_hash, name_only) ? 0 : 1;
    }
    else if (command == "add") {
        if (argc < 3) {
            UI::printError("Usage: mygit add <files...>");
            return 1;
        }
        
        std::vector<std::string> files;
        for (int i = 2; i < argc; i++) {
            files.push_back(argv[i]);
        }
        
        return git.add(files) ? 0 : 1;
    }
    else if (command == "commit") {
        std::string message = "";
        
        if (argc >= 4 && std::string(argv[2]) == "-m") {
            message = argv[3];
        } else if (argc == 2) {
            UI::printError("Please provide a commit message with -m");
            return 1;
        }
        
        UI::printProgress("Creating commit...");
        std::string commit_hash = git.commit(message);
        if (!commit_hash.empty()) {
            UI::printSuccess("Created commit " + commit_hash.substr(0, 7));
            std::cout << BRIGHT_YELLOW << commit_hash << RESET << std::endl;
        }
        return commit_hash.empty() ? 1 : 0;
    }
    else if (command == "log") {
        return git.log() ? 0 : 1;
    }
    else if (command == "show") {
        if (argc != 3) {
            UI::printError("Usage: mygit show <commit_hash>");
            return 1;
        }
        return git.show(argv[2]) ? 0 : 1;
    }
    else if (command == "diff") {
        return git.diff() ? 0 : 1;
    }
    else if (command == "checkout") {
        if (argc != 3) {
            UI::printError("Usage: mygit checkout <commit_hash>");
            return 1;
        }
        
        UI::printProgress("Checking out commit " + std::string(argv[2]).substr(0, 7) + "...");
        return git.checkout(argv[2]) ? 0 : 1;
    }
    else {
        UI::printError("Unknown command: " + command);
        std::cout << DIM << "Run 'mygit --help' for usage information" << RESET << std::endl;
        return 1;
    }
}