#pragma once

#include <string>

namespace resonance {

struct MemoryJournalEntry {
    std::string id;
    std::string region_id;
    std::string story_text;
};

}  // namespace resonance
