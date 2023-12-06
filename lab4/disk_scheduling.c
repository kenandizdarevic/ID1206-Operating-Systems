#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CYLINDERS 5000
#define REQUESTS 1000

// Function to add an element to an array
int addElement(int **arr, int element, int size) {
    size++;
    *arr = (int*) realloc(*arr, size * sizeof(int));  
    (*arr)[size - 1] = element;
    return size;
}


// Comparator functions for qsort
int compareHighToLow(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

int compareLowToHigh(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// FCFS (First-Come, First-Served) Algorithm
int fcfs(int headPointer, int requests[]) {
    int headMovement = 0;
    for (int i = 0; i < REQUESTS; i++) {
        headMovement += abs(headPointer - requests[i]);
        headPointer = requests[i];
    }
    return headMovement;
}

// SSTF (Shortest Seek Time First) Algorithm
int sstf(int headPointer, int requests[]) {
    int headMovement = 0;
    int visited[REQUESTS] = {0};

    for (int i = 0; i < REQUESTS; i++) {
        int minDistance = MAX_CYLINDERS + 1;
        int nextRequest = -1;

        for (int j = 0; j < REQUESTS; j++) {
            if (!visited[j]) {
                int distance = abs(headPointer - requests[j]);
                if (distance < minDistance) {
                    minDistance = distance;
                    nextRequest = j;
                }
            }
        }

        visited[nextRequest] = 1;
        headMovement += minDistance;
        headPointer = requests[nextRequest];
    }

    return headMovement;
}

// SCAN Algorithm
int scan(int headPointer, int requests[], int direction) {
    int headMovement = 0;

    int sizeLeft = 0, sizeRight = 0;  
    int *leftArray = (int*) malloc(sizeof(int)); 
    int *rightArray = (int*) malloc(sizeof(int));

    if(direction == 1) {
        sizeRight = addElement(&rightArray, MAX_CYLINDERS - 1, sizeRight);
    } else if(direction == -1){
        sizeLeft = addElement(&leftArray, 0, sizeLeft);
    } else {
        printf("Invalid direction for SCAN function\n");
        return -1;
    }

    for(int i = 0; i < REQUESTS; i++) {
        if (requests[i] < headPointer) {
            sizeLeft = addElement(&leftArray, requests[i], sizeLeft);
        } else {
            sizeRight = addElement(&rightArray, requests[i], sizeRight);
        }
    }

    qsort(leftArray, sizeLeft, sizeof(int), compareHighToLow);
    qsort(rightArray, sizeRight, sizeof(int), compareLowToHigh);

    for(int i = 0; i < sizeLeft; i++) {
        headMovement += abs(headPointer - leftArray[i]);
        headPointer = leftArray[i];
    }
    for(int i = 0; i < sizeRight; i++) {
        headMovement += abs(headPointer - rightArray[i]);
        headPointer = rightArray[i];
    }
    
    free(leftArray);
    free(rightArray);
    return headMovement;
}

// C-SCAN Algorithm
int cscan(int headPointer, int requests[]) {
    int headMovement = 0;

    int sizeLeft = 0, sizeRight = 0;  
    int *leftArray = (int*) malloc(sizeof(int)); 
    int *rightArray = (int*) malloc(sizeof(int));

    sizeRight = addElement(&rightArray, MAX_CYLINDERS - 1, sizeRight);

    sizeLeft = addElement(&leftArray, 0, sizeLeft);

    // Divides the requests into two arrays, one for requests lower than the head pointer 
    // and one for requests higher than the head pointer
    for(int i = 0; i < REQUESTS; i++) {
        if (requests[i] < headPointer) {
            sizeLeft = addElement(&leftArray, requests[i], sizeLeft);
        } else {
            sizeRight = addElement(&rightArray, requests[i], sizeRight);
        }
    }

    qsort(leftArray, sizeLeft, sizeof(int), compareLowToHigh);
    qsort(rightArray, sizeRight, sizeof(int), compareLowToHigh);

    for(int i = 0; i < sizeRight; i++) {
        headMovement += abs(headPointer - rightArray[i]);
        headPointer = rightArray[i];
    }

    for(int i = 0; i < sizeLeft; i++) {
        headMovement += abs(headPointer - leftArray[i]);
        headPointer = leftArray[i];
    }
    
    free(leftArray);
    free(rightArray);
    return headMovement;
}

// LOOK Algorithm
int look(int headPointer, int requests[], int direction) {
    int headMovement = 0;

    int sizeLeft = 0, sizeRight = 0;  
    int *leftArray = (int*) malloc(sizeof(int)); 
    int *rightArray = (int*) malloc(sizeof(int));

    // Divides the requests into two arrays, one for requests lower than the head pointer 
    // and one for requests higher than the head pointer
    for(int i = 0; i < REQUESTS; i++) {
        if (requests[i] < headPointer) {
            sizeLeft = addElement(&leftArray, requests[i], sizeLeft);
        } else {
            sizeRight = addElement(&rightArray, requests[i], sizeRight);
        }
    }

    qsort(leftArray, sizeLeft, sizeof(int), compareHighToLow);
    qsort(rightArray, sizeRight, sizeof(int), compareLowToHigh);

    if(direction == 1) {
        for(int i = 0; i < sizeRight; i++) {
            headMovement += abs(headPointer - rightArray[i]);
            headPointer = rightArray[i];
        }
        direction = -1;
    } else if(direction == -1) {
        for(int i = 0; i < sizeLeft; i++) {
            headMovement += abs(headPointer - leftArray[i]);
            headPointer = leftArray[i];
        }
        direction = 1;
    }

        if(direction == 1) {
        for(int i = 0; i < sizeRight; i++) {
            headMovement += abs(headPointer - rightArray[i]);
            headPointer = rightArray[i];
        }
        direction = -1;
    } else if(direction == -1) {
        for(int i = 0; i < sizeLeft; i++) {
            headMovement += abs(headPointer - leftArray[i]);
            headPointer = leftArray[i];
        }
        direction = 1;
    }


    
    free(leftArray);
    free(rightArray);
    return headMovement;
}

// C-LOOK Algorithm
int clook(int headPointer, int requests[]) {
    int headMovement = 0;

    int sizeLeft = 0, sizeRight = 0;  
    int *leftArray = (int*) malloc(sizeof(int)); 
    int *rightArray = (int*) malloc(sizeof(int));

    // Divides the requests into two arrays, one for requests lower than the head pointer 
    // and one for requests higher than the head pointer
    for(int i = 0; i < REQUESTS; i++) {
        if (requests[i] < headPointer) {
            sizeLeft = addElement(&leftArray, requests[i], sizeLeft);
        } else {
            sizeRight = addElement(&rightArray, requests[i], sizeRight);
        }
    }

    qsort(leftArray, sizeLeft, sizeof(int), compareLowToHigh);
    qsort(rightArray, sizeRight, sizeof(int), compareLowToHigh);

    for(int i = 0; i < sizeRight; i++) {
        headMovement += abs(headPointer - rightArray[i]);
        headPointer = rightArray[i];
    }

    for(int i = 0; i < sizeLeft; i++) {
        headMovement += abs(headPointer - leftArray[i]);
        headPointer = leftArray[i];
    }
    
    free(leftArray);
    free(rightArray);
    return headMovement;
}

int main(int argc, char *argv[]) {

    int headPointer = atoi(argv[1]);

    //Generate random cylinder requests
    int requests[REQUESTS];
    srand(time(NULL));     // Seed random number generator
    for (int i = 0; i < REQUESTS; i++) {
        requests[i] = rand() % MAX_CYLINDERS;
    }

    // int requests[REQUESTS] = { 98, 183, 37, 122, 14, 124, 65, 67 };
    // int headPointer = 53;

    // FCFS
    int fcfsHeadMovement = fcfs(headPointer, requests);
    printf("FCFS Head Movement: %d\n", fcfsHeadMovement);

    // SSTF
    int sstfHeadMovement = sstf(headPointer, requests);
    printf("SSTF Head Movement: %d\n", sstfHeadMovement);

    // SCAN
    // 1 for moving right, -1 for moving left
    int scanHeadMovement = scan(headPointer, requests, -1);
    printf("SCAN Head Movement: %d\n", scanHeadMovement);

    // C-SCAN ***WRONG
    int cscanHeadMovement = cscan(headPointer, requests);
    printf("C-SCAN Head Movement: %d\n", cscanHeadMovement);

    // LOOK
    int lookHeadMovement = look(headPointer, requests, 1);
    printf("LOOK Head Movement: %d\n", lookHeadMovement);

    // C-LOOK
    int clookHeadMovement = clook(headPointer, requests);
    printf("C-LOOK Head Movement: %d\n", clookHeadMovement);

    return 0;
}
