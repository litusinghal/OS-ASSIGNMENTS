#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;

/*
 * release memory and other cleanups
 */
void loader_cleanup()
{
    free(ehdr);
    free(phdr);
    close(fd);
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char **exe)
{
    fd = open(exe[1], O_RDONLY);

    // Allocate memory to store the binary content
    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    ssize_t ehdr_read = read(fd, ehdr, sizeof(Elf32_Ehdr));
    phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr) * ehdr->e_phnum);
    // Read the binary content into the allocated memory
    lseek(fd, ehdr->e_phoff, SEEK_SET);
    ssize_t phdr_read = read(fd, phdr, sizeof(Elf32_Phdr)* ehdr->e_phnum);

    // Calculate the address of the program header table

    // Find the entry point address
    // Iterate through the program header table
    int flag = 0;

    for (int i = 0; i < ehdr->e_phnum; i++)
    {
        if (flag == 1) break;

        if (phdr[i].p_type == PT_LOAD)
        {
            if (ehdr->e_entry >= phdr[i].p_vaddr && ehdr->e_entry < (phdr[i].p_vaddr + phdr[i].p_memsz))
            {
                void *virtual_mem = mmap((void *)phdr[i].p_vaddr, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_FIXED, fd, phdr[i].p_offset);
                // printf("Alloacted virtual memory\n");
                if (virtual_mem == MAP_FAILED) {
                    perror("Error allocating memory with mmap");
                    close(fd);
                    exit(1);
                }

                // Calculate the offset from segment start to entry point
                size_t entry_offset = (size_t)(ehdr->e_entry - phdr[i].p_vaddr);

                // Navigate to the entry point within the segment
                int (*navigated_entry_point)() = (int (*)())((char *)virtual_mem + entry_offset);

                // Perform any other actions you need here
                // ...
                flag = 1;
                navigated_entry_point();
                int start_result = navigated_entry_point();
                printf(" %d\n", start_result);
            }
        }
    }
    //  void* virtual_mem = mmap((void*)phdr[i].p_vaddr , phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE|MAP_FIXED, fd, phdr[i].p_offset);

    // Print the value returned from "_start"
    // navigated_entry_point();
    // navigated_entry_point();
    // int start_result= navigated_entry_point();
    // printf(" %d\n", start_result);
    // Clean up allocated memory
    // munmap(virtual_mem, phdr[i].p_memsz);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <ELF Executable> \n", argv[0]);
        exit(1);
    }
    // 1. carry out necessary checks on the input ELF file
    // 2. passing it to the loader for carrying out the loading/execution
    load_and_run_elf(argv);
    // 3. invoke the cleanup routine inside the loader
    loader_cleanup();
    return 0;
}
