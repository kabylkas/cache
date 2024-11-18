#include "Cache.h"

static const size_t K = 1024;
static const size_t kMemorySize = 32792;

int main()
{
    // Create and init memory.
    uint8_t* memory_model = new uint8_t[kMemorySize];
    for (size_t i = 0; i < kMemorySize; i++)
    {
        memory_model[i] = (i & 0xFF);
    }

    // Create the cache.
    cs231::Cache cache(4*K, 4, memory_model);

    // Cache access.
    int32_t data = 0;
    bool is_hit = cache.Read(0, cs231::MemAccessSize::kByte, data);
    is_hit = cache.Read(0x4000, cs231::MemAccessSize::kByte, data);
    is_hit = cache.Read(0x4000, cs231::MemAccessSize::kByte, data);

    // Dealloc memory.
    delete [] memory_model;
    return 0;
}
