#include "engine/editor/EditorDocument.h"
#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"
#include "game/demo/WorldWorkspaceViewModel.h"

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

const resonance::StoryAnchorVisual* find_anchor_visual(
    const std::vector<resonance::StoryAnchorVisual>& anchors,
    const std::string& id
) {
    for (const auto& anchor : anchors) {
        if (anchor.id == id) {
            return &anchor;
        }
    }
    return nullptr;
}

resonance::WorldPosition region_pick_point(const resonance::RegionData& region) {
    return resonance::WorldPosition{region.bounds.x + 1.0F, region.bounds.y + 1.0F};
}

}  // namespace

int main() {
    const auto temp_root = make_temp_root();
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    const auto* region_ptr = find_region(scene.regions(), "meadow");
    const auto anchor_visuals = scene.story_anchor_visuals();
    const auto* anchor_visual_ptr = find_anchor_visual(anchor_visuals, "meadow-swing");
    if (region_ptr == nullptr || anchor_visual_ptr == nullptr) {
        return 1;
    }

    const auto region = *region_ptr;
    const auto anchor_visual = *anchor_visual_ptr;

    scene.toggle_editor_mode();
    scene.select_region_at(region_pick_point(region));
    scene.nudge_editor_selection({12.0F, 8.0F});
    scene.adjust_editor_selection_primary(2.0F);
    scene.select_story_anchor_at(anchor_visual.position);
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
    const auto workspace_state = scene.world_workspace_state();
    const auto workspace_view_model = resonance::build_world_workspace_view_model(workspace_state);

    const bool has_save_overlay = std::find(
        overlay_lines.begin(),
        overlay_lines.end(),
        std::string{"Editor Save: saved"}
    ) != overlay_lines.end();
    const bool has_save_summary = summary.find("EditorSave=saved") != std::string::npos;
    const bool has_clean_summary = summary.find("EditorDirty=clean") != std::string::npos;
    const bool has_saved_workspace_state = workspace_state.mode_label == "Edit" &&
                                          workspace_state.selection_summary == "Story Anchor: meadow-swing";
    const bool has_saved_workspace_view_model = workspace_view_model.dirty_label == "dirty:false" &&
                                               workspace_view_model.save_status_label == "save:saved";

    const bool anchor_saved = saved_anchor != nullptr &&
        approx_equal(saved_anchor->position.x, anchor_visual.position.x + 8.0F) &&
        approx_equal(saved_anchor->position.y, anchor_visual.position.y + 6.0F) &&
        approx_equal(saved_anchor->activation_radius, anchor_visual.activation_radius + 4.0F);
    const bool region_saved = saved_region != nullptr &&
        approx_equal(saved_region->bounds.x, region.bounds.x + 12.0F) &&
        approx_equal(saved_region->bounds.y, region.bounds.y + 8.0F) &&
        approx_equal(saved_region->bounds.width, region.bounds.width + 16.0F) &&
        approx_equal(saved_region->bounds.height, region.bounds.height + 8.0F);

    std::filesystem::remove_all(temp_root);

    return (save_ok &&
            has_save_overlay &&
            has_save_summary &&
            has_clean_summary &&
            has_saved_workspace_state &&
            has_saved_workspace_view_model &&
            anchor_saved &&
            region_saved)
        ? 0
        : 1;
}
