#include "sha1.h"
#include <fstream>
#include <cstring>
#include <cstdint>

SHA1::SHA1() {
    reset();
}

void SHA1::reset() {
    digest[0] = 0x67452301;
    digest[1] = 0xEFCDAB89;
    digest[2] = 0x98BADCFE;
    digest[3] = 0x10325476;
    digest[4] = 0xC3D2E1F0;
    
    buffer = "";
    transforms = 0;
}

SHA1::uint32 SHA1::rol(uint32 value, size_t amount) {
    return (value << amount) | (value >> (32 - amount));
}

SHA1::uint32 SHA1::blk(const uint32 block[BLOCK_BYTES], const size_t i) {
    return rol(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i], 1);
}

SHA1::uint32 SHA1::f1(uint32 b, uint32 c, uint32 d) {
    return d ^ (b & (c ^ d));
}

SHA1::uint32 SHA1::f2(uint32 b, uint32 c, uint32 d) {
    return b ^ c ^ d;
}

SHA1::uint32 SHA1::f3(uint32 b, uint32 c, uint32 d) {
    return (b & c) | (d & (b | c));
}

void SHA1::buffer_to_block(const std::string &buffer, uint32 block[BLOCK_BYTES]) {
    for (size_t i = 0; i < BLOCK_INTS; i++) {
        block[i] = (buffer[4*i+3] & 0xff)
                 | (buffer[4*i+2] & 0xff)<<8
                 | (buffer[4*i+1] & 0xff)<<16
                 | (buffer[4*i+0] & 0xff)<<24;
    }
}

void SHA1::transform(uint32 block[BLOCK_BYTES]) {
    uint32 w[80];
    for (size_t i = 0; i < 16; i++) {
        w[i] = block[i];
    }
    for (size_t i = 16; i < 80; i++) {
        w[i] = rol((w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);
    }

    uint32 a = digest[0];
    uint32 b = digest[1];
    uint32 c = digest[2];
    uint32 d = digest[3];
    uint32 e = digest[4];

    for (size_t i = 0; i < 80; ++i) {
        uint32 f = 0;
        uint32 k = 0;

        if (i < 20) {
            f = f1(b, c, d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = f2(b, c, d);
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = f3(b, c, d);
            k = 0x8F1BBCDC;
        } else {
            f = f2(b, c, d);
            k = 0xCA62C1D6;
        }

        uint32 temp = rol(a, 5) + f + e + k + w[i];
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = temp;
    }

    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;

    transforms++;
}

void SHA1::update(const std::string &s) {
    update(s.c_str(), s.length());
}

void SHA1::update(const char* data, size_t len) {
    buffer.append(data, len);
    
    while (buffer.size() >= BLOCK_BYTES) {
        uint32 block[BLOCK_BYTES];
        buffer_to_block(buffer, block);
        transform(block);
        buffer.erase(0, BLOCK_BYTES);
    }
}

std::string SHA1::final() {
    uint64 total_bits = (transforms * BLOCK_BYTES + buffer.size()) * 8;

    buffer += (char)0x80;
    size_t orig_size = buffer.size();
    while (buffer.size() < BLOCK_BYTES) {
        buffer += (char)0x00;
    }

    uint32 block[BLOCK_BYTES];
    buffer_to_block(buffer, block);

    if (orig_size > BLOCK_BYTES - 8) {
        transform(block);
        for (size_t i = 0; i < BLOCK_INTS - 2; i++) {
            block[i] = 0;
        }
    }

    block[BLOCK_INTS - 1] = (uint32)total_bits;
    block[BLOCK_INTS - 2] = (uint32)(total_bits >> 32);
    transform(block);

    std::ostringstream result;
    for (size_t i = 0; i < DIGEST_INTS; i++) {
        result << std::hex << std::setfill('0') << std::setw(8);
        result << digest[i];
    }

    reset();
    return result.str();
}

std::string SHA1::from_file(const std::string &filename) {
    std::ifstream stream(filename.c_str(), std::ios::binary);
    if (!stream) {
        return "";
    }

    SHA1 checksum;
    
    char buffer[1024];
    while (stream.read(buffer, sizeof(buffer))) {
        checksum.update(buffer, sizeof(buffer));
    }
    checksum.update(buffer, stream.gcount());
    
    return checksum.final();
}

std::string sha1(const std::string &string) {
    SHA1 checksum;
    checksum.update(string);
    return checksum.final();
}
