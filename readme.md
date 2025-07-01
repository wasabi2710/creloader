## C Hot Reloading

My personal take on C Hot Reloading.

## Installation

Use the provided custom scripts or build the project with CMake as usual.

## Dependencies

- CMake >= 3.25  
- Standard system libraries

## Usage

1. Define your program logic using a custom entry point called `run_all`. Example:
    
    ```c
    void hello() {}

    void goodbye() {}

    // u must not use main() entry
    void run_all() {
        hello();
        goodbye();
    }
    ```

2. Place your source files in a dedicated source directory.

3. Run the reloader with:

    ```sh
    reloader <src_path>
    ```
4. Valla...yay

## Why?

I'm tired of repetitive rebuilds and restarts. #idontlikemanuallabor
