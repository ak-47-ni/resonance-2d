#include "engine/event/EventData.h"
#include "game/demo/DemoScene.h"

#include <algorithm>
#include <string>

int main() {
    const auto bundle = resonance::load_demo_content("assets/data");
    resonance::DemoScene scene(bundle);

    scene.set_player_position({96.0F, 80.0F});
    scene.update();

    const auto lines = scene.overlay_lines();
    const bool has_mode_play_line = std::find(lines.begin(), lines.end(), std::string{"Mode: Play"}) != lines.end();
    const bool has_region_line = std::find(lines.begin(), lines.end(), std::string{"Region: meadow"}) != lines.end();
    const bool has_music_line = std::find(lines.begin(), lines.end(), std::string{"Music: mysterious"}) != lines.end();
    const bool has_prompt_line = std::find(lines.begin(), lines.end(), std::string{"Action: Press E to listen"}) != lines.end();
    const bool has_nearby_anchor_line = std::find(lines.begin(), lines.end(), std::string{"Nearby Anchor: meadow-swing"}) != lines.end();
    const bool has_story_focus_line = std::find(lines.begin(), lines.end(), std::string{"Story Focus: 1.00"}) != lines.end();
    const bool has_event_emphasis_line = std::find(lines.begin(), lines.end(), std::string{"Event Emphasis: 1.00"}) != lines.end();
    const bool has_event_duck_line = std::find(lines.begin(), lines.end(), std::string{"Event Duck: 0.85"}) != lines.end();
    const bool has_ambient_boost_line = std::find(lines.begin(), lines.end(), std::string{"Ambient Boost: 0.65"}) != lines.end();
    const bool has_bgm_gain_line = std::find(lines.begin(), lines.end(), std::string{"Bgm Gain: 0.70"}) != lines.end();
    const bool has_ambient_gain_line = std::find(lines.begin(), lines.end(), std::string{"Ambient Gain: 1.86"}) != lines.end();

    scene.toggle_editor_mode();
    scene.update();
    const auto edit_lines = scene.overlay_lines();
    const bool has_mode_edit_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Mode: Edit"}) != edit_lines.end();
    const bool has_editor_controls_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Editor Controls: Click select | Drag/WASD move | Wheel/[ ] size | F5 save | Backspace clear"}) != edit_lines.end();
    const bool has_editor_selection_none_line = std::find(edit_lines.begin(), edit_lines.end(), std::string{"Editor Selection: none"}) != edit_lines.end();
    const bool selected_region = scene.select_region_at({24.0F, 24.0F});
    scene.update();
    const auto region_edit_lines = scene.overlay_lines();
    const bool has_selected_region_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Selected Region: meadow"}) != region_edit_lines.end();
    const bool has_region_inspector_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Inspector: Region"}) != region_edit_lines.end();
    const bool has_region_music_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Inspector Music: explore"}) != region_edit_lines.end();
    const bool has_editor_dirty_clean_line = std::find(region_edit_lines.begin(), region_edit_lines.end(), std::string{"Editor Dirty: no"}) != region_edit_lines.end();
    const bool moved_region = scene.nudge_editor_selection({12.0F, 8.0F});
    const bool resized_region = scene.adjust_editor_selection_primary(2.0F);
    scene.update();
    const auto moved_region_lines = scene.overlay_lines();
    const bool has_region_bounds_line = std::find(moved_region_lines.begin(), moved_region_lines.end(), std::string{"Inspector Bounds: 12.0, 8.0, 272.0, 264.0"}) != moved_region_lines.end();
    const bool has_editor_dirty_yes_line = std::find(moved_region_lines.begin(), moved_region_lines.end(), std::string{"Editor Dirty: yes"}) != moved_region_lines.end();
    const bool selected_anchor = scene.select_story_anchor_at({96.0F, 80.0F});
    scene.update();
    const auto selected_edit_lines = scene.overlay_lines();
    const bool has_selected_anchor_line = std::find(selected_edit_lines.begin(), selected_edit_lines.end(), std::string{"Selected Anchor: meadow-swing"}) != selected_edit_lines.end();
    const bool has_anchor_inspector_line = std::find(selected_edit_lines.begin(), selected_edit_lines.end(), std::string{"Inspector: Story Anchor"}) != selected_edit_lines.end();
    const bool has_anchor_region_line = std::find(selected_edit_lines.begin(), selected_edit_lines.end(), std::string{"Inspector Region: meadow"}) != selected_edit_lines.end();
    const bool moved_anchor = scene.nudge_editor_selection({8.0F, 6.0F});
    const bool resized_anchor = scene.adjust_editor_selection_primary(2.0F);
    scene.update();
    const auto manipulated_edit_lines = scene.overlay_lines();
    const bool has_selected_position_line = std::find(manipulated_edit_lines.begin(), manipulated_edit_lines.end(), std::string{"Selected Position: 104.0, 86.0"}) != manipulated_edit_lines.end();
    const bool has_selected_radius_line = std::find(manipulated_edit_lines.begin(), manipulated_edit_lines.end(), std::string{"Selected Radius: 32.00"}) != manipulated_edit_lines.end();
    const bool cleared_selection = scene.clear_editor_selection();
    scene.update();
    const auto cleared_selection_lines = scene.overlay_lines();
    const bool has_cleared_selection_line = std::find(cleared_selection_lines.begin(), cleared_selection_lines.end(), std::string{"Editor Selection: none"}) != cleared_selection_lines.end();
    const bool cleared_selected_region_line = std::find_if(cleared_selection_lines.begin(), cleared_selection_lines.end(), [](const std::string& line) {
        return line.find("Selected Region:") == 0;
    }) == cleared_selection_lines.end();
    const bool cleared_selected_anchor_line = std::find_if(cleared_selection_lines.begin(), cleared_selection_lines.end(), [](const std::string& line) {
        return line.find("Selected Anchor:") == 0;
    }) == cleared_selection_lines.end();

    scene.toggle_editor_mode();
    scene.update();
    scene.interact();
    scene.update();

    const auto story_lines = scene.overlay_lines();
    const bool has_story_line = std::any_of(story_lines.begin(), story_lines.end(), [](const std::string& line) {
        return line.find("Story: ") == 0;
    });
    const bool has_active_anchor_line = std::find(story_lines.begin(), story_lines.end(), std::string{"Active Anchor: meadow-swing"}) != story_lines.end();

    scene.set_player_position({999.0F, 999.0F});
    scene.update();

    return (scene.current_region_id() == "" &&
            scene.current_music_state() == "" &&
            scene.current_event_id() == "" &&
            scene.current_interaction_prompt() == "" &&
            has_mode_play_line &&
            has_region_line &&
            has_music_line &&
            has_prompt_line &&
            has_nearby_anchor_line &&
            has_story_focus_line &&
            has_event_emphasis_line &&
            has_event_duck_line &&
            has_ambient_boost_line &&
            has_bgm_gain_line &&
            has_ambient_gain_line &&
            has_mode_edit_line &&
            has_editor_controls_line &&
            has_editor_selection_none_line &&
            selected_region &&
            has_selected_region_line &&
            has_region_inspector_line &&
            has_region_music_line &&
            has_editor_dirty_clean_line &&
            moved_region &&
            resized_region &&
            has_region_bounds_line &&
            has_editor_dirty_yes_line &&
            selected_anchor &&
            has_selected_anchor_line &&
            has_anchor_inspector_line &&
            has_anchor_region_line &&
            moved_anchor &&
            resized_anchor &&
            has_selected_position_line &&
            has_selected_radius_line &&
            cleared_selection &&
            has_cleared_selection_line &&
            cleared_selected_region_line &&
            cleared_selected_anchor_line &&
            has_story_line &&
            has_active_anchor_line)
        ? 0
        : 1;
}
