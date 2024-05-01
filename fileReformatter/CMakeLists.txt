
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

