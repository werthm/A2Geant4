# - Try to find Pluto
# Once done this will define
#  Pluto_FOUND - System has Pluto
#  Pluto_INCLUDE_DIRS - The Pluto include directories
#  Pluto_LIBRARIES - The libraries needed to use Pluto 

Set(Pluto_SEARCHPATH
        $ENV{PLUTOSYS}
)

find_library(Pluto_LIBRARY NAMES Pluto PATHS ${Pluto_SEARCHPATH} PATH_SUFFIXES lib)
find_path(Pluto_INCLUDE_DIR NAMES PParticle.h PATHS ${Pluto_SEARCHPATH}/.. PATH_SUFFIXES include)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Pluto_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Pluto DEFAULT_MSG
                                  Pluto_LIBRARY Pluto_INCLUDE_DIR)

mark_as_advanced(Pluto_INCLUDE_DIR Pluto_LIBRARY)

set(Pluto_LIBRARIES ${Pluto_LIBRARY} )
set(Pluto_INCLUDE_DIRS ${Pluto_INCLUDE_DIR} )
