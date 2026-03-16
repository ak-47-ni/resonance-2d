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
    engine/world/RegionData.cpp
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

add_executable(demo_runtime_test
    tests/demo/test_demo_runtime.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(demo_runtime_test PRIVATE resonance_engine)
resonance_set_warnings(demo_runtime_test)

add_executable(demo_visual_state_test
    tests/demo/test_demo_visual_state.cpp
    game/demo/DemoScene.cpp
)

target_link_libraries(demo_visual_state_test PRIVATE resonance_engine)
resonance_set_warnings(demo_visual_state_test)

add_executable(overlay_panel_test
    tests/debug/test_overlay_panel.cpp
)

target_link_libraries(overlay_panel_test PRIVATE resonance_engine)
resonance_set_warnings(overlay_panel_test)

if(BUILD_TESTING)
    add_test(NAME startup_smoke COMMAND startup_smoke_test)
    add_test(NAME content_load COMMAND content_load_test)
    add_test(NAME region_switch COMMAND region_switch_test)
    add_test(NAME audio_state COMMAND audio_state_test)
    add_test(NAME audio_tone_profile COMMAND audio_tone_profile_test)
    add_test(NAME audio_asset_generator COMMAND python3 tests/audio/test_generate_audio_assets.py)
    if(RESONANCE_SDL_TARGET)
        add_test(NAME wav_audio_asset COMMAND wav_audio_asset_test)
        set_tests_properties(wav_audio_asset PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    add_test(NAME event_director COMMAND event_director_test)
    add_test(NAME trace_log COMMAND trace_log_test)
    add_test(NAME demo_slice COMMAND demo_slice_test)
    add_test(NAME demo_runtime COMMAND demo_runtime_test)
    add_test(NAME demo_visual_state COMMAND demo_visual_state_test)
    add_test(NAME overlay_panel COMMAND overlay_panel_test)
    add_test(NAME demo_hidden_runtime
        COMMAND ${CMAKE_COMMAND} -E env SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy RESONANCE_DEMO_HIDDEN=1 RESONANCE_DEMO_AUTOCLOSE=1 $<TARGET_FILE:resonance_demo>
    )
    set_tests_properties(content_load PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(audio_asset_generator PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(region_switch PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(event_director PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_slice PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_runtime PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_visual_state PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(demo_hidden_runtime PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
