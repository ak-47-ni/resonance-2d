#pragma once

#include <filesystem>
#include <string>

namespace resonance {

std::string read_text_file(const std::filesystem::path& path);

}  // namespace resonance
