
cmake_minimum_required(VERSION 3.25)
project(reloader)

add_executable(
    ${PROJECT_NAME} 
    src/main.c 
    #src/reloc.c
    #src/fftype.c    
)

# Make sure it links as a console app on Windows
if (WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE -mconsole)
endif()

# If you want to link against Windows system libraries, you can add them explicitly
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
