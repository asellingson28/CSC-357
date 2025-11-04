#include <unistd.h>
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/unistd.h"
#include <iostream>
using namespace std;

typedef unsigned char BYTE;
const int pagesize = 4096;

struct chunkinfo;

class simulate_brk_ {
private:
    BYTE* PB = 0;
    BYTE* mem = 0;

public:
    simulate_brk_() {
        PB = mem = new BYTE[10000000]; // 10 MB
    }

    ~simulate_brk_() {
        delete[] mem;
    }

    void* sbrk(int s) {
        BYTE* currPB = PB;
        PB += s;
        return currPB;
    }

    int brk(void* a) {
        PB = (BYTE*)a;
        if (PB >= mem) return 1;
        return 0;
    }
};

simulate_brk_ simbrk; //global
// You need to have a structure chunkinfo as part of each memory chunk:

/**
 * @struct chunkinfo
 * @brief Describes a memory chunk in a custom allocator.
 *
 * This structure holds metadata for each memory chunk, including its size,
 * occupancy status, and links to neighboring chunks.
 */
struct chunkinfo
{
    int size; // need + chunkinfo
    int occ;  // free = 0, occupied = 1 could this not just be a boolean?
    chunkinfo *next, *prev;
}; // 24 byte struct

chunkinfo *startofheap = nullptr;

static void *currentbrk = NULL;

/**
 * @brief
 *
 * @param node
 * @return chunkinfo*
 */
chunkinfo *traverse_to_tail(chunkinfo *node)
{
    if (node == NULL)
        return NULL;
    while (node->next != NULL)
        node = node->next;
    return node;
}

void analyze()
{


    printf("\n--------------------------------------------------------------\n");
    if (!startofheap)
    {
        printf("no heap, program break on address: %x\n",simbrk.sbrk(0));
        return;
    }
    chunkinfo *ch = (chunkinfo *)startofheap;
    for (int no = 0; ch; ch = (chunkinfo *)ch->next, no++)
    {
        printf("%d | current addr: %x |", no, ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->occ);
        printf("next: %x | ", ch->next);
        printf("prev: %x", ch->prev);
        printf(" \n");
    }
    printf("program break on address: %x\n", simbrk.sbrk(0));
}

/**
 * @brief
 *
 * @param size
 * @return BYTE*
 */
BYTE *mymalloc(int size)
{
    if (size <= 0)
    {
        return NULL;
    }
    int netsize = sizeof(chunkinfo) + size;
    void *base;

    // You start with a heapsize of 0, be aware of that.
    // if there is no heap, create one chunk big enough for the requested size with brk/sbrk.
    if (startofheap == NULL)
    {                          // no memory has every been malloced
        base = simbrk.sbrk(pagesize); // starts on a new page ???
        startofheap = static_cast<chunkinfo *>(base);
        // first is a new chunkinfo and no memory has ever been made
        chunkinfo *first = static_cast<chunkinfo *>(base);
        first->size = pagesize;
        first->occ = 0;
        first->prev = NULL;
        first->next = NULL;
    }

    // now, traverse the chunk info as bytes to find the next spot with netsize
    chunkinfo *c = startofheap;
    while (c != NULL)
    {
        if (!c->occ && c->size >= netsize)
            break;
        c = c->next;
    }

    // if no chunk is free or fits, move the program break further: Create a new chunk (and link correctly).
    if (c == NULL)
    {
        int grow = netsize;
        if (grow < pagesize)
            grow = pagesize;
        base = simbrk.sbrk(grow);
        chunkinfo *final = traverse_to_tail(startofheap); // tail (where next == NULL)
        chunkinfo *newc = static_cast<chunkinfo *>(base);
        newc->size = grow;
        newc->occ = 0;
        newc->prev = final;
        newc->next = NULL;

        if (final)
            final->next = newc;
        c = newc; // allocate from newly added space
    }
    c->occ = 1;
    int remainder = c->size - netsize;
    if (remainder >= (int)sizeof(chunkinfo) + 8)
    {
        BYTE *cbytes = (BYTE *)c;
        chunkinfo *split = (chunkinfo *)(cbytes + netsize);

        split->size = remainder;
        split->occ = 0;
        split->prev = c;
        split->next = c->next;

        if (c->next)
            c->next->prev = split;
        c->next = split;

        c->size = netsize;
    }
    return (BYTE *)c + sizeof(chunkinfo); // add chunk info to shift
}
/**
 * @brief frees a chunk of memory from a given addr 
 * 
 * @param addr a memory address excluding chunk info
 */
void myfree(BYTE *addr)
{


    chunkinfo *c = (chunkinfo*)(addr - sizeof(chunkinfo));

    c->occ = 0;

        if (c->next && !c->next->occ) {
            c->size += c->next->size;
            c->next = c->next->next;
            if (c->next)
                c->next->prev = c;
        }

        // Step 3b: coalesce with previous if possible
        if (c->prev && !c->prev->occ) {
            c->prev->size += c->size;
            c->prev->next = c->next;
            if (c->next)
                c->next->prev = c->prev;
            c = c->prev; // move back to start of merged block
        }
        if (c->next == NULL) {
            simbrk.sbrk(-(c->size));
        if (c->prev)
            c->prev->next = NULL;
        else
            startofheap = NULL; 
    }

// If the resulting merged chunk is at the end of the heap, move the program break (brk) backward to release memory to the OS.
    // should combine two malloc sections of consecutive free memory into one
    //  If the chunk (or the merged chunk) is the last one, you can remove the chunk completely and
    // move the program break back for the size of the chunk.
}

int main()
{
    currentbrk=simbrk.sbrk(0);
    BYTE *a[100];
    analyze(); // 50% points
    for (int i = 0; i < 100; i++)
        a[i] = mymalloc(1000);
    for (int i = 0; i < 90; i++)
        myfree(a[i]);
    analyze(); // 50% of points if this is correct
    myfree(a[95]);
    a[95] = mymalloc(1000);
    analyze(); // 25% points, this new chunk should fill the smaller free one
    //(best fit)
    for (int i = 90; i < 100; i++)
        myfree(a[i]);
    analyze();
    return 0;
}
