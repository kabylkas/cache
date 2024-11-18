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

        std::cout << std::hex << "  Getting data from the cache. Offset = " << offset << ". Data = " << data << std::endl;
        return data;
    }

    Cache::Cache(size_t total_size, size_t cache_line_size, uint8_t* memory_pointer)
    {
        // Save and calculate missing parameters.
        total_size_ = total_size;
        cache_line_size_ = cache_line_size;
        num_cache_lines_ = total_size / cache_line_size;

        // Calculate number of bits for each component.
        num_offset_bits_ = std::log2(cache_line_size_);
        num_index_bits_ = std::log2(num_cache_lines_);
        num_tag_bits_ = kAddressWidth - (num_offset_bits_ + num_index_bits_);

        // Set the memory pointer. Cache will talk to this
        // element on miss.
        memory_pointer_ = memory_pointer;

        // Initialize the cache memory elements.
        // 1. Create an init vector with the size cache_line_size_
        // and initialize all the values to gargage values.
        CacheLine init_cache_line(cache_line_size_, 0xC0);

        // 2. Go over each cache line and init values for each
        // memory element.
        for (size_t i = 0; i < num_cache_lines_; i++)
        {
            // All cachelines are invalid initially.
            valids_.push_back(false);

            // Init values to data and tag.
            tags_.push_back(0xDEADBEEF);
            data_.push_back(init_cache_line);
        }
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
        std::cout << "-----" << std::endl;
        std::cout << "Splitting the address." << std::endl;
        std::cout << "  Tag    = " << std::hex << tag << std::endl;
        std::cout << "  Index  = " << std::hex << index << std::endl;
        std::cout << "  Offset = " << std::hex << offset << std::endl;

        // 2. Check the valid bit.
        if (valids_[index])
        {
            // 3. Check the tags.
            if (tags_[index] == tag)
            {
                // That's the hit! We can safely get the data.
                // Report.
                std::cout << "Hit!" << std:: endl;
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
            // Use the address to access memory location
            // and bring N bytes from the memory. N = cache_line_size_.
            CacheLine new_cache_line;
            for (size_t i = 0; i < cache_line_size_; i++)
            {
                uint32_t byte_address = ((address >> num_index_bits_) << num_index_bits_) + i;
                new_cache_line.push_back(memory_pointer_[byte_address]);
            }

            // Overwrite memory elements of the cache with new data.
            data_[index] = new_cache_line;
            tags_[index] = tag;
            valids_[index] = true;

            // Report.
            std::cout << "Miss. Data brought from memory." << std:: endl;
            std::cout << "  New data = ";
            for (const uint8_t b : new_cache_line)
            {
                std::cout << std::hex << "0x" << static_cast<uint32_t>(b) << " ";
            }
            std::cout << std::endl;
        }

        // Get the data from the cache.
        data = GetDataFromCacheLine(data_[index], offset, access_size);

        return !is_miss;
    }
}
