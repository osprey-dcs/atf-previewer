cmake_minimum_required(VERSION 3.27)
project(viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Charts
        REQUIRED)

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
        DspErr.cpp
        DspErr.h
        ErrHndlr.cpp
        ErrHndlr.h
)

include_directories(/usr/include/c++/10
        /home/osprey/fftw/include)

target_link_libraries(viewer
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        /home/osprey/fftw/lib/libfftw3.so
)

