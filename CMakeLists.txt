cmake_minimum_required(VERSION 3.25)
project(reloader)

add_executable(
    ${PROJECT_NAME} 
    src/main.c 
    src/reloc.c
    src/fftype.c    
)