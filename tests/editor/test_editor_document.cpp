#include "engine/core/FileSystem.h"
#include "engine/editor/EditorDocument.h"
#include "engine/world/RegionData.h"
#include "engine/world/StoryAnchorData.h"

#include <cmath>
#include <filesystem>
#include <string>

namespace {

bool approx_equal(float lhs, float rhs) {
    return std::fabs(lhs - rhs) < 0.0001F;
}

std::filesystem::path make_temp_root() {
    const auto root = std::filesystem::temp_directory_path() / "resonance_editor_document_test";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "regions");
    std::filesystem::create_directories(root / "story");
    return root;
}

void copy_fixture_files(const std::filesystem::path& root) {
    std::filesystem::copy_file(
        "assets/data/regions/regions.json",
        root / "regions" / "regions.json",
        std::filesystem::copy_options::overwrite_existing
    );
    std::filesystem::copy_file(
        "assets/data/story/story_anchors.json",
        root / "story" / "story_anchors.json",
        std::filesystem::copy_options::overwrite_existing
    );
}

const resonance::StoryAnchorData* find_anchor(
    const std::vector<resonance::StoryAnchorData>& anchors,
    const std::string& id
) {
    for (const auto& anchor : anchors) {
        if (anchor.id == id) {
            return &anchor;
        }
    }
    return nullptr;
}

const resonance::RegionData* find_region(
    const std::vector<resonance::RegionData>& regions,
    const std::string& id
) {
    for (const auto& region : regions) {
        if (region.id == id) {
            return &region;
        }
    }
    return nullptr;
}

}  // namespace

int main() {
    const auto root = make_temp_root();
    copy_fixture_files(root);

    auto document = resonance::load_editor_document(root);

    const auto anchor_state = resonance::build_editor_inspector_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::story_anchor, "meadow-swing"}
    );
    const auto region_state = resonance::build_editor_inspector_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::region, "meadow"}
    );
    const auto missing_state = resonance::build_editor_inspector_state(
        document,
        resonance::EditorSelection{resonance::EditorSelectionKind::story_anchor, "missing-anchor"}
    );

    const bool anchor_state_ok = anchor_state.story_anchor.has_value() &&
        anchor_state.region == std::nullopt &&
        anchor_state.story_anchor->id == "meadow-swing" &&
        anchor_state.story_anchor->region_id == "meadow";

    const bool region_state_ok = region_state.region.has_value() &&
        region_state.story_anchor == std::nullopt &&
        region_state.region->id == "meadow" &&
        region_state.region->default_music_state == "explore";

    const bool missing_state_ok = !missing_state.story_anchor.has_value() &&
        !missing_state.region.has_value();

    auto edited_anchor = *anchor_state.story_anchor;
    edited_anchor.x += 22.5F;
    edited_anchor.y += 11.0F;
    edited_anchor.activation_radius += 6.0F;
    edited_anchor.prompt_text = "Press E to edit";

    auto edited_region = *region_state.region;
    edited_region.x -= 12.0F;
    edited_region.width += 43.0F;
    edited_region.height += 19.0F;

    const bool anchor_apply_ok = resonance::apply_story_anchor_inspector(document, edited_anchor);
    const bool region_apply_ok = resonance::apply_region_inspector(document, edited_region);

    resonance::write_editor_document(document, root);

    const auto reloaded_document = resonance::load_editor_document(root);
    const auto* reloaded_anchor = find_anchor(reloaded_document.story_anchors, "meadow-swing");
    const auto* reloaded_region = find_region(reloaded_document.regions, "meadow");

    const auto story_json = resonance::read_text_file(root / "story" / "story_anchors.json");
    const auto region_json = resonance::read_text_file(root / "regions" / "regions.json");
    const bool file_shape_ok = story_json.find("\"story_anchors\"") != std::string::npos &&
        region_json.find("\"regions\"") != std::string::npos;

    const bool anchor_round_trip_ok = reloaded_anchor != nullptr &&
        approx_equal(reloaded_anchor->position.x, edited_anchor.x) &&
        approx_equal(reloaded_anchor->position.y, edited_anchor.y) &&
        approx_equal(reloaded_anchor->activation_radius, edited_anchor.activation_radius) &&
        reloaded_anchor->prompt_text == edited_anchor.prompt_text;

    const bool region_round_trip_ok = reloaded_region != nullptr &&
        approx_equal(reloaded_region->bounds.x, edited_region.x) &&
        approx_equal(reloaded_region->bounds.width, edited_region.width) &&
        approx_equal(reloaded_region->bounds.height, edited_region.height);

    std::filesystem::remove_all(root);

    return (anchor_state_ok && region_state_ok && missing_state_ok && anchor_apply_ok &&
            region_apply_ok && file_shape_ok && anchor_round_trip_ok && region_round_trip_ok)
        ? 0
        : 1;
}
