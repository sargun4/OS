// #define _GNU_SOURCE
// #include "loader.h"

// struct segment_data
// {
//     uint32_t type;
//     Elf32_Addr address;
//     size_t size;
//     off_t offset;
// };

// struct virtual_memory
// {
//     void *address;
//     size_t size;
//     size_t loaded_size;
//     int fully_loaded;
// };

// long pagesize = 4096;
// int fd;
// Elf32_Ehdr *ehdr = NULL;
// Elf32_Phdr *phdr = NULL;
// struct segment_data *segment_data = NULL;
// struct virtual_memory *allocated_memory = NULL;
// int allocated_memory_size = 0;

// int total_page_faults = 0;
// int total_page_allocations = 0;
// size_t total_internal_fragmentation = 0;

// // release memory and other cleanups
// void loader_cleanup()
// {
//     for (int i = 0; i < allocated_memory_size; i++)
//     {
//         munmap(allocated_memory[i].address, allocated_memory[i].size);
//     }

//     free(allocated_memory);
//     free(ehdr);
//     free(phdr);
// }

// void sigsegv_handler(int signum, siginfo_t *info, void *context)
// {
//     total_page_faults++;
//     int segment_index = 0;

//     for (int i = 0; i < ehdr->e_phnum; i++)
//     {
//         if (segment_data[i].type == PT_LOAD)
//         {
//             if ((info->si_addr >= (void *)segment_data[i].address) && (info->si_addr < (void *)(segment_data[i].address + segment_data[i].size)))
//             {
//                 segment_index = i;
//                 break;
//             }
//         }
//     }

//     size_t page_number = ((size_t)info->si_addr - segment_data[segment_index].address) / pagesize;
//     void *virtual_mem;

//     if (!allocated_memory[segment_index].fully_loaded)
//     {
//         // Allocate memory for the specific page
//         virtual_mem = mmap((void *)(segment_data[segment_index].address + page_number * pagesize), pagesize,
//                            PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

//         if (virtual_mem == MAP_FAILED)
//         {
//             perror("mmap");
//             exit(1);
//         }

//         allocated_memory[segment_index].address = virtual_mem;
//         allocated_memory[segment_index].size = pagesize;
//         allocated_memory[segment_index].fully_loaded = (allocated_memory[segment_index].loaded_size + pagesize >= segment_data[segment_index].size);
//         allocated_memory[segment_index].loaded_size += pagesize;
//         total_page_allocations++;
//     }

//     if (lseek(fd, segment_data[segment_index].offset + page_number * pagesize, SEEK_SET) == -1)
//     {
//         // Handle lseek error
//         perror("lseek");
//         exit(1);
//     }

//     // Read the segment content from the file into the allocated memory page
//     if (read(fd, allocated_memory[segment_index].address + page_number * pagesize, pagesize) == -1){
//         // Handle read error
//         perror("read");
//         exit(1);
//     }

//     if (allocated_memory[segment_index].fully_loaded)
//     {
//         total_internal_fragmentation += pagesize - (segment_data[segment_index].size % pagesize);
//     }
// }

// void load_and_run_elf(char **exe)
// {
//     fd = open(exe[1], O_RDONLY);
//     if (fd == -1)
//     {
//         perror("Error opening file");
//         exit(1);
//     }

//     ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));

//     int bytes_read = read(fd, ehdr, sizeof(Elf32_Ehdr));
//     if (bytes_read != sizeof(Elf32_Ehdr))
//     {
//         perror("Error reading ELF header");
//         close(fd);
//         exit(1);
//     }

//     // Check if the file has a valid ELF magic number

//     if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
//         ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
//         ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
//         ehdr->e_ident[EI_MAG3] != ELFMAG3)
//     {
//         printf("Error: File '%s' is not a valid ELF file.\n", exe[1]);
//         close(fd);
//         exit(1);
//     }

//     lseek(fd, ehdr->e_phoff, SEEK_SET);
//     phdr = (Elf32_Phdr *)malloc(ehdr->e_phnum * sizeof(Elf32_Phdr));
//     segment_data = (struct segment_data *)malloc(ehdr->e_phnum * sizeof(struct segment_data));
//     allocated_memory = (struct virtual_memory *)malloc(ehdr->e_phnum * sizeof(struct virtual_memory));

//     for (int i = 0; i < ehdr->e_phnum; i++)
//     {
//         read(fd, phdr, sizeof(Elf32_Phdr));

//         segment_data[i].type = phdr->p_type;
//         segment_data[i].address = phdr->p_paddr;
//         segment_data[i].size = phdr->p_memsz;
//         segment_data[i].offset = phdr->p_offset;
//         allocated_memory[i].fully_loaded = 0;
//     }

//     struct sigaction sa;
//     sa.sa_sigaction = sigsegv_handler;
//     sa.sa_flags = SA_SIGINFO;

//     if (sigaction(SIGSEGV, &sa, NULL) == -1)
//     {
//         perror("Error setting up SIGSEGV handler");
//         return;
//     }

//     int (*_start)() = (int (*)())ehdr->e_entry;
//     int result = _start();

