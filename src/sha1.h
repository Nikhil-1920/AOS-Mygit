#ifndef SHA1_H
#define SHA1_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdint>

class SHA1 {
public:
    SHA1();
    void update(const std::string &s);
    void update(const char* data, size_t len);
    std::string final();
    static std::string from_file(const std::string &filename);

private:
    typedef uint32_t uint32;
    typedef uint64_t uint64;
    
    static const unsigned int DIGEST_INTS = 5;
    static const unsigned int BLOCK_INTS = 16;
    static const unsigned int BLOCK_BYTES = BLOCK_INTS * 4;
    
    uint32 digest[DIGEST_INTS];
    std::string buffer;
    uint64 transforms;
    
    void reset();
    void transform(uint32 block[BLOCK_BYTES]);
    static void buffer_to_block(const std::string &buffer, uint32 block[BLOCK_BYTES]);
    static uint32 rol(uint32 value, size_t amount);
    static uint32 blk(const uint32 block[BLOCK_BYTES], const size_t i);
    static uint32 f1(uint32 b, uint32 c, uint32 d);
    static uint32 f2(uint32 b, uint32 c, uint32 d);
    static uint32 f3(uint32 b, uint32 c, uint32 d);
};

std::string sha1(const std::string &string);

#endif
