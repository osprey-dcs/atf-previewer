cmake_minimum_required(VERSION 3.22)
project(viewer)

find_package(PkgConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS
        Core
        REQUIRED
        OPTIONAL_COMPONENTS
        Gui
        Widgets
        Charts
)

if(Qt5Gui_FOUND AND Qt5Widgets_FOUND AND Qt5Charts_FOUND)
message(STATUS "Build Viewer")

find_package(Git)

add_executable(viewer main.cpp
        ViewerMainWin.cpp
        ViewerMainWin.h
        UserPrefs.cpp
        UserPrefs.h
        UserPrefsFac.cpp
        UserPrefsFac.h
        ViewerCtlr.cpp
        ViewerCtlr.h
        ViewerGraphArea.cpp
        ViewerGraphArea.h
        ViewerGraphBase.cpp
        ViewerGraphBase.h
        ViewerGraph.cpp
        ViewerGraph.h
        ViewerGraphLogY.cpp
        ViewerGraphLogY.h
        common/DataHeader.cpp
        common/DataHeader.h
        common/DataHeaderFac.cpp
        common/DataHeaderFac.h
        common/BinDataFac.cpp
        common/BinDataFac.h
        common/BinData.cpp
        common/BinData.h
        Combo.cpp
        Combo.h
        Cnst.h
        LineSeriesBuilderBase.cpp
        LineSeriesBuilderBase.h
        LineSeriesBuilderMinMax.cpp
        LineSeriesBuilderMinMax.h
        LineSeriesBuilderSimple.cpp
        LineSeriesBuilderSimple.h
        ViewerChart.cpp
        ViewerChart.h
        LineSeriesBuilderMinMaxFillUnder.cpp
        LineSeriesBuilderMinMaxFillUnder.h
        LineSeriesBuilderSimpleFillUnder.cpp
        ViewerCtlrPushButton.cpp
        ViewerCtlrPushButton.h
        ViewStack.cpp
        ViewStack.h
        ViewerGraphAreaBase.cpp
        ViewerGraphAreaFft.cpp
        ViewerGraphAreaFft.h
        ViewerSlider.cpp
        ViewerSlider.h
        HelpAboutDialog.cpp
        HelpAboutDialog.h
        ShortcutDialog.cpp
        ShortcutDialog.h
        FileUtil.cpp
        common/BinDataBase.cpp
        common/BinDataBase.h
        common/BinDataMFile.cpp
        common/BinDataMFile.h
        common/VDisk.cpp
        common/VDisk.h
        ExportDialog.cpp
        ExportDialog.h
        ChanSelector.cpp
        ChanSelector.h
        Uff58bExport.cpp
        Uff58bExport.h
        CsvExport.cpp
        CsvExport.h
        RoiSelector.cpp
        RoiSelector.h
        Tunable.cpp
        Tunable.h
        common/DspErr.cpp
        common/DspErr.h
        common/ErrHndlr.cpp
        common/ErrHndlr.h
        DataMediator.cpp
        DataMediator.h
        DataMediatorFac.cpp
        DataMediatorFac.h
        ${CMAKE_CURRENT_BINARY_DIR}/git_revision.h
        )

add_custom_target(GitDescribeH
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/git_revision.h.in
                ${CMAKE_CURRENT_SOURCE_DIR}/git_describe.cmake
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/git_revision.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND}
                -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
                -D INPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/git_revision.h.in
                -D OUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/git_revision.h
                -P ${CMAKE_CURRENT_SOURCE_DIR}/git_describe.cmake
)


include_directories(common)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3)

target_link_libraries(viewer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        PkgConfig::FFTW
)

else()
        message(STATUS "Skip Viewer")
endif()

#add_subdirectory(fileReformatter)
add_subdirectory(fileReformatter2)

install(
        TARGETS viewer
        RUNTIME DESTINATION bin
)
