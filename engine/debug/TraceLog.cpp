#include "engine/debug/TraceLog.h"

#include <utility>

namespace resonance {

TraceLog::TraceLog(std::size_t max_entries)
    : max_entries_(max_entries) {}

void TraceLog::push(std::string entry) {
    if (max_entries_ == 0U) {
        return;
    }

    if (entries_.size() == max_entries_) {
        entries_.pop_front();
    }
    entries_.push_back(std::move(entry));
}

std::vector<std::string> TraceLog::entries() const {
    return {entries_.begin(), entries_.end()};
}

}  // namespace resonance
