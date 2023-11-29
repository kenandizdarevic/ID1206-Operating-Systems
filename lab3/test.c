#include <stdio.h>
#include <string.h>
#define FRAMES 256
#define SIZE 256

int memory[FRAMES * SIZE];
int takenFrames[256];

int pageTable[256];
FILE *fileBin;

int tlbHead = 0;
int tlb[16][2];

int tlbHits = 0;
int pageFaults = 0;

struct addressComponents
{
    int logicalAddress;
    int pageNumber;
    int offset;
    int physicalAddress;
};

// Loads a page from backup memory into the main memory
int loadPage(int pageNumber)
{
    signed char buffer[256];
    fseek(fileBin, (pageNumber * 256), SEEK_SET);
    for (int i = 0; i < 256; i++)
    {
        if (takenFrames[i] == 0)
        {
            // i is free Frame
            fread(buffer, 1, 256, fileBin);

            // Literally copying over the 256 bytes (one page) from the bin into the memory
            for (int j = 0; j < 256; j++)
            {
                memory[(i * 256) + j] = buffer[j];
            }

            // This frame is now taken
            takenFrames[i] = 1;

            return i;
        }
    }
}

int tlbLookup(struct addressComponents address)
{
    tlbHead %= 16;

    int len = (sizeof(tlb) / sizeof(int));
    for (int i = 0; i < len; i++)
    {
        int page = tlb[i][0];
        int frame = tlb[i][1];

        // hit
        if (page == address.pageNumber)
        {
            return tlb[i][1];
        }
    }

    // if we get here, it's a miss
    return -1;
}

int search(struct addressComponents address)
{
    int frame = tlbLookup(address);
    // TLB hit
    if (frame != -1)
    {
        tlbHits++;
        return frame;
    }
    // TLB miss
    else
    {
        // Page table hit
        if (pageTable[address.pageNumber] != -1)
        {
            frame = pageTable[address.pageNumber];
        }
        // PageFault -- The page is not found in the main memory and must search the second backup memory (BACKING_STORE.bin)
        else
        {
            pageFaults++;
            frame = loadPage(address.pageNumber);
            pageTable[address.pageNumber] = frame;
        }

        // update the TLB
        tlb[tlbHead][0] = address.pageNumber;
        tlb[tlbHead][1] = frame;

        tlbHead++;

        return frame;
    }
}

struct addressComponents parseAddress(int logicalAddress)
{
    struct addressComponents address;

    address.logicalAddress = logicalAddress;
    address.offset = logicalAddress & 0xff;
    address.pageNumber = (logicalAddress >> 8) & 0xff;

    return address;
}

int main()
{
    unsigned char buffer[100];
    struct addressComponents addressList[1000];

    int logicalAddress;
    FILE *addresses;
    addresses = fopen("addresses.txt", "r");
    fileBin = fopen("BACKING_STORE.bin", "rb");

    for (int i = 0; i < (sizeof(pageTable) / sizeof(int)); i++)
    {
        pageTable[i] = -1;
    }

    for (int i = 0; i < (sizeof(tlb) / sizeof(int)); i++)
    {
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }

    for (int i = 0; i < 1000; i++)
    {
        // get the virtual address
        fgets(buffer, 8, addresses);
        sscanf(buffer, "%d", &logicalAddress);

        // populate addressList with logical/physical address
        addressList[i] = parseAddress(logicalAddress);
        addressList[i].physicalAddress = (search(addressList[i]) * 256) + addressList[i].offset;

        // print out addresses
        printf("Virtual address: %d ", addressList[i].logicalAddress);
        printf("Physical address: %d ", addressList[i].physicalAddress);
        signed int value = memory[addressList[i].physicalAddress];
        printf("Value: %d\n", value); // Make print signed plz
    }

    printf("TLB Hit Rate: %d\n", tlbHits);
    printf("Page Fault Rate: %d\n", pageFaults);
    return 0;
}