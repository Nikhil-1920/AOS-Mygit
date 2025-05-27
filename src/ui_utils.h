#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <string>
#include <iostream>

// ANSI Color Codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define UNDERLINE "\033[4m"

// Regular Colors
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Bright Colors
#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

// Background Colors
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"

// Unicode symbols
#define CHECKMARK "✓"
#define CROSS "✗"
#define ARROW "→"
#define BRANCH "⎇"
#define DOT "●"
#define STAR "★"

namespace UI {
    void printHeader();
    void printSuccess(const std::string& message);
    void printError(const std::string& message);
    void printWarning(const std::string& message);
    void printInfo(const std::string& message);
    void printProgress(const std::string& message);
    void printSeparator();
    void printCommitGraph(const std::string& hash, const std::string& message, bool hasParent);
    void printFileStatus(char status, const std::string& filename);
    void printTreeEntry(const std::string& mode, const std::string& type, const std::string& hash, const std::string& name);
}

#endif