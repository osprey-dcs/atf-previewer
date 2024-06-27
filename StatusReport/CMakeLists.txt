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
        StatusFileType.cpp
        StatusFileType.h
#        FileUtil.cpp
#        FileUtil.h
        DspErr.cpp
        DspErr.h
        ErrHndlr.cpp
        ErrHndlr.h
        StatusFileFac.cpp
        StatusFileFac.h
        StatusFileBase.cpp
        StatusFileBase.h
        PsnStatusFile.cpp
        PsnStatusFile.h
        StatusDisplay.cpp
        StatusDisplay.h)
target_link_libraries(StatusReport
        Qt5::Core
)

