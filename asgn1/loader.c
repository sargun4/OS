#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <elf.h>

typedef struct {
    unsigned int p_type;
    unsigned int p_offset;
    unsigned int p_vaddr;
    unsigned int p_filesz;
    unsigned int p_memsz;
    unsigned int p_flags;
    unsigned int p_align;
} Elf32_Phdr;

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
} Elf32_Ehdr;

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

    char *file_contents = (char *)malloc(file_info.st_size);
    if (read(fd, file_contents, file_info.st_size) != file_info.st_size) {
        perror("Error reading file");
        close(fd);
        free(file_contents);
        exit(1);
    }

    close(fd);

    Elf32_Ehdr *elf_header = (Elf32_Ehdr *)file_contents;
    Elf32_Phdr *program_headers = (Elf32_Phdr *)(file_contents + elf_header->e_phoff);

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

    void (*entry_point)() = (void (*)())(elf_header->e_entry);
    entry_point();

    free(file_contents);
}

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
