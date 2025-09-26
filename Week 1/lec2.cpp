#include <iostream>
using namespace std;


// doublely linked list element
struct listelement {
    listelement *next;
    listelement *prev;
    char d;
};

listelement *head = 0; // invalid address

int main() {

    // ne is the address of the request sizeofelement
    listelement *newElement = (listelement*) malloc(sizeof(listelement)); 

    // set new element to head if head is null
    if(head == 0) {
        head=newElement;
        head ->prev = head -> next = 0; 
        // sets head's previous and next to 0 if the first element in the DLL;
    } else { // if element in head
        listelement *last; // empty list element

        for (last = head; last->next != 0; last = last -> next); 
        // initially, last element is the head
        // keeps changing last until last has no next
        // move last to the next element each time 
        // last points to the last element after the entire iteration

        last->next = newElement;
        newElement->next=0;
        newElement->prev=last;
    }
    return 0;
}

/// @brief A function for inserting an element in a doubley linked list
/// @param prev The previous element
/// @param i The new element
void insert(listelement * prev, listelement * i) {

    listelement *A = prev, *B = prev->next;
    A->next = i;
    if (B!= 0) B->prev = i; // can error if B is null
    i->next = B;
    i->prev = A;

}

// traversing through a list
// for(listelement*p=head, p!=0, p=p->n) 
// cout << p