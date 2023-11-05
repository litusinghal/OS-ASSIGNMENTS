#include "loader.h"
#include<signal.h>
#include <ucontext.h>

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;

int fd;
int page_faults=0;
int page_allocation=0;
int fragmentation=0;

uintptr_t page_size = 4096;//4KB as mentioned in Assignment

void allocate_memory(Elf32_Phdr *segment, uintptr_t fault_address);

void signal_handler(int s ,siginfo_t *si, void *ptr){
    uintptr_t fault_address = (uintptr_t)si->si_addr;//Extracting the address of pagefault
    printf("\n");
    printf("Page fault at address : %p\n",(void*)fault_address);
    page_faults++;

    Elf32_Phdr *segment = NULL;
    for(int i =0;i<ehdr->e_phnum;i++){
        if(phdr[i].p_type == PT_LOAD) {
            if(fault_address>= phdr[i].p_vaddr && fault_address < (phdr[i].p_vaddr + phdr[i].p_memsz)) {
                segment = &phdr[i]; //setting segment pointer to program header
                break;
            }
        }
    }
    if(segment ==NULL){
        printf("Not loadable Segment\n");
        exit(0);
    }
    else{
        allocate_memory(segment,fault_address);
    }

}

void allocate_memory(Elf32_Phdr *segment, uintptr_t fault_address){  
    size_t required_size=4096;
    int i =1;
    while(required_size<segment->p_memsz){
        required_size=required_size+4096;
        i++;
    }
    
    void* map_address =mmap((void *)fault_address ,required_size,PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS,0,0); 
    if(map_address ==MAP_FAILED){
        perror("Allocation Failed");
        exit(1);
    }

    page_allocation+=i;
    fragmentation+=required_size-segment->p_memsz;

    printf("Memory allocated at address %p\n",map_address);
    lseek(fd,segment->p_offset,SEEK_SET);//Reading and copying the content
    read(fd,map_address, 4096);
}

void load_and_run_elf(char **exe) {
    fd = open(exe[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    read(fd, ehdr,sizeof(Elf32_Ehdr));
    phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr) *ehdr->e_phnum);
    lseek(fd,ehdr->e_phoff,SEEK_SET);
    read(fd,phdr, sizeof(Elf32_Phdr) *ehdr->e_phnum);
 
    typedef int (*result_function)(void);
    result_function _start =(result_function)((uintptr_t)ehdr->e_entry); //set the entry point of elf_program
    int result = _start();

    double internalfrag = (double)fragmentation/1024.0;
    
    printf("\n");
    printf("Result is : %d \n",result);
    printf("Total Page Faults : %d \n",page_faults);
    printf("Total Page Allocations : %d \n",page_allocation);    
    printf("Total Internal Fragmentation in bytes: %d KB\n", fragmentation);
    printf("Total Internal Fragmentation in KB: %.5f KB\n", internalfrag);
    printf("\n");

}
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }

    //Signal handler for segmentation fault (Custom signal handler)
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigaction(SIGSEGV, &sa, NULL);
    load_and_run_elf(argv);

    return 0;
}
