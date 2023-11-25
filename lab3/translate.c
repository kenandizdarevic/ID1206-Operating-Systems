#include <stdio.h>

#define PAGE_SIZE 256
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256

const int pageOffsetMask = 0xFF;
const int pageNumberMask = 0xFF00;

// Page table has 256 entries.
int pageTable[PAGE_TABLE_SIZE];

struct TLB{
    int page;
    int frame;
};



/// @brief Extracts bits 0-7, which is the page offset.
/// @param int address 
/// @return int pageOffset
int extractPageOffset(int logicalAddress) {
    return logicalAddress & pageOffsetMask;
}

/// @brief Extracts bits 8-15, which is the page number.
/// @param int address 
/// @return int pageNumber
int extractPageNumber(int logicalAddress) {
    return (logicalAddress & pageNumberMask) >> 8;
}

void initPageTable() {
    for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
        pageTable[i] = -1;
    }
}

/// @brief Read page from disk, store it in available page frame in physical memory.
/// @param int logicalAddress 
/// @param FILE *disk 
void handlePageFault(int pageNumber, FILE *disk) {
    fseek(disk, pageNumber * PAGE_SIZE, SEEK_SET);
    fread(&pageTable[pageNumber * FRAME_SIZE], sizeof(char), PAGE_SIZE, disk);
}

/// @brief Translates logical address to physical address.
/// @brief First check TLB for page number, if miss check page table.
/// @brief Update TLB with FIFO replacement strategy.
/// @param int logicalAddress 
/// @param FILE *disk
/// @return 
int translateAddress(int logicalAddress, FILE *disk) {
    /*
    * 2. Consult the TLB.
    * 3. If TLB hit, obtain frame number from TLB.
    * 4. If TLB miss, consult page table.
    * 5. Frame number obtained or handle page fault.
    */

    // Extract desired bits from logical address.
    int pageOffset = extractPageOffset(logicalAddress);
    int pageNumber = extractPageNumber(logicalAddress);
    printf("Page number: %d", pageNumber);

    if(pageTable[pageNumber] == -1) {
        handlePageFault(pageNumber, disk);
    }


    // Consult TLB, implement after page table.

    // Consult page table
    return pageTable[pageNumber] * FRAME_SIZE + pageOffset;
}

int main(int argc, char *argv[]) {
    FILE *addresses = fopen(argv[1], "r");
    FILE *disk = fopen("BACKING_STORE.bin", "rb");

    initPageTable();
    int logicalAddress;
    while(fscanf(addresses, "%d", &logicalAddress) != EOF) {
        printf("Logical address: %d\n", logicalAddress);
        printf("Physical address: %d\n", translateAddress(logicalAddress, disk));
    }

    return 0;
}