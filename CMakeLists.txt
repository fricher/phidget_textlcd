cmake_minimum_required(VERSION 2.8.3)
project(phidget_textlcd)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package(
 CATKIN_DEPENDS roscpp std_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ~/libphidget-2.1.8.20140319
)

add_executable(phidget_textlcd src/phidget_textlcd.cpp)
target_link_libraries(phidget_textlcd ${catkin_LIBRARIES} phidget21)
