# Caller defines
#  GIT_EXECUTABLE
#  INPUT_FILE
#  OUTPUT_FILE
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --always
    OUTPUT_VARIABLE GIT_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "GIT_REVISION=${GIT_REVISION}")
configure_file(${INPUT_FILE} ${OUTPUT_FILE}.tmp)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OUTPUT_FILE}.tmp
        ${OUTPUT_FILE}
)
