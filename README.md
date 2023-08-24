# Repo for OS asgns
# Asgn-1

## Creating a program that can load and execute 32-bit executable files (ELF format) without using any pre-built libraries for handling ELF files. The program we create will be split into two parts: a "loader" and a "launcher."

## Loader (loader.c): It reads an ELF executable file, loads it into memory, and executes it. step by step explanation:
a. Opens the ELF executable file we provide.

b. Reads the content of the file into memory.

c. Parses the ELF header to understand the file's structure.

d. Iterates through the program headers, which describe the memory segments needed by the executable.

e. Loads these memory segments into the program's memory space using mmap and copies the content from the file.

f. Finds the entry point of the loaded program and starts executing it.

## Launcher (launch.c): in this part, we run the loader and load different executable files. It does the following:

a. Accepts the name of an ELF executable file as a command-line argument.

b. Dynamically loads the shared library (lib_simpleloader.so) that contains the loader's code.

c. Gets the function from the shared library that loads and executes the ELF file.

d. Executes the loader function, which in turn loads and runs the provided ELF executable file.

e. Cleans up resources after the execution is complete.
