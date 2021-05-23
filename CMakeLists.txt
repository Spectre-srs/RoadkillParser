cmake_minimum_required(VERSION 3.10)
project(RoadkillParser)
add_executable(roadkill-parser src/Converter.cpp)
install(
    TARGETS roadkill-parser
    DESTINATION ./
)
