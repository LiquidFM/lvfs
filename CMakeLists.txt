project (lvfs)

# Project header
project_cxx_library_header_default ()

# Sources
add_subdirectory (src)

# Target - lvfs
add_library(lvfs STATIC ${${PROJECT_NAME}_SOURCES})
target_link_libraries (lvfs ${${PROJECT_NAME}_LIBS})

# Version
build_version_file (lvfs 0.0.1)

# Documentation
add_documentation ("liquidDB" 0.0.1 "This library is a front-end for SQLite database")
