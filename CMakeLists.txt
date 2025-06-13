
cmake_minimum_required(VERSION 3.25)
project(reloader)

# commentsafsasf
# winds only
if (WIN32)
add_executable(
    ${PROJECT_NAME} 
    src/main.c 
    src/reloader.c
    src/watcher.c    
)
elseif(UNIX)
add_executable(
    ${PROJECT_NAME} 
    posix/main.c 
    posix/reloader.c
    posix/watcher.c 
    posix/processor.c   
)
endif()

if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE -mconsole)
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
    )
endif()
