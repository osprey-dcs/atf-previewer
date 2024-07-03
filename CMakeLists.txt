cmake_minimum_required(VERSION 3.22)
project(viewer)

find_package(PkgConfig)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS
        Core
        OPTIONAL_COMPONENTS
        Gui
        Widgets
        Charts
)

if(NOT Qt5Core_FOUND)
        message(SEND_ERROR "Missing required Qt5Core")
endif()

if(Qt5Gui_FOUND AND Qt5Widgets_FOUND AND Qt5Charts_FOUND)
message(STATUS "Build Viewer")

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
        )

include_directories(common)

pkg_check_modules(FFTW REQUIRED IMPORTED_TARGET fftw3)

target_link_libraries(viewer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        PkgConfig::FFTW
)

install(
        TARGETS viewer
        RUNTIME DESTINATION bin
)

else()
        message(STATUS "Skip Viewer")
endif()

add_subdirectory(fileReformatter2)
