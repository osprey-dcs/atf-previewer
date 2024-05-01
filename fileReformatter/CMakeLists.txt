cmake_minimum_required(VERSION 3.25)
project(fileReformatter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        REQUIRED)

add_executable(fileReformatter main.cpp
        DataHeader.cpp
        DataHeader.h
        Cnst.h
        FileConverterFac.cpp
        FileConverterFac.h
        FileConverter.cpp
        FileConverter.h
        PsnFileConverter.cpp
        PsnFileConverter.h
        BinFileType.cpp
        BinFileType.h
        FileUtil.cpp
        FileUtil.h)
target_link_libraries(fileReformatter
        Qt5::Core
)

