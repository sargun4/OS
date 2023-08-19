// The task involves creating a loader in C that loads and executes an ELF 32-bit executable without using any pre-built libraries. The loader should be compiled into a shared library (lib_simpleloader.so) and should be used in conjunction with a helper program to load and execute the target executable.

// Here's a breakdown of the key concepts and steps involved:

// ELF Header and Program Headers:

// ELF files have a specific structure, including an ELF header (Elf32_Ehdr) and program headers (Elf32_Phdr) that describe segments in the file.
// The ELF header contains important information about the file, such as the entry point (the starting address of the program) and the offset to the program header table.
// The program headers describe the segments of the file, including loadable segments that are relevant to executing the program.
// Loader Implementation (loader.c):

// The loader.c file is responsible for implementing the loading and execution of the ELF executable.
// The loader reads the ELF file, iterates through its program headers, and loads the loadable segments into memory.
// It then navigates to the entry point of the program and executes it.
// The loader should be compiled into a shared library called lib_simpleloader.so.
// Loader Shared Library (lib_simpleloader.so):

// This shared library encapsulates the loader's functionality.
// It exports functions that the helper program can use to load and execute an ELF executable.
// Helper Program (launch.c):

// The launch.c file acts as a driver for the loader.
// It loads the shared library lib_simpleloader.so dynamically using dlopen.
// It retrieves function pointers to the loader's functions using dlsym.
// It calls the loader's functions to load and execute the target ELF executable.
// After execution, it also calls the cleanup function provided by the loader to release any resources.
// Compilation and Execution:

// The provided Makefiles and directory structure help in compiling the code and organizing the files.
// The ELF executable (fib.c) is compiled with specific flags (-m32, -no-pie, -nostdlib) to ensure compatibility with the loader.
// The helper program (launch.c) is compiled by dynamically linking it to lib_simpleloader.so.
// Result:

// When you run the helper program (launch), it loads the target ELF executable using the loader, executes it, and then cleans up resources.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>
// Each program header provides information about a segment, 
// such as its size, location in the file, and location in memory.
typedef struct {
    unsigned int p_type;
    unsigned int p_offset;
    unsigned int p_vaddr;
    unsigned int p_filesz;
    unsigned int p_memsz;
    unsigned int p_flags;
    unsigned int p_align;
} Elf32_Phdr; //program header

// The ELF header contains general information about the file format 
// and provides a roadmap for the rest of the file's organization.
typedef struct {
    unsigned char e_ident[EI_NIDENT];
    unsigned short e_type;
    unsigned short e_machine;
    unsigned int e_version;
    unsigned int e_entry;
    unsigned int e_phoff;
    unsigned short e_phnum;
    unsigned short e_shentsize;
    unsigned short e_shnum;
} Elf32_Ehdr; //ELF header

// This function load_and_execute is responsible for loading and executing the ELF file. It opens the file, 
// checks its size, and prepares to read its content.
void load_and_execute(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    struct stat file_info;
    if (fstat(fd, &file_info) != 0) {
        perror("Error getting file information");
        close(fd);
        exit(1);
    }
    // allocates memory to store the file content, reads the
    //  content from the file into memory, and checks if the read operation was successful
    char *file_contents = (char *)malloc(file_info.st_size);
    if (read(fd, file_contents, file_info.st_size) != file_info.st_size) {
        perror("Error reading file");
        close(fd);
        free(file_contents);
        exit(1);
    }
    // closes the file and sets up pointers to the ELF header and program headers
    //  within the loaded content.
    close(fd);
    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)file_contents;
    Elf32_Phdr *program_headers = (Elf32_Phdr *)(file_contents + elf_header->e_phoff);

    // This loop iterates through the program headers and maps the program segments into memory 
    // using the mmap system call. If the segment type is PT_LOAD, it allocates memory, sets up 
    // the necessary protection flags, and copies the segment content into memory.
    for (int i = 0; i < elf_header->e_phnum; i++) {
        if (program_headers[i].p_type == PT_LOAD) {
            void *segment_addr = mmap((void *)program_headers[i].p_vaddr,
                                      program_headers[i].p_memsz,
                                      PROT_READ | PROT_WRITE | PROT_EXEC,
                                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                                      -1, 0);
            if (segment_addr == MAP_FAILED) {
                perror("Error mapping segment");
                free(file_contents);
                exit(1);
            }
            memcpy(segment_addr, file_contents + program_headers[i].p_offset, program_headers[i].p_filesz);
        }
    }
    // It sets up a func pointer to the entry point of the ELF program and then executes the program by calling that function pointer.
    void (*entry_point)() = (void (*)())(elf_header->e_entry);
    entry_point();

    free(file_contents);
}

// when compiled and executed with an ELF executable as an argument, will load and execute the specified ELF executable using memory mapping
//  and the program headers defined in the ELF format.




// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/mman.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <string.h>
// #include <elf.h>

// void load_and_execute(const char *filename) {
//     int fd = open(filename, O_RDONLY);
//     if (fd == -1) {
//         perror("Error opening file");
//         exit(1);
//     }

//     struct stat file_info;
//     if (fstat(fd, &file_info) != 0) {
//         perror("Error getting file information");
//         close(fd);
//         exit(1);
//     }

//     // Read the entire file into memory
//     char *file_contents = (char *)malloc(file_info.st_size);
//     if (read(fd, file_contents, file_info.st_size) != file_info.st_size) {
//         perror("Error reading file");
//         close(fd);
//         free(file_contents);
//         exit(1);
//     }

//     // Close the file
//     close(fd);

//     // Parse ELF header to find the entry point
//     Elf32_Ehdr *elf_header = (Elf32_Ehdr *)file_contents;
//     if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0 ||
//         elf_header->e_ident[EI_CLASS] != ELFCLASS32 ||
//         elf_header->e_ident[EI_DATA] != ELFDATA2LSB) {
//         fprintf(stderr, "Unsupported ELF format\n");
//         free(file_contents);
//         exit(1);
//     }

//     Elf32_Phdr *program_headers = (Elf32_Phdr *)(file_contents + elf_header->e_phoff);

//     for (int i = 0; i < elf_header->e_phnum; i++) {
//         if (program_headers[i].p_type == PT_LOAD) {
//             // Load the segment into memory using mmap
//             void *segment_addr = mmap((void *)program_headers[i].p_vaddr,
//                                       program_headers[i].p_memsz,
//                                       PROT_READ | PROT_WRITE | PROT_EXEC,
//                                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
//                                       -1, 0);
//             if (segment_addr == MAP_FAILED) {
//                 perror("Error mapping segment");
//                 free(file_contents);
//                 exit(1);
//             }

//             // Copy segment content from file to memory
//             memcpy(segment_addr, file_contents + program_headers[i].p_offset, program_headers[i].p_filesz);
//         }
//     }

//     // Find and execute the entry point
//     void (*entry_point)() = (void (*)())(elf_header->e_entry);
//     entry_point();

//     // Cleanup
//     free(file_contents);
// }

// // a) Implement all the functionality of the loader inside a single loader.c file. It should directly be able to
// // accept the executable of fib.c as a command line parameter, and then should be able to load/run it.
// // b) Once you are done with the above implementation, implement error handlings at all required places.
// // c) Now you should implement the launch.c file as mentioned above in requirements.
// // d) Your final step should be to create Makefiles, directory structure, and shared library version of the
// // loader.
