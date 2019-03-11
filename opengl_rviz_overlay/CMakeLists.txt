cmake_minimum_required(VERSION 2.8)

# Uncomment to enable QML (currently with a performance loss for qwidgets rendering which is why it is disabled by default)
#add_definitions(-DRENDER_OVERLAYS_USING_OPENGL)
# Uncomment to print the render time in ms in each frame (Useful for debugging)
#add_definitions(-DDRAW_RENDERTIME)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

project(opengl_rviz_overlay)

set(CMAKE_AUTOMOC ON)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rviz
  )


## System dependencies are found with CMake's conventions
#find_package(Qt5 REQUIRED Core Gui)
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5Quick)
find_package(Boost REQUIRED)

# We also use Ogre
include($ENV{ROS_ROOT}/core/rosbuild/FindPkgConfig.cmake)
pkg_check_modules(OGRE OGRE)
link_directories( ${OGRE_LIBRARY_DIRECTORIES} )

#include(${QT_USE_FILE})

include_directories(
  include
  ${OGRE_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Gui_INCLUDE_DIRS}
  ${Qt5Quick_INCLUDE_DIRS}
)

set(SOURCES
  src/display_wrapper.cpp
  src/overlay.cpp
  )

set(HEADERS
  include/opengl_rviz_overlay/display_wrapper.h
  include/opengl_rviz_overlay/overlay.h
  )

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp rviz
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Quick ${Boost_LIBRARIES} ${catkin_LIBRARIES})

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h")

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})
