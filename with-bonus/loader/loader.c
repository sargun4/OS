#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
void *virtual_mem;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup()
{

  munmap(virtual_mem, phdr->p_memsz);
  free(ehdr);
  free(phdr);
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char **exe)
{
  fd = open(exe[1], O_RDONLY);

  // 1. Load entire binary content into the memory from the ELF file.

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

  // 2. Iterate through the PHDR table and find the section of PT_LOAD
  //    type that contains the address of the entrypoint method in fib.c

  for (int i = 0; i < ehdr->e_phnum; i++)
  {
    read(fd, phdr, sizeof(Elf32_Phdr));

    if ((phdr->p_type == PT_LOAD) && ((ehdr->e_entry > phdr->p_vaddr) && (ehdr->e_entry < (phdr->p_vaddr + phdr->p_memsz))))
    {
      break;
    }
  }

  // 3. Allocate memory of the size "p_memsz" using mmap function
  //    and then copy the segment content

  virtual_mem = mmap(NULL, phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, phdr->p_offset);

  int *entry_point = (virtual_mem + (ehdr->e_entry - phdr->p_vaddr));

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // 6. Call the "_start" method and print the value returned from the "_start"

  int (*_start)() = (int (*)())entry_point;
  int result = _start();

  printf("User _start return value = %d\n", result);

  close(fd);
}

int main(int argc, char const *argv[])
{

  return 0;
}
