#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(A2Geant4)

# workaround for ubuntu: check if gold linker is available and use it
execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
if ("${LD_VERSION}" MATCHES "GNU gold")
  set(DEFAULT_LINKER_FLAGS "${DEFAULT_LINKER_FLAGS} -Wl,-fuse-ld=gold")
message(STATUS "GNU google ld (gold) linker will be used.")
endif()
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DEFAULT_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${DEFAULT_LINKER_FLAGS}")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build target with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# define variables
set(EXT_LIBRARIES)

# use copied CMake modules from ROOT 6
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED MODULE COMPONENTS MathCore RIO Hist Tree TreePlayer Geom EG)

# locate Pluto
find_package(Pluto)
if (Pluto_FOUND)
  if (ROOT_VERSION VERSION_LESS 6)
    message(WARNING "ROOT 6 is needed for the Pluto reader, disabling Pluto support")
  else()
    include_directories(${Pluto_INCLUDE_DIRS})
    set(EXT_LIBRARIES ${EXT_LIBRARIES} ${Pluto_LIBRARIES})
    add_definitions(-DWITH_PLUTO)
  endif()
endif()

# define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(A2Geant4 ${PROJECT_SOURCE_DIR}/src/A2.cc ${sources} ${headers})
target_link_libraries(A2Geant4 ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${EXT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build A2Geant4. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
#set(EXAMPLE_HardBeam_SCRIPTS
#  macros/run.mac
#  macros/vis.mac
#  macros/init_vis.mac
#  )
#
#foreach(_script ${EXAMPLE_HardBeam_SCRIPTS})
#  configure_file(
#    ${PROJECT_SOURCE_DIR}/${_script}
#    ${PROJECT_BINARY_DIR}/${_script}
#    COPYONLY
#    )
#endforeach()
#
#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(HardBeam DEPENDS hardbeam)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
#install(TARGETS A2Geant4 DESTINATION bin)

