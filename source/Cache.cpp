// Copyright (c) Maveric@NU and texer.ai
// All rights reserved.

#include "Cache.h"

// C++ libraries.
#include <cassert>
#include <cmath>
#include <iostream>

namespace cs231
{
    // Gets specific bits from number. Returns bits number[x+bits : x].
    static uint32_t GetBits(uint32_t number, uint32_t x, uint32_t bits)
    {
        // Calculate the mask.
        uint32_t mask = (1 << bits) - 1;

        // Retrieve the value.
        uint32_t value = (number >> x) & mask;
        return value;
    }

    // Given the cache line read the data out of it with all proper checks.
    static int32_t GetDataFromCacheLine(const CacheLine& cache_line, uint32_t offset, MemAccessSize access_size)
    {
        int32_t data = 0;

        switch (access_size)
        {
        case MemAccessSize::kByte:
            // Provide single byte from the cache line.
            data = static_cast<int32_t>(cache_line[offset]);
            break;
        case MemAccessSize::kHalfWord:
            // Provide two bytes from the cache line.
            data = static_cast<int32_t>(cache_line[offset + 1] << 8 |
                cache_line[offset]);
            break;
        case MemAccessSize::kWord:
            // Provide four bytes from the cache line.
            data = static_cast<int32_t>((cache_line[offset + 3] << 24) |
                (cache_line[offset + 2] << 16) |
                (cache_line[offset + 1] << 8) |
                cache_line[offset]);
            break;
        default:
            // Something wrong we should not be here.
            assert(false);
            break;
        }

        return data;
    }

    Cache::Cache(size_t total_size, size_t cache_line_size)
    {
        // Save and calculate missing parameters.
        total_size_ = total_size;
        cache_line_size_ = cache_line_size;
        num_cache_lines_ = total_size / cache_line_size;

        // Calculate number of bits for each component.
        num_offset_bits_ = std::log2(cache_line_size_);
        num_index_bits_ = std::log2(num_cache_lines_);
        num_tag_bits_ = kAddressWidth - (num_offset_bits_ + num_index_bits_);
    }

    bool Cache::Read(uint32_t address, MemAccessSize access_size, int32_t& data)
    {
        bool is_miss = false;

        // Report.
        std::cout << std::hex << "Cache read access. Address = " << address << std::endl;

        // 1. Split the address into offset, index and tag.
        uint32_t offset = GetBits(address, 0, num_offset_bits_);
        uint32_t index = GetBits(address, num_offset_bits_, num_index_bits_);
        uint32_t tag = address >> (num_offset_bits_ + num_index_bits_);

        // Report.
        std::cout << "Splitting the address." << std::endl;
        std::cout << "  Tag    = " << std::hex << tag << std::endl;
        std::cout << "  Index  = " << std::hex << index << std::endl;
        std::cout << "  Offset = " << std::hex << offset << std::endl;
        std::cout << "-----" << std::endl;

        // 2. Check the valid bit.
        if (valids_[index])
        {
            // 3. Check the tags.
            if (tags_[index] == tag)
            {
                // That's the hit! We can safely get the data.
                data = GetDataFromCacheLine(data_[index], offset, access_size);
            }
            else
            {
                // Tags didn't match. The data in the cache is
                // NOT the one we are requesting.
                is_miss = true;
            }
        }
        else
        {
            // Valid bit is set to 0, this is a miss.
            is_miss = true;
        }

        // 4. If this was a miss, talk to memory to bring the data.
        if (is_miss)
        {

        }

        return !is_miss;
    }
}
