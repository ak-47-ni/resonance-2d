#include "engine/debug/TraceLog.h"

int main() {
    resonance::TraceLog log(3);
    log.push("a");
    log.push("b");
    log.push("c");
    log.push("d");

    const auto entries = log.entries();
    return (entries.size() == 3 && entries.front() == "b" && entries.back() == "d") ? 0 : 1;
}
