CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lz
TARGET = mygit
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/mygit.cpp $(SRCDIR)/sha1.cpp $(SRCDIR)/utils.cpp $(SRCDIR)/ui_utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "\033[32m✓ Build successful!\033[0m"
	@echo "\033[33mRun './mygit --help' to get started\033[0m"

%.o: %.cpp
	@echo "\033[34mCompiling $<...\033[0m"
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "\033[31mCleaning build files...\033[0m"
	rm -f $(OBJECTS) $(TARGET)
	@echo "\033[32m✓ Clean complete\033[0m"

install:
	@echo "\033[34mInstalling mygit...\033[0m"
	chmod +x $(TARGET)
	@echo "\033[32m✓ Installation complete\033[0m"