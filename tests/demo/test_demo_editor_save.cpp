#include "engine/editor/EditorDocument.h"
#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

namespace {

bool approx_equal(float lhs, float rhs) {
    return std::fabs(lhs - rhs) < 0.0001F;
}

std::filesystem::path make_temp_root() {
    const auto root = std::filesystem::temp_directory_path() / "resonance_demo_editor_save_test";
    std::filesystem::remove_all(root);
    return root;
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
    const auto temp_root = make_temp_root();
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.toggle_editor_mode();
    scene.select_region_at({24.0F, 24.0F});
    scene.nudge_editor_selection({12.0F, 8.0F});
    scene.adjust_editor_selection_primary(2.0F);
    scene.select_story_anchor_at({96.0F, 80.0F});
    scene.nudge_editor_selection({8.0F, 6.0F});
    scene.adjust_editor_selection_primary(2.0F);
    scene.update();

    const bool save_ok = scene.save_editor_document(temp_root);
    scene.update();

    const auto saved_document = resonance::load_editor_document(temp_root);
    const auto* saved_anchor = find_anchor(saved_document.story_anchors, "meadow-swing");
    const auto* saved_region = find_region(saved_document.regions, "meadow");
    const auto overlay_lines = scene.overlay_lines();
    const auto summary = scene.debug_summary();

    const bool has_save_overlay = std::find(
        overlay_lines.begin(),
        overlay_lines.end(),
        std::string{"Editor Save: saved"}
    ) != overlay_lines.end();
    const bool has_save_summary = summary.find("EditorSave=saved") != std::string::npos;
    const bool has_clean_summary = summary.find("EditorDirty=clean") != std::string::npos;

    const bool anchor_saved = saved_anchor != nullptr &&
        approx_equal(saved_anchor->position.x, 104.0F) &&
        approx_equal(saved_anchor->position.y, 86.0F) &&
        approx_equal(saved_anchor->activation_radius, 32.0F);
    const bool region_saved = saved_region != nullptr &&
        approx_equal(saved_region->bounds.x, 12.0F) &&
        approx_equal(saved_region->bounds.y, 8.0F) &&
        approx_equal(saved_region->bounds.width, 272.0F) &&
        approx_equal(saved_region->bounds.height, 264.0F);

    std::filesystem::remove_all(temp_root);

    return (save_ok && has_save_overlay && has_save_summary && has_clean_summary && anchor_saved && region_saved) ? 0 : 1;
}
