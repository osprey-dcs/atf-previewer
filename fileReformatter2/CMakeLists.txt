cmake_minimum_required(VERSION 3.28)
project(FileReformatter2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        REQUIRED)

add_executable(FileReformatter2 main.cpp
        BinFileType.cpp
        DataHeader.cpp
        FileConverter.cpp
        FileConverter.h
        FileUtil.cpp
        PsnFileConverter.cpp
        PsnFileConverter.h
        FileConverterFac.cpp)
target_link_libraries(FileReformatter2
        Qt5::Core
)

