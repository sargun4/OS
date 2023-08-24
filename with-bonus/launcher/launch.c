#include "../loader/loader.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: %s <ELF Executable>\n", argv[0]);
    exit(1);
  }

  // 1. Carry out necessary checks on the input ELF file

  const char *elfFileName = argv[1];

  // Check if the file exists

  if (access(elfFileName, F_OK) == -1)
  {
    printf("Error: File '%s' does not exist.\n", elfFileName);
    exit(1);
  }

  // Check if the file is readable

  if (access(elfFileName, R_OK) == -1)
  {
    printf("Error: File '%s' is not readable.\n", elfFileName);
    exit(1);
  }

  // 2. Passing it to the loader for carrying out the loading/execution

  load_and_run_elf(argv);

  // 3. Invoke the cleanup routine inside the loader

  loader_cleanup();

  return 0;
}