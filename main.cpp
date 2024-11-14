#include "Cache.h"

static const uint32_t K = 1024;

int main()
{
    uint32_t data = 0;

    cs231::Cache cache(4*K, 4);
    bool is_hit = cache.Read(0, cs231::MemAccessSize::kByte, data);
    is_hit = cache.Read(0x4000, cs231::MemAccessSize::kByte, data);

    return 0;
}
