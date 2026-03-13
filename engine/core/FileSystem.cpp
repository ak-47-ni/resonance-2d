#include "engine/core/FileSystem.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace resonance {

std::string read_text_file(const std::filesystem::path& path) {
    std::ifstream stream(path);
    if (!stream.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    std::ostringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

}  // namespace resonance
