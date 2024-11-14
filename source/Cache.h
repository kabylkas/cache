// Copyright (c) Maveric@NU and texer.ai
// All rights reserved.
#include <cstdint>
#include <memory>
#include <vector>

namespace cs231
{
    // As we said in the lecture, cache line is a row of bytes.
    // Let's model it as vector of bytes.
    using CacheLine = std::vector<uint8_t>;

    enum class MemAccessSize
    {
        kInvalid,
        kByte,
        kHalfWord,
        kWord
    };

    class Cache
    {
    public:
        bool Read(uint32_t address, MemAccessSize access_size, int32_t& data);

        // Control constructors.
        Cache() = delete;
        Cache(size_t total_size, size_t cache_line_size);

    private:
        const size_t kAddressWidth = 32;

        // Cache params.
        size_t total_size_ = 0;
        size_t cache_line_size_ = 0;
        size_t num_cache_lines_ = 0;

        // How to split the address.
        size_t num_offset_bits_ = 0;
        size_t num_index_bits_ = 0;
        size_t num_tag_bits_ = 0;

        // Internal cache memory elements.
        std::vector<bool> valids_;
        std::vector<uint32_t> tags_;
        std::vector<CacheLine> data_;

        // Commnuication to memory.
        std::shared_ptr<uint8_t> memory_pointer_;
    };
}
