# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\QtEnglishListen_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\QtEnglishListen_autogen.dir\\ParseCache.txt"
  "QtEnglishListen_autogen"
  )
endif()
