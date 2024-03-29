project (lvfs)

# Project header
project_header_default ("POSITION_INDEPENDENT_CODE:YES")

# 3rdparty
list (APPEND ${PROJECT_NAME}_LIBS brolly efc)

if (UNIX)
    find_package (X11 REQUIRED)
    include_directories (${X11_INCLUDE_DIR})
    list (APPEND ${PROJECT_NAME}_LIBS ${X11_LIBRARIES})

    list (APPEND ${PROJECT_NAME}_LIBS dl)

    add_definitions (-DPLATFORM_DE_KDE=1)
endif ()

find_package (Threads REQUIRED)
list (APPEND ${PROJECT_NAME}_LIBS ${CMAKE_THREAD_LIBS_INIT})

find_package (LibXml2 REQUIRED)
include_directories (${LIBXML2_INCLUDE_DIR})
list (APPEND ${PROJECT_NAME}_LIBS ${LIBXML2_LIBRARIES})
add_definitions (${LIBXML2_DEFINITIONS})

set (BUILD_MIME_SPEC YES)
set (BUILD_DESKTOP_SPEC YES)
set (BUILD_THEMES_SPEC YES)
set (BUILD_MENU_SPEC YES)
add_subdirectory (libxdg/src)
list (APPEND ${PROJECT_NAME}_LIBS xdg)

# Sources
add_subdirectory (src)

# Target - lvfs
add_library (lvfs SHARED ${${PROJECT_NAME}_SOURCES})
target_compile_features (lvfs PUBLIC cxx_std_14)
target_link_libraries (lvfs ${${PROJECT_NAME}_LIBS})
add_dependencies (lvfs platform)

# Documentation
add_documentation (lvfs 0.0.1 "Liquid Virtual File System")

# Install rules
install_header_files (lvfs "src/desktop/lvfs_Desktop.h:Desktop"
                           "src/lvfs_Error.h:Error"
                           "src/lvfs_IApplication.h:IApplication"
                           "src/lvfs_IApplications.h:IApplications"
                           "src/lvfs_IDirectory.h:IDirectory"
                           "src/lvfs_IEntry.h:IEntry"
                           "src/lvfs_Interface.h:Interface"
                           "src/lvfs_IProperties.h:IProperties"
                           "src/lvfs_IStream.h:IStream"
                           "src/lvfs_IType.h:IType"
                           "src/lvfs_Module.h:Module"

                           "src/settings/lvfs_settings_Option.h:settings/Option"
                           "src/settings/lvfs_settings_IntOption.h:settings/IntOption"
                           "src/settings/lvfs_settings_StringOption.h:settings/StringOption"
                           "src/settings/lvfs_settings_List.h:settings/List"
                           "src/settings/lvfs_settings_Scope.h:settings/Scope"
                           "src/settings/lvfs_settings_Visitor.h:settings/Visitor"
                           "src/settings/lvfs_settings_Instance.h:settings/Instance"

                           "src/plugins/lvfs_IPackage.h:plugins/IPackage"
                           "src/plugins/lvfs_IContentPlugin.h:plugins/IContentPlugin"
                           "src/plugins/lvfs_IProtocolPlugin.h:plugins/IProtocolPlugin"
                           "src/plugins/lvfs_Package.h:plugins/Package")
install_cmake_files ("cmake/FindLvfs.cmake")
install_target (lvfs)
