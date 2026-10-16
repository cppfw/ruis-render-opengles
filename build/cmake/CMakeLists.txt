cmake_minimum_required(VERSION 3.30)

set(name ruis-render-opengles)
project(${name})

set(dir_name ruis)

# !!! find_package must go after project() declaration !!!
# Otherwise VCPKG does not set the CMAKE_PREFIX_PATH to find packages.
find_package(myci CONFIG REQUIRED)

set(srcs)
myci_add_source_files(srcs
    DIRECTORY
        ../../src/${dir_name}
    RECURSIVE
)

myci_declare_library(${name}
    SOURCES
        ${srcs}
    PUBLIC_INCLUDE_DIRECTORIES
        ../../src
    INSTALL_INCLUDE_DIRECTORIES
        ../../src/${dir_name}
    DEPENDENCIES
        ruis
    LINUX_ONLY_DEPENDENCIES
        PkgConfig::glesv2
    WINDOWS_ONLY_DEPENDENCIES
        unofficial-angle/unofficial::angle::libGLESv2
)

if(APPLE)
    target_link_libraries(${name}
        PRIVATE
            # Link the iOS frameworks directly using the -framework flag string.
            # The quotes are important so CMake passes it as a single linker argument.
            "-framework OpenGLES"
    )

    # Define the GLES_SILENCE_DEPRECATION macro to silence OpenGL ES deprecation warnings on Apple platforms.
    target_compile_definitions(${name} PRIVATE GLES_SILENCE_DEPRECATION)
endif()
