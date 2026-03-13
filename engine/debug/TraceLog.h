#pragma once

#include <cstddef>
#include <deque>
#include <string>
#include <vector>

namespace resonance {

class TraceLog {
public:
    explicit TraceLog(std::size_t max_entries);

    void push(std::string entry);
    std::vector<std::string> entries() const;

private:
    std::size_t max_entries_;
    std::deque<std::string> entries_;
};

}  // namespace resonance
