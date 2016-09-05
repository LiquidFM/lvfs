# - Find lvfs
# Find the lvfs library
#
#  This module defines the following variables:
#     LVFS_FOUND   - True if LVFS_INCLUDE is found
#     LVFS_INCLUDE - where to find header files
#     LVFS_LIB     - the library files


if (NOT LVFS_FOUND)
    find_path (LVFS_INCLUDE
               NAMES "lvfs/Module"
               PATH_SUFFIXES include
               DOC "Lvfs include directory")

    find_library (LVFS_LIB
                  NAMES lvfs
                  DOC "Lvfs library file")

    # handle the QUIETLY and REQUIRED arguments and set LVFS_FOUND to TRUE if all listed variables are TRUE
    include (FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS (Lvfs DEFAULT_MSG LVFS_INCLUDE LVFS_LIB)

    mark_as_advanced (LVFS_INCLUDE LVFS_LIB)
endif ()
