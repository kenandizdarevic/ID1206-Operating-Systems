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
char physicalMemory[PHYSICAL_MEMORY_SIZE];

int frameNumber = 0;

typedef struct {
    int frame;
    int valid;
} PageTableEntry;

typedef struct{
    int page;
    int frame;
}TLB;

TLB tlb[TLB_SIZE];
PageTableEntry pgTable[PAGE_TABLE_SIZE];

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
        //pageTable[i] = -1;
        pgTable[i].frame = -1;
        pgTable[i].valid = 0;
    }

    for(int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }
}

int getFrameTLB(int pageNumber) {
    for(int i = 0; i < TLB_SIZE; i++) {
        if(tlb[i].page == pageNumber) {
            return tlb[i].frame;
        }
    }
    return -1;
}

void addEntryTLB(int pageNumber, int frameNumber) {
    tlb[tlbHead].page = pageNumber;
    tlb[tlbHead].frame = frameNumber;

    tlbHead = (tlbHead + 1) % TLB_SIZE;

    if (tlbCount < TLB_SIZE) {
        tlbCount++;
    }
}

/// @brief Read page from disk, store it in available page frame in physical memory.
/// @param int logicalAddress 
/// @param FILE *disk 
void handlePageFault(int pageNumber, FILE *disk) {
    fseek(disk, pageNumber * PAGE_SIZE, SEEK_SET);
    fread(&physicalMemory[frameNumber * FRAME_SIZE], sizeof(char), PAGE_SIZE, disk);
}

/// @brief Translates logical address to physical address.
/// @brief First check TLB for page number, if miss check page table.
/// @brief Update TLB with FIFO replacement strategy.
/// @param int logicalAddress 
/// @param FILE *disk
/// @return int physicalAddress
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
    else if(pgTable[pageNumber].valid == 1) { // TLB miss, page table hit
        physicalAddress = pgTable[pageNumber].frame * FRAME_SIZE + pageOffset;
        addEntryTLB(pageNumber, pgTable[pageNumber].frame);
        TLBMiss++;
    } 
    else {
        handlePageFault(pageNumber, disk);
        pageFaultCount++;
        physicalAddress = frameNumber * FRAME_SIZE + pageOffset;
        
        pgTable[pageNumber].frame = frameNumber;
        pgTable[pageNumber].valid = 1;

        addEntryTLB(pageNumber, frameNumber);

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
        int physicalAddress = translateAddress(logicalAddress, disk);
        printf("Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, 
                                physicalAddress, physicalMemory[physicalAddress]);
    }

    printf("Page fault rate: %.3f TLB hit rate: %.3f\n", pageFaultCount/1000.0, TLBHit/1000.0);

    return 0;
}