//     printf("User _start return value = %d\n", result);
//     printf("Total number of page faults = %d\n", total_page_faults);
//     printf("Total number of page allocations = %d\n", total_page_allocations);
//     printf("Total amount of internal fragmentation = %f KB\n", (double)total_internal_fragmentation / 1000);

//     close(fd);
// }

// int main(int argc, char const *argv[])
// {
//     return 0;
// }

#define _GNU_SOURCE
#include "loader.h"

struct segment_data
{
  uint32_t type;
  Elf32_Addr address;
  size_t size;
  off_t offset;
};

struct virtual_memory
{
  void *address;
  size_t size;
};

long pagesize = 4096;
int fd;
Elf32_Ehdr *ehdr = NULL;
Elf32_Phdr *phdr = NULL;
struct segment_data *segment_data = NULL;
struct virtual_memory *allocated_memory = NULL;
int allocated_memory_size = 0;

int total_page_faults = 0;
int total_page_allocations = 0;
size_t total_internal_fragmentation = 0;

// release memory and other cleanups

void loader_cleanup()
{
  for (int i = 0; i < allocated_memory_size; i++)
  {
    munmap(allocated_memory[i].address, allocated_memory[i].size);
  }

  free(allocated_memory);
  free(ehdr);
  free(phdr);
}

void sigsegv_handler(int signum, siginfo_t *info, void *context)
{
  total_page_faults++;
  int segment_index = 0;
  long number_of_pages = 0;

  for (int i = 0; i < ehdr->e_phnum; i++)
  {
    if (segment_data[i].type == PT_LOAD)
    {
      if ((info->si_addr >= (void *)segment_data[i].address) && (info->si_addr < (void *)(segment_data[i].address + segment_data[i].size)))
      {
        segment_index = i;
        // break;
      }
    }
  }

  number_of_pages = (segment_data[segment_index].size + pagesize - 1) / pagesize;

  printf("handler error\n");
  printf("mem size: %ld\n", number_of_pages);
  printf("Address causing segmentation fault: %p\n", info->si_addr);

  void *virtual_mem = mmap((void *)(segment_data[segment_index].address), number_of_pages * pagesize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  if (virtual_mem == MAP_FAILED)
  {
    perror("mmap");
    exit(1);
  }

  if (lseek(fd, segment_data[segment_index].offset, SEEK_SET) == -1)
  {
    // Handle lseek error
    perror("lseek");
    exit(1);
  }

  // Read the segment content from the file into the allocated memory
  if (read(fd, virtual_mem, segment_data[segment_index].size) == -1)
  {
    // Handle read error
    perror("read");
    exit(1);
  }

  allocated_memory[allocated_memory_size].address = virtual_mem;
  allocated_memory[allocated_memory_size].size = number_of_pages * pagesize;
  allocated_memory_size++;
  total_page_allocations = total_page_allocations + number_of_pages;
  total_internal_fragmentation = (number_of_pages * pagesize) - segment_data[segment_index].size;

  return;
}

void load_and_run_elf(char **exe)
{
  fd = open(exe[1], O_RDONLY);
  if (fd == -1)
  {
    perror("Error opening file");
    exit(1);
  }

  ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));

  int bytes_read = read(fd, ehdr, sizeof(Elf32_Ehdr));
  if (bytes_read != sizeof(Elf32_Ehdr))
  {
    perror("Error reading ELF header");
    close(fd);
    exit(1);
  }

  // Check if the file has a valid ELF magic number

  if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
      ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
      ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
      ehdr->e_ident[EI_MAG3] != ELFMAG3)
  {
    printf("Error: File '%s' is not a valid ELF file.\n", exe[1]);
    close(fd);
    exit(1);
  }

  lseek(fd, ehdr->e_phoff, SEEK_SET);
  phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr));
  segment_data = (struct segment_data *)malloc(ehdr->e_phnum * sizeof(struct segment_data));
  allocated_memory = (struct virtual_memory *)malloc(ehdr->e_phnum * sizeof(struct virtual_memory));

  for (int i = 0; i < ehdr->e_phnum; i++)
  {
    read(fd, phdr, sizeof(Elf32_Phdr));

    segment_data[i].type = phdr->p_type;
    segment_data[i].address = phdr->p_paddr;
    segment_data[i].size = phdr->p_memsz;
    segment_data[i].offset = phdr->p_offset;
  }

  struct sigaction sa;
  sa.sa_sigaction = sigsegv_handler;
  sa.sa_flags = SA_SIGINFO;

  if (sigaction(SIGSEGV, &sa, NULL) == -1)
  {
    perror("Error setting up SIGSEGV handler");
    return;
  }

  int (*_start)() = (int (*)())ehdr->e_entry;
  int result = _start();

  printf("User _start return value = %d\n", result);
  printf("Total number of page faults = %d\n", total_page_faults);
  printf("Total number of page allocations = %d\n", total_page_allocations);
  printf("Total amount of internal fragmentation = %d KB\n", total_internal_fragmentation/1000);

  close(fd);
}

int main(int argc, char const *argv[])
{
  return 0;
}

