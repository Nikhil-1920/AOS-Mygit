#!/bin/bash

echo "🚀 MyGit Complete Test Suite"
echo "=============================="

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' 

TESTS_PASSED=0
TESTS_FAILED=0

run_test() {
    local test_name="$1"
    local command="$2"
    local expected_exit_code="${3:-0}"
    
    echo -e "\n${BLUE}🧪 Testing: $test_name${NC}"
    echo "Command: $command"
    
    eval "$command"
    local actual_exit_code=$?
    
    if [ $actual_exit_code -eq $expected_exit_code ]; then
        echo -e "${GREEN}✅ PASSED${NC}"
        ((TESTS_PASSED++))
    else
        echo -e "${RED}❌ FAILED (expected exit code $expected_exit_code, got $actual_exit_code)${NC}"
        ((TESTS_FAILED++))
    fi
}

create_test_files() {
    echo "Creating test files..."
    
    echo "Hello World" > test1.txt
    echo "This is a test file" > test2.txt
    echo "Binary data: $(date)" > test3.txt
    
    mkdir -p testdir
    echo "File in subdirectory" > testdir/subfile.txt
    echo "Another file" > testdir/another.txt
    
    mkdir -p deep/nested/directory
    echo "Deep file" > deep/nested/directory/deepfile.txt
    
    echo "Test files created."
}

cleanup() {
    echo -e "\n${YELLOW}🧹 Cleaning up test files...${NC}"
    rm -rf .mygit test*.txt testdir deep output.txt commit_hash.txt tree_hash.txt binary_test.txt empty_test.txt special_chars.txt large_test.txt newdir perf_test_*.txt
    echo "Cleanup completed."
}

cleanup

echo -e "\n${YELLOW}📋 Step 1: Building MyGit${NC}"
run_test "Clean build" "make clean"
run_test "Compile MyGit" "make"

echo -e "\n${YELLOW}📋 Step 2: Basic Repository Operations${NC}"
run_test "Initialize repository" "./mygit init"
run_test "Check .mygit directory exists" "[ -d .mygit ]"
run_test "Check objects directory exists" "[ -d .mygit/objects ]"
run_test "Check HEAD file exists" "[ -f .mygit/HEAD ]"

echo -e "\n${YELLOW}📋 Step 3: File Hashing Tests${NC}"
create_test_files

run_test "Hash object without writing" "./mygit hash-object test1.txt"
run_test "Hash object with writing" "./mygit hash-object -w test1.txt"

./mygit hash-object -w test1.txt 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g' | grep -E '^[a-f0-9]{40}$' > /tmp/hash1.txt
HASH1=$(cat /tmp/hash1.txt)
echo "Stored hash: $HASH1"

run_test "Hash different file" "./mygit hash-object -w test2.txt"
./mygit hash-object -w test2.txt 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g' | grep -E '^[a-f0-9]{40}$' > /tmp/hash2.txt
HASH2=$(cat /tmp/hash2.txt)
echo "Stored hash: $HASH2"

echo -e "\n${YELLOW}📋 Step 4: Object Inspection Tests${NC}"
run_test "Cat-file print content" "./mygit cat-file -p $HASH1"
run_test "Cat-file show size" "./mygit cat-file -s $HASH1"
run_test "Cat-file show type" "./mygit cat-file -t $HASH1"

echo -e "\n${YELLOW}📋 Step 5: Tree Operations${NC}"
run_test "Write tree" "./mygit write-tree"
./mygit write-tree 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g' | grep -E '^[a-f0-9]{40}$' > /tmp/tree_hash.txt
TREE_HASH=$(cat /tmp/tree_hash.txt)
echo "Tree hash: $TREE_HASH"

run_test "List tree contents" "./mygit ls-tree $TREE_HASH"
run_test "List tree names only" "./mygit ls-tree --name-only $TREE_HASH"

echo -e "\n${YELLOW}📋 Step 6: Staging Tests${NC}"
run_test "Add single file" "./mygit add test1.txt"
run_test "Add multiple files" "./mygit add test2.txt test3.txt"
run_test "Add directory" "./mygit add testdir/subfile.txt"
run_test "Add all files" "./mygit add ."

