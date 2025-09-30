/* 
Write a program. It should read words from the keyboard. If you type “print” it should print all
words. However, each word needs to be stored in a linked list depending on their first letter.

That means every letter of the alphabet has its own linked list. These list heads are connected
as list elements, making a linked list of alphabetic letters. And, as already mentioned, each of
these letter-list-elements contain a linked list of the words.
You must use double linked list elements for all lists.
With “print” you print out the words as they are alphabetically. It doesn’t matter if the linked
list of each letter is not sorted. I.e. Anna, Alpha is acceptable in this order.
The program ends after printing, but it must delete every single list element (50%) of all points.

*/

#include <iostream>
#include <string>
using namespace std;

// this assumes all inputs will be capitalized

char alphabet[26] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z'
};


/// @brief  a list for containing words input by user
struct wordliste {
    char *content;
    wordliste *prev;
    wordliste *next;
};

/// @brief A double linked list for containing each char of alphabet
struct alphaliste {
    alphaliste *prev;
    alphaliste *next;
    char letter;
    wordliste *wordlist = NULL;
};

alphaliste *head=NULL;


void initAlphabet() {

    for (int c : alphabet){
        // new element
        alphaliste *ne  = (alphaliste*) malloc(sizeof(alphaliste));

        if(head == NULL) {
            head=ne;
            // sets head's previous and next to NULL if the first element in the DLL;
            head ->prev = head -> next = NULL; 
        } else { // if element in head, find the last element and add an element after
            alphaliste *last; // empty list element

            for (last = head; last->next != NULL; last = last -> next); 
            last->next = ne;
            ne->next=0;
            ne->prev=last;
        }
        ne->letter = c;
    }    
}

void freeMemory() {
    // free up memory here
    alphaliste *p = head;
    while (p) {
        alphaliste *p_next = p->next;  

        wordliste *q = p->wordlist;

        while (q) {
            wordliste *q_next = q->next;  
            free(q->content);  // special because made through dynamic malloc
            free(q);
            q = q_next;
        } 

        free(p);
        p = p_next;
    }
    // free(head);
    head=NULL;
}


void insertWord(char *input) { 
    char first = input[0]; // yes you can index a ptr
    // loop to find the correct alpha
    alphaliste *p = head;
    while (p && p->letter != first) {
        // cout << p << '\n'; 
        p = p->next;
    }
    
    // allocate enough for new list element
    int wordlen = strlen(input);
    struct wordliste *node = (wordliste *)malloc(sizeof(struct wordliste));
    node->content = (char *) malloc(wordlen + 1);  
    strcpy(node->content, input);
    node->prev = node->next = NULL;

    // insert new wordliste in correc spot
    if (p->wordlist == NULL) {
        p->wordlist = node;
    } else {
        wordliste *q = p->wordlist;       
        while (q->next != NULL) q = q->next;
        q->next = node;
        node->prev = q;
    }

}

// print all
void printAllLetters() {
    for (alphaliste *p = head; p != NULL; p = p->next) {
        if (p->wordlist != NULL) {
            printf("Letter %c:\n", p->letter);
            for (wordliste *q = p->wordlist; q != NULL; q = q->next) {
                printf("    %s\n", q->content);
            }
        }
    }
}

int main() {
    initAlphabet();
    cout << "Please continuously enter words to enter into the list. \nThe first letter must becapitalized. Type `print` to quit" << endl;

    char word[256]; // 256 byte limit
    
    // continually enter a word and word is not print
    while (cin >> word && std::strcmp(word, "print") != 0) {
        insertWord(word);
    }
    printAllLetters();
    freeMemory();
    return 0;
}

