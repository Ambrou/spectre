# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "ControlSystemHelpers")

set(LIBRARY_SOURCES
  FoTUpdater_Helper.cpp
  )

add_spectre_library(${LIBRARY} ${LIBRARY_SOURCES})

target_link_libraries(
  ${LIBRARY}
  INTERFACE
  DataStructures
  ControlSystem

  PRIVATE
  FunctionsOfTime
  Utilities
  )
