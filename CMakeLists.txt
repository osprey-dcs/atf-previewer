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
        DataHeader.cpp
        DataHeader.h
        DataHeaderFac.cpp
        DataHeaderFac.h
        BinDataFac.cpp
        BinDataFac.h
        BinData.cpp
        BinData.h
        Combo.cpp
        Combo.h
        Cnst.h
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
        BinDataBase.cpp
        BinDataBase.h
        BinDataMFile.cpp
        BinDataMFile.h
        VDisk.cpp
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
)

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
