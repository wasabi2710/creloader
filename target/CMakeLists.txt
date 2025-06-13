cmake_minimum_required(VERSION 3.25)
project(test)

## compile to shared object only

add_library(${PROJECT_NAME} SHARED
    src/main.c      
)

#add_executable(${PROJECT_NAME} src/main.c) 