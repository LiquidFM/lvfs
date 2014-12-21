project (lvfs)

# Project header
project_cxx_library_header_default ()

# 3rdparty
list (APPEND ${PROJECT_NAME}_LIBS platform brolly efc xdg)

if (UNIX)
    find_package (X11 REQUIRED)
    include_directories (${X11_INCLUDE_DIR})
    list (APPEND ${PROJECT_NAME}_LIBS ${X11_LIBRARIES})

    list (APPEND ${PROJECT_NAME}_LIBS dl)
endif ()

find_package (Threads REQUIRED)
list (APPEND ${PROJECT_NAME}_LIBS ${CMAKE_THREAD_LIBS_INIT})

# Sources
add_subdirectory (src)

# Target - lvfs
add_library (lvfs SHARED ${${PROJECT_NAME}_SOURCES})
target_link_libraries (lvfs ${${PROJECT_NAME}_LIBS})

# Version
build_version_file (lvfs 0.0.1)

# Documentation
add_documentation ("LVFS" 0.0.1 "Liquid Virtual File System")

# Install rules
install_header_files (lvfs "src/desktop/lvfs_Desktop.h:Desktop"
                           "src/lvfs_Error.h:Error"
                           "src/lvfs_IApplication.h:IApplication"
                           "src/lvfs_IApplications.h:IApplications"
                           "src/lvfs_IDirectory.h:IDirectory"
                           "src/lvfs_IEntry.h:IEntry"
                           "src/lvfs_IFile.h:IFile"
                           "src/lvfs_IFsFile.h:IFsFile"
                           "src/lvfs_Interface.h:Interface"
                           "src/lvfs_IType.h:IType"
                           "src/lvfs_Module.h:Module"

                           "src/plugins/lvfs_IContentPlugin.h:plugins/IContentPlugin"
                           "src/plugins/lvfs_IPlugin.h:plugins/IPlugin"
                           "src/plugins/lvfs_IProtocolPlugin.h:plugins/IProtocolPlugin"
                           "src/plugins/lvfs_Package.h:plugins/Package")
install_target (lvfs)
