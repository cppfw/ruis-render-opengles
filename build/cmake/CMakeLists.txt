cmake_minimum_required(VERSION 3.20)

set(name ruis-render-opengles)
project(${name})

set(dir_name ruis)

# !!! find_package must go after project() declaration !!!
# Otherwise VCPKG does not set the CMAKE_PREFIX_PATH to find packages.
find_package(myci CONFIG REQUIRED)

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype CONFIG REQUIRED)

if(WIN32)
    find_package(unofficial-angle CONFIG REQUIRED)
    set(gles2_dep unofficial::angle::libGLESv2)
elseif(LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLES2 REQUIRED IMPORTED_TARGET "glesv2")
    set(gles2_dep PkgConfig::GLES2)
else()
    set(gles2_dep)
endif()

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
        rasterimage
        r4
        papki
        tml
        utki
        agg
        veg
        mikroxml
        cssom
        svgdom
        svgren
        ruis
    EXTERNAL_DEPENDENCIES
        JPEG::JPEG
        PNG::PNG
        Freetype::Freetype
        ${gles2_dep}
)
