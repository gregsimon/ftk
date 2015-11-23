cmake_minimum_required(VERSION 2.8.9)

# Use this plugin for precompiled headers.
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
include(cotire)

project (ftk)

include_directories(..)
include_directories(../..)
include_directories(../../third_party/yaml/include)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  include_directories(../../third_party/wxWidgets/build-debug/lib/wx/include/gtk2-unicode-static-3.0)
else()
  include_directories(../../third_party/wxWidgets/build-release/lib/wx/include/gtk2-unicode-static-3.0)
endif()
include_directories(../../third_party/wxWidgets/include)


# flags specific to wxWidgets
add_definitions(-D_FILE_OFFSET_BITS=64)
add_definitions(-D__WXGTK__)
add_definitions(-DWX_PRECOMP)


set ( SOURCES
  ../../src/main.cpp
  ../../adb/ftk-adb.cpp
  ../../adb/usb_endpoint_linux.cpp
  ../../src/linux/ftk_platform_linux.cpp
  ../../src/command_bar.cpp
  ../../src/project_frame.cpp
  ../../src/settings.cpp
  ../../src/project.cpp
  ../../src/console.cpp
  ../../src/sdk.cpp

)

file (GLOB YAML_SOURCES ../../third_party/yaml/src/*.c)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  link_directories(../../third_party/wxWidgets/build-debug/lib)
else()
  link_directories(../../third_party/wxWidgets/build-release/lib)
endif()

add_executable(ftk ${SOURCES} ${YAML_SOURCES})
set_target_properties(ftk PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT 
  "../../third_party/wxWidgets/include/wx/wxprec.h")
cotire(ftk)

set ( PROJECT_LINK_LIBS 
  libwx_gtk2u-3.0.a
  libwx_gtk2u_gl-3.0.a
  libwxregexu-3.0.a
  libwxscintilla-3.0.a
  libwxtiff-3.0.a
  X11 gtk-x11-2.0 gdk-x11-2.0 gdk-x11-2.0
  pango-1.0 cairo freetype gthread-2.0 fontconfig freetype
  pthread dl m Xxf86vm SM
  gobject-2.0 gmodule-2.0 gio-2.0
  jpeg z png 
  glib-2.0 pangocairo-1.0 pangoft2-1.0
  gdk_pixbuf-2.0 gdk_pixbuf-2.0
  )

target_link_libraries(ftk ${PROJECT_LINK_LIBS} )

