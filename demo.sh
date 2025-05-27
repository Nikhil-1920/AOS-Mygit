#!/bin/bash

# Colors
CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
BOLD='\033[1m'
DIM='\033[2m'
NC='\033[0m'

# Function to simulate typing
type_command() {
    echo -en "${GREEN}$ ${NC}"
    for ((i=0; i<${#1}; i++)); do
        echo -n "${1:$i:1}"
        sleep 0.05
    done
    echo
    sleep 0.5
}

# Function to pause
pause() {
    echo -e "\n${DIM}Press Enter to continue...${NC}"
    read
}

clear

echo -e "${CYAN}${BOLD}"
echo "╔══════════════════════════════════════╗"
echo "║     MyGit Interactive Demo           ║"
echo "║     Version Control System           ║"
echo "╚══════════════════════════════════════╝"
echo -e "${NC}\n"

echo -e "${YELLOW}Welcome to MyGit!${NC}"
echo -e "${DIM}This demo will show you the main features${NC}\n"
pause

# Clean up
rm -rf demo_repo
mkdir demo_repo
cd demo_repo

echo -e "${MAGENTA}📁 Creating a new repository...${NC}\n"
type_command "../mygit init"
../mygit init
pause

echo -e "\n${MAGENTA}📄 Creating some files...${NC}\n"
type_command "echo 'Hello, MyGit!' > README.md"
echo 'Hello, MyGit!' > README.md
type_command "echo 'print(\"Hello, World!\")' > hello.py"
echo 'print("Hello, World!")' > hello.py
pause

echo -e "\n${MAGENTA}📊 Checking repository status...${NC}\n"
type_command "../mygit status"
../mygit status
pause

echo -e "\n${MAGENTA}➕ Adding files to staging area...${NC}\n"
type_command "../mygit add README.md hello.py"
../mygit add README.md hello.py
pause

echo -e "\n${MAGENTA}📊 Status after staging...${NC}\n"
type_command "../mygit status"
../mygit status
pause

echo -e "\n${MAGENTA}💾 Creating first commit...${NC}\n"
type_command "../mygit commit -m 'Initial commit: Add README and hello.py'"
../mygit commit -m "Initial commit: Add README and hello.py"
pause

echo -e "\n${MAGENTA}✏️  Modifying files...${NC}\n"
type_command "echo '# MyGit Demo Repository' > README.md"
echo '# MyGit Demo Repository' > README.md
type_command "echo 'This is a demonstration of MyGit features.' >> README.md"
echo 'This is a demonstration of MyGit features.' >> README.md
pause

echo -e "\n${MAGENTA}📊 Checking what changed...${NC}\n"
type_command "../mygit status"
../mygit status
pause

echo -e "\n${MAGENTA}💾 Committing changes...${NC}\n"
type_command "../mygit add README.md"
../mygit add README.md
type_command "../mygit commit -m 'Update README with proper title'"
../mygit commit -m "Update README with proper title"
pause

echo -e "\n${MAGENTA}📜 Viewing commit history...${NC}\n"
type_command "../mygit log"
../mygit log
pause

echo -e "\n${MAGENTA}🎯 Demo completed!${NC}"
echo -e "${GREEN}✨ You've successfully used MyGit to:${NC}"
echo -e "  • Initialize a repository"
echo -e "  • Add files"
echo -e "  • Create commits"
echo -e "  • View history"
echo -e "\n${DIM}Try './mygit --help' for more commands${NC}\n"

cd ..
rm -rf demo_repo