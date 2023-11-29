#include <stdio.h>

#define PAGE_SIZE 256
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 65536
#define TLB_SIZE 16

int TLBHit = 0;
int TLBMiss = 0;
int pageFaultCount = 0;

const int pageOffsetMask = 0xFF;
const int pageNumberMask = 0xFF00;

// Page table has 256 entries.
int pageTable[PAGE_TABLE_SIZE];
int physicalMemory[PHYSICAL_MEMORY_SIZE];
int frameNumber = 0;

typedef struct{
    int page;
    int frame;
}TLB;

TLB tlb[TLB_SIZE];
int tlbHead = 0;
int tlbCount = 0;

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

void initTables() {
    for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
        pageTable[i] = -1;
    }

    for(int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }
}

int getFrameTLB(int pageNumber) {
    for(int i = 0; i < FRAME_SIZE; i++) {
        if(tlb[i].page == pageNumber) {
            return tlb[i].frame;
        }
    }
    return -1;
}

void addEntryTLB(int pageNumber, int frameNumber) {
    tlb[tlbHead].page = pageNumber;
    tlb[tlbHead].frame = frameNumber;

    // Move the head of the TLB (front of the queue) to the next index
    tlbHead = (tlbHead + 1) % TLB_SIZE;

    // If the TLB is not full, increment the count
    if (tlbCount < TLB_SIZE) {
        tlbCount++;
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
    int physicalAddress = 0;

    // Extract desired bits from logical address.
    int pageOffset = extractPageOffset(logicalAddress);
    int pageNumber = extractPageNumber(logicalAddress);

    int frame = getFrameTLB(pageNumber);

    if (frame != -1) { // TLB hit
        physicalAddress = frame * FRAME_SIZE + pageOffset;
        TLBHit++;
    }
    else if(pageTable[pageNumber] != -1) { // TLB miss, page table hit
        physicalAddress = pageTable[pageNumber] * FRAME_SIZE + pageOffset;
        TLBMiss++;
    } 
    else {
        handlePageFault(pageNumber, disk);
        pageFaultCount++;
        physicalAddress = frameNumber * FRAME_SIZE + pageOffset;
        pageTable[pageNumber] = frameNumber;

        addEntryTLB(pageNumber, frameNumber);
        printf("Added page: %d\n", pageNumber);

        frameNumber++;
    }
    return physicalAddress;
}

int main(int argc, char *argv[]) {
    FILE *addresses = fopen(argv[1], "r");
    FILE *disk = fopen("BACKING_STORE.bin", "rb");

    initTables();
    int logicalAddress;
    while(fscanf(addresses, "%d", &logicalAddress) != EOF) {
        printf("Logical address: %d Physical address: %d\n", logicalAddress, 
                                translateAddress(logicalAddress, disk));

    }

    printf("Page fault rate: %f TLB hit rate: %f\n", pageFaultCount/1000.0, TLBHit/1000.0);

    return 0;
}