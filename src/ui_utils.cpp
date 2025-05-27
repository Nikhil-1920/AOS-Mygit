#include "ui_utils.h"
#include <iomanip>
#include <sstream>

namespace UI {
    void printHeader() {
        std::cout << std::endl;
        std::cout << BRIGHT_CYAN << "  __  __        ____ _ _   " << RESET << std::endl;
        std::cout << BRIGHT_CYAN << " |  \\/  |_   _ / ___(_) |_ " << RESET << std::endl;
        std::cout << BRIGHT_CYAN << " | |\\/| | | | | |  _| | __|" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << " | |  | | |_| | |_| | | |_ " << RESET << std::endl;
        std::cout << BRIGHT_CYAN << " |_|  |_|\\__, |\\____|_|\\__|" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "         |___/              " << RESET << std::endl;
        std::cout << DIM << "         Mini Git - Version Control System" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    void printSuccess(const std::string& message) {
        std::cout << GREEN << CHECKMARK << " " << message << RESET << std::endl;
    }
    
    void printError(const std::string& message) {
        std::cerr << RED << CROSS << " Error: " << message << RESET << std::endl;
    }
    
    void printWarning(const std::string& message) {
        std::cout << YELLOW << "⚠ Warning: " << message << RESET << std::endl;
    }
    
    void printInfo(const std::string& message) {
        std::cout << BLUE << "ℹ " << message << RESET << std::endl;
    }
    
    void printProgress(const std::string& message) {
        std::cout << MAGENTA << "◆ " << message << RESET << std::endl;
    }
    
    void printSeparator() {
        std::cout << DIM << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
    }
    
    void printCommitGraph(const std::string& hash, const std::string& message, bool hasParent) {
        if (hasParent) {
            std::cout << YELLOW << "│" << RESET << std::endl;
        }
        std::cout << YELLOW << DOT << " " << BRIGHT_YELLOW << "commit " << hash.substr(0, 7) << RESET;
        std::cout << " " << DIM << "(" << hash << ")" << RESET << std::endl;
        std::cout << YELLOW << "│" << RESET << " " << message << std::endl;
    }
    
    void printFileStatus(char status, const std::string& filename) {
        switch(status) {
            case 'A':
                std::cout << GREEN << "  A " << RESET << filename << std::endl;
                break;
            case 'M':
                std::cout << YELLOW << "  M " << RESET << filename << std::endl;
                break;
            case 'D':
                std::cout << RED << "  D " << RESET << filename << std::endl;
                break;
            case '?':
                std::cout << BRIGHT_BLACK << "  ? " << RESET << filename << std::endl;
                break;
            default:
                std::cout << "    " << filename << std::endl;
        }
    }
    
    void printTreeEntry(const std::string& mode, const std::string& type, const std::string& hash, const std::string& name) {
        if (type == "tree") {
            std::cout << BLUE << "📁 " << RESET;
        } else {
            std::cout << "📄 ";
        }
        std::cout << std::left << std::setw(20) << name;
        std::cout << DIM << " [" << mode << "] " << hash.substr(0, 8) << "..." << RESET << std::endl;
    }
}