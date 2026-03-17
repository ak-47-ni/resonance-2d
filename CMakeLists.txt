cmake_minimum_required(VERSION 3.20)

project(resonance_2d LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(cmake/Warnings.cmake)

find_package(nlohmann_json REQUIRED CONFIG)

set(RESONANCE_SDL_TARGET "")
find_package(SDL3 QUIET CONFIG)
if(SDL3_FOUND)
    set(RESONANCE_SDL_TARGET SDL3::SDL3)
endif()

if(NOT RESONANCE_SDL_TARGET)
    find_package(SDL2 QUIET CONFIG)
    if(SDL2_FOUND)
        set(RESONANCE_SDL_TARGET SDL2::SDL2)
    endif()
endif()

add_library(resonance_engine
    engine/core/Application.cpp
    engine/core/FileSystem.cpp
    engine/core/Time.cpp
    engine/editor/EditorDocument.cpp
    engine/world/RegionData.cpp
    engine/world/StoryAnchorData.cpp
    engine/world/Trigger.cpp
    engine/world/World.cpp
    engine/audio/MusicStateData.cpp
    engine/audio/AudioToneProfile.cpp
    engine/audio/AudioDirector.cpp
    engine/audio/WavAudioAsset.cpp
    engine/audio/AudioBackendSDL.cpp
    engine/event/EventData.cpp
    engine/event/EventDirector.cpp
    engine/debug/TraceLog.cpp
    engine/debug/DebugOverlay.cpp
)

target_include_directories(resonance_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(resonance_engine PUBLIC cxx_std_20)
target_link_libraries(resonance_engine PUBLIC nlohmann_json::nlohmann_json)
resonance_set_warnings(resonance_engine)

if(RESONANCE_SDL_TARGET)
    target_link_libraries(resonance_engine PUBLIC ${RESONANCE_SDL_TARGET})
    target_compile_definitions(resonance_engine PUBLIC RESONANCE_HAS_SDL=1)
else()
    target_compile_definitions(resonance_engine PUBLIC RESONANCE_HAS_SDL=0)
endif()

add_executable(resonance_demo
    game/demo/main.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(resonance_demo PRIVATE resonance_engine)
resonance_set_warnings(resonance_demo)

add_executable(startup_smoke_test
    tests/smoke/test_startup.cpp
)

target_link_libraries(startup_smoke_test PRIVATE resonance_engine)
resonance_set_warnings(startup_smoke_test)

add_executable(content_load_test
    tests/data/test_content_load.cpp
)

target_link_libraries(content_load_test PRIVATE resonance_engine)
resonance_set_warnings(content_load_test)

add_executable(region_switch_test
    tests/world/test_region_switch.cpp
)

target_link_libraries(region_switch_test PRIVATE resonance_engine)
resonance_set_warnings(region_switch_test)

add_executable(audio_state_test
    tests/audio/test_audio_state.cpp
)

target_link_libraries(audio_state_test PRIVATE resonance_engine)
resonance_set_warnings(audio_state_test)

add_executable(audio_tone_profile_test
    tests/audio/test_audio_tone_profile.cpp
)

add_executable(audio_backend_controls_test
    tests/audio/test_audio_backend_controls.cpp
)

target_link_libraries(audio_backend_controls_test PRIVATE resonance_engine)
resonance_set_warnings(audio_backend_controls_test)

if(RESONANCE_SDL_TARGET)
add_executable(wav_audio_asset_test
    tests/audio/test_wav_audio_asset.cpp
)

target_link_libraries(wav_audio_asset_test PRIVATE resonance_engine)
resonance_set_warnings(wav_audio_asset_test)
endif()

target_link_libraries(audio_tone_profile_test PRIVATE resonance_engine)
resonance_set_warnings(audio_tone_profile_test)

add_executable(event_director_test
    tests/event/test_event_director.cpp
)

target_link_libraries(event_director_test PRIVATE resonance_engine)
resonance_set_warnings(event_director_test)

add_executable(trace_log_test
    tests/debug/test_trace_log.cpp
)

target_link_libraries(trace_log_test PRIVATE resonance_engine)
resonance_set_warnings(trace_log_test)

add_executable(demo_slice_test
    tests/data/test_demo_slice.cpp
)

target_link_libraries(demo_slice_test PRIVATE resonance_engine)
resonance_set_warnings(demo_slice_test)

add_executable(story_anchor_content_test
    tests/data/test_story_anchor_content.cpp
)

target_link_libraries(story_anchor_content_test PRIVATE resonance_engine)
resonance_set_warnings(story_anchor_content_test)

add_executable(demo_runtime_test
    tests/demo/test_demo_runtime.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(demo_runtime_test PRIVATE resonance_engine)
resonance_set_warnings(demo_runtime_test)

add_executable(demo_event_rotation_test
    tests/demo/test_demo_event_rotation.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(demo_event_rotation_test PRIVATE resonance_engine)
resonance_set_warnings(demo_event_rotation_test)

add_executable(demo_visual_state_test
    tests/demo/test_demo_visual_state.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(demo_visual_state_test PRIVATE resonance_engine)
resonance_set_warnings(demo_visual_state_test)

add_executable(story_anchor_interaction_test
    tests/demo/test_story_anchor_interaction.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(story_anchor_interaction_test PRIVATE resonance_engine)
resonance_set_warnings(story_anchor_interaction_test)

add_executable(story_anchor_visuals_test
    tests/demo/test_story_anchor_visuals.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(story_anchor_visuals_test PRIVATE resonance_engine)
resonance_set_warnings(story_anchor_visuals_test)

add_executable(story_anchor_audio_focus_test
    tests/demo/test_story_anchor_audio_focus.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(story_anchor_audio_focus_test PRIVATE resonance_engine)
resonance_set_warnings(story_anchor_audio_focus_test)

add_executable(event_audio_emphasis_test
    tests/demo/test_event_audio_emphasis.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(event_audio_emphasis_test PRIVATE resonance_engine)
resonance_set_warnings(event_audio_emphasis_test)

add_executable(memory_journal_test
    tests/demo/test_memory_journal.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(memory_journal_test PRIVATE resonance_engine)
resonance_set_warnings(memory_journal_test)

add_executable(journal_overlay_test
    tests/demo/test_journal_overlay.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(journal_overlay_test PRIVATE resonance_engine)
resonance_set_warnings(journal_overlay_test)

add_executable(memory_unlock_event_test
    tests/demo/test_memory_unlock_event.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(memory_unlock_event_test PRIVATE resonance_engine)
resonance_set_warnings(memory_unlock_event_test)

add_executable(multi_memory_event_unlock_test
    tests/demo/test_multi_memory_event_unlock.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(multi_memory_event_unlock_test PRIVATE resonance_engine)
resonance_set_warnings(multi_memory_event_unlock_test)

add_executable(three_stage_memory_chain_test
    tests/demo/test_three_stage_memory_chain.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(three_stage_memory_chain_test PRIVATE resonance_engine)
resonance_set_warnings(three_stage_memory_chain_test)

add_executable(overlay_panel_test
    tests/debug/test_overlay_panel.cpp
)

target_link_libraries(overlay_panel_test PRIVATE resonance_engine)
resonance_set_warnings(overlay_panel_test)

add_executable(editor_document_test
    tests/editor/test_editor_document.cpp
)

target_link_libraries(editor_document_test PRIVATE resonance_engine)
resonance_set_warnings(editor_document_test)

if(BUILD_TESTING)
    add_test(NAME startup_smoke COMMAND startup_smoke_test)
    add_test(NAME content_load COMMAND content_load_test)
    add_test(NAME region_switch COMMAND region_switch_test)
    add_test(NAME audio_state COMMAND audio_state_test)
    add_test(NAME audio_tone_profile COMMAND audio_tone_profile_test)
    add_test(NAME audio_backend_controls COMMAND audio_backend_controls_test)
    add_test(NAME audio_asset_generator COMMAND python3 tests/audio/test_generate_audio_assets.py)
    if(RESONANCE_SDL_TARGET)
        add_test(NAME wav_audio_asset COMMAND wav_audio_asset_test)
        set_tests_properties(wav_audio_asset PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    add_test(NAME event_director COMMAND event_director_test)
    add_test(NAME trace_log COMMAND trace_log_test)
    add_test(NAME demo_slice COMMAND demo_slice_test)
    add_test(NAME story_anchor_content COMMAND story_anchor_content_test)
    add_test(NAME demo_runtime COMMAND demo_runtime_test)
    add_test(NAME demo_event_rotation COMMAND demo_event_rotation_test)
    add_test(NAME demo_visual_state COMMAND demo_visual_state_test)
    add_test(NAME story_anchor_interaction COMMAND story_anchor_interaction_test)
    add_test(NAME story_anchor_visuals COMMAND story_anchor_visuals_test)
    add_test(NAME story_anchor_audio_focus COMMAND story_anchor_audio_focus_test)
    add_test(NAME event_audio_emphasis COMMAND event_audio_emphasis_test)
    add_test(NAME memory_journal COMMAND memory_journal_test)
    add_test(NAME journal_overlay COMMAND journal_overlay_test)
    add_test(NAME memory_unlock_event COMMAND memory_unlock_event_test)
    add_test(NAME multi_memory_event_unlock COMMAND multi_memory_event_unlock_test)
    add_test(NAME three_stage_memory_chain COMMAND three_stage_memory_chain_test)
    add_test(NAME overlay_panel COMMAND overlay_panel_test)
    add_test(NAME editor_document COMMAND editor_document_test)
    add_test(NAME demo_hidden_runtime
        COMMAND ${CMAKE_COMMAND} -E env SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 $<TARGET_FILE:resonance_demo>
    )
    set_tests_properties(content_load PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(audio_backend_controls PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(audio_asset_generator PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(region_switch PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(event_director PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_slice PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(story_anchor_content PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_runtime PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_event_rotation PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_visual_state PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(story_anchor_interaction PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(story_anchor_visuals PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(story_anchor_audio_focus PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(event_audio_emphasis PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(memory_journal PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(journal_overlay PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(memory_unlock_event PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(multi_memory_event_unlock PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(three_stage_memory_chain PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(editor_document PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_hidden_runtime PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
