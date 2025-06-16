## C Hot Reloading
My personal take on C Hot Reloading.

## Installation
Run the custom scripts or use typical cmake build.

## Dependencies
1. Cmake >= 3.25.
2. All default libraries.

## How to use
1. This reloader will rely on redefining your main entry. Use "run_all" symbol for main entry.
`
void hello() {
}

void goodbye() {
}

void run_all() {
    hello();
    goodbye();
}       
`
2. I recommend adding the source files to a source directory.
3. Run 
`
reloader <src_path>.
`

## Why?
I'm just getting sick of manual labor.