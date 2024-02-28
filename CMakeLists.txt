cmake_minimum_required( VERSION 3.25 )

option( ENABLE_LOGGING "Enable logging to stdout" OFF )

project( TomNJerry )

set( CMAKE_CXX_STANDARD 23 )

if ( NOT WIN32 )
  set( CMAKE_CXX_FLAGS "-Wno-c++11-narrowing")
endif()

add_executable( Tom
  Tom.cpp
  TomMain.cpp
  Log.cpp
  RAM.cpp
)

add_executable( Jerry
  BS94.cpp
  Bus.cpp
  DSPRaw.cpp
  Jerry.cpp
  JerryMain.cpp
  Log.cpp
  PipelineLog.cpp
  ProgramOptions.cpp
  RAM.cpp
  wav.c
  WaveOut.cpp
)

set_source_files_properties(wav.c PROPERTIES
  SKIP_PRECOMPILE_HEADERS ON
  COMPILE_DEFINITIONS "__LITTLE_ENDIAN__"
)

if ( ENABLE_LOGGING)
  set_target_properties( Jerry PROPERTIES
    COMPILE_DEFINITIONS LOG_PIPELINE
  )
endif()

target_precompile_headers( Tom PRIVATE
  <algorithm>
  <atomic>
  <array>
  <bit>
  <cassert>
  <chrono>
  <climits>
  <cstddef>
  <cstdint>
  <exception>
  <filesystem>
  <fstream>
  <functional>
  <iostream>
  <memory>
  <span>
  <string>
  <thread>
  <vector>
)

target_precompile_headers( Jerry PRIVATE
  <algorithm>
  <atomic>
  <array>
  <bit>
  <cassert>
  <chrono>
  <climits>
  <cstddef>
  <cstdint>
  <exception>
  <filesystem>
  <fstream>
  <functional>
  <iostream>
  <memory>
  <optional>
  <sstream>
  <stdexcept>
  <string>
  <span>
  <thread>
  <vector>
)