echo -e "\n${YELLOW}📋 Step 7: Commit Tests${NC}"
run_test "Commit with message" "./mygit commit -m 'Initial commit'"
COMMIT1=$(./mygit log 2>/dev/null | grep -E '^[a-f0-9]{40}$' | head -1)
if [ -z "$COMMIT1" ]; then
    COMMIT1=$(./mygit log 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g' | grep "^commit " | head -1 | awk '{print $2}')
fi
echo "First commit: $COMMIT1"

echo "Modified content" > test1.txt
run_test "Add modified file" "./mygit add test1.txt"
run_test "Second commit" "./mygit commit -m 'Second commit'"
COMMIT2=$(./mygit log 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g' | grep "^commit " | head -1 | awk '{print $2}')
echo "Second commit: $COMMIT2"

echo "More changes" > test3.txt
mkdir -p newdir
echo "New file in new directory" > newdir/newfile.txt
run_test "Add new changes" "./mygit add ."
run_test "Third commit" "./mygit commit -m 'Third commit with new directory'"
COMMIT3=$(./mygit log 2>/dev/null | sed 's/\x1b\[[0-9;]*m//g' | grep "^commit " | head -1 | awk '{print $2}')

echo -e "\n${YELLOW}📋 Step 8: Log Tests${NC}"
run_test "Show commit log" "./mygit log"

echo -e "\n${YELLOW}📋 Step 9: Checkout Tests${NC}"
cp test.sh /tmp/test_backup.sh 2>/dev/null || true

if [ ! -z "$COMMIT1" ]; then
    run_test "Checkout first commit" "./mygit checkout $COMMIT1"
    
    echo -e "\n${BLUE}Verifying checkout restored original content:${NC}"
    if [ -f test1.txt ]; then
        content=$(cat test1.txt)
        if [[ "$content" == "Hello World" ]]; then
            echo -e "${GREEN}✅ Content correctly restored${NC}"
            ((TESTS_PASSED++))
        else
            echo -e "${RED}❌ Content not restored correctly${NC}"
            echo "Expected: 'Hello World'"
            echo "Got: '$content'"
            ((TESTS_FAILED++))
        fi
    else
        echo -e "${RED}❌ File not restored${NC}"
        ((TESTS_FAILED++))
    fi
    
    if [ ! -z "$COMMIT2" ]; then
        run_test "Checkout back to second commit" "./mygit checkout $COMMIT2"
    fi
else
    echo -e "${YELLOW}⚠ Skipping checkout tests - no commits found${NC}"
fi

if [ ! -f test.sh ] && [ -f /tmp/test_backup.sh ]; then
    cp /tmp/test_backup.sh test.sh
    chmod +x test.sh
fi

echo -e "\n${YELLOW}📋 Step 10: Error Handling Tests${NC}"
run_test "Hash non-existent file" "./mygit hash-object nonexistent.txt" 1
run_test "Cat-file with invalid hash" "./mygit cat-file -p invalidhash123" 1
run_test "Checkout invalid commit" "./mygit checkout invalidcommithash" 1
run_test "Invalid command" "./mygit invalidcommand" 1

echo -e "\n${YELLOW}📋 Step 11: Advanced Tests${NC}"

echo -e "\x00\x01\x02\x03\xFF\xFE\xFD" > binary_test.txt
run_test "Hash binary file" "./mygit hash-object -w binary_test.txt"

dd if=/dev/zero of=large_test.txt bs=1024 count=100 2>/dev/null
run_test "Hash large file" "./mygit hash-object -w large_test.txt"

touch empty_test.txt
run_test "Hash empty file" "./mygit hash-object -w empty_test.txt"

echo -e "Line 1\nLine 2\tTab\nLine 3 with spaces" > special_chars.txt
run_test "Hash file with special chars" "./mygit hash-object -w special_chars.txt"

echo -e "\n${YELLOW}📋 Step 12: Repository State Verification${NC}"

echo -e "\n${BLUE}Repository structure:${NC}"
find .mygit -type f | head -20

echo -e "\n${BLUE}Object count:${NC}"
find .mygit/objects -type f | wc -l

echo -e "\n${BLUE}Current HEAD:${NC}"
cat .mygit/HEAD 2>/dev/null || echo "No HEAD file"

echo -e "\n${BLUE}Index content:${NC}"
cat .mygit/index 2>/dev/null | head -10 || echo "No index file"

echo -e "\n${YELLOW}📋 Step 13: Performance Test${NC}"
echo "Creating 50 small files for performance test..."
for i in {1..50}; do
    echo "File $i content" > perf_test_$i.txt
done

start_time=$(date +%s)
run_test "Add 50 files" "./mygit add perf_test_*.txt"
run_test "Commit 50 files" "./mygit commit -m 'Performance test commit'"
end_time=$(date +%s)

echo "Performance test took $((end_time - start_time)) seconds"

rm -f perf_test_*.txt

echo -e "\n${YELLOW}📋 Step 14: Final Repository State${NC}"
run_test "Final log check" "./mygit log"

echo -e "\n${YELLOW}🏁 Test Summary${NC}"
echo "==============="
echo -e "${GREEN}Tests Passed: $TESTS_PASSED${NC}"
echo -e "${RED}Tests Failed: $TESTS_FAILED${NC}"
echo "Total Tests: $((TESTS_PASSED + TESTS_FAILED))"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All tests passed! Your MyGit implementation is working correctly!${NC}"
    exit 0
else
    echo -e "\n${RED}❌ Some tests failed. Please check the implementation.${NC}"
    exit 1
fi