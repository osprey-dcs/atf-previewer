add_executable(FileReformatter2 main.cpp
        BinFileType.cpp
        DataHeader.cpp
        FileConverter.cpp
        FileConverter.h
        FileUtil.cpp
        PsnFileConverter.cpp
        PsnFileConverter.h
        FileConverterFac.cpp
        DataHeaderFac.cpp
        DataHeaderFac.h
        DspErr.cpp
        DspErr.h
        ErrHndlr.cpp
        ErrHndlr.h)
target_link_libraries(FileReformatter2
        Qt5::Core
)
install(
        TARGETS FileReformatter2
        RUNTIME DESTINATION bin
)
