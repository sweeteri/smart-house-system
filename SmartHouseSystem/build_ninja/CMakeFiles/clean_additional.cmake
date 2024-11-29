# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/SmartHouseSystem_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/SmartHouseSystem_autogen.dir/ParseCache.txt"
  "SmartHouseSystem_autogen"
  )
endif()
