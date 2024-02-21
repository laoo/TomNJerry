cmake_minimum_required( VERSION 3.25 )

project( "TomNJery" )

set( CMAKE_CXX_STANDARD 23 )

set( CMAKE_CXX_FLAGS "-Wno-c++11-narrowing")

add_executable( Tom
  Tom.cpp
  TomMain.cpp
  Log.cpp
  RAM.cpp
)

add_executable( Jerry
  BS94.cpp
  DSPRaw.cpp
  Jerry.cpp
  JerryMain.cpp
  Log.cpp
  PipelineLog.cpp
  ProgramOptions.cpp
  RAM.cpp
  wav.c
)

set_source_files_properties(wav.c PROPERTIES
  SKIP_PRECOMPILE_HEADERS ON
  COMPILE_DEFINITIONS "__LITTLE_ENDIAN__"
)

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
