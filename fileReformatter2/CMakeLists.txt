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
        ../common/BinFileType.cpp
        ../common/BinFileType.h
        ../common/DataHeader.cpp
        ../common/DataHeader.h
        FileConverter.cpp
        FileConverter.h
        FileUtil.cpp
        FileUtil.h
        PsnFileConverter.cpp
        PsnFileConverter.h
        FileConverterFac.cpp
        FileConverterFac.h
        ../common/DataHeaderFac.cpp
        ../common/DataHeaderFac.h
        ../common/DspErr.cpp
        ../common/DspErr.h
        ../common/ErrHndlr.cpp
        ../common/ErrHndlr.h)

include_directories(../common)

target_link_libraries(FileReformatter2
        Qt5::Core
)

install(
        TARGETS FileReformatter2
        RUNTIME DESTINATION bin
)
