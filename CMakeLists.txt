project (lvfs)

# Project header
project_cxx_library_header_default ()

if (UNIX)
    find_package (X11 REQUIRED)
    include_directories (${X11_INCLUDE_DIR})
    list (APPEND ${PROJECT_NAME}_LIBS ${X11_LIBRARIES})
endif ()

# Sources
add_subdirectory (src)

# Target - lvfs
add_library(lvfs STATIC ${${PROJECT_NAME}_SOURCES})
target_link_libraries (lvfs ${${PROJECT_NAME}_LIBS} dl pthread)

# Version
build_version_file (lvfs 0.0.1)

# Documentation
add_documentation ("LVFS" 0.0.1 "Liquid Virtual File System")
