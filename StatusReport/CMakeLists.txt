cmake_minimum_required(VERSION 3.28)
project(StatusReport)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        REQUIRED)

add_executable(StatusReport main.cpp
        ../common/StatusFileType.cpp
        ../common/StatusFileType.h
        ../common/DspErr.cpp
        ../common/DspErr.h
        ../common/ErrHndlr.cpp
        ../common/ErrHndlr.h
        ../common/StatusFileFac.cpp
        ../common/StatusFileFac.h
        ../common/StatusFileBase.cpp
        ../common/StatusFileBase.h
        ../common/PsnStatusFile.cpp
        ../common/PsnStatusFile.h
        StatusDisplay.cpp
        StatusDisplay.h)

include_directories(../common)
      
target_link_libraries(StatusReport
        Qt5::Core
)

