#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

/*
cis: cgarc283@calpoly.edu
arjan: aselling@calpoly.edu
jonathan: jgarc710@calpoly.edu


*/
//******************************************************************************
//
// TASK 1: write the new_element function
// TASK 2: write the swap_element function
// TASK 3: write the remove_element function
//
//******************************************************************************
typedef struct linkedlist_element
{
int i;
linkedlist_element* next, *prev;
} linkedlist_element;

void new_element(int value, linkedlist_element ** head) //insert correct datatype for passing the head here.    
{
    if (*head == NULL)
    {
       linkedlist_element * nextElement = (linkedlist_element *)malloc(sizeof(linkedlist_element));
        nextElement->i = value;
        nextElement->prev = NULL;
        nextElement->next = NULL;
        *head = nextElement;
    }
    else
    {
        linkedlist_element * last = (linkedlist_element *)malloc(sizeof(linkedlist_element));
        for(last = *head; last->next != NULL; last = last->next);
        linkedlist_element * nextElement = (linkedlist_element *)malloc(sizeof(linkedlist_element));
        nextElement->i = value;
        nextElement->prev = last;
        nextElement->next = NULL;
        last->next = nextElement;
        //printf("Line to stop at\n");
    }
}

void bubble_sort( linkedlist_element **head ) //insert correct datatype for passing the head here.
{
    int counter = 0;

    for(linkedlist_element *last = (*head); last!= NULL; last = last->next){
    counter++;
    }

    linkedlist_element *next_num =  (*head)->next;
    for (int current = 0; current < counter-1; current++){
        for(int next=0; next < counter-current-1; next++){
            if((*head)->i > next_num->i){ //arr[next] > arr[next+1]){
                //swap
                linkedlist_element *temp = (linkedlist_element *) (sizeof(linkedlist_element));
                temp = (*head);
                temp->prev = (*head)->prev;
                temp->next = (*head)->next;
                temp->i = (*head)->i;
                
                (*head)->i = next_num -> i;

                next_num = temp;
                next_num->i = temp->i;
                next_num->prev = temp-> prev;
                next_num-> next = temp -> next;


                (*head)-> prev = next_num-> next;
                (*head) = (*head) -> next;
                
                
                // int temp = (*head)->i;  //arr[next];
                // (*head)->i = next_num->i;   //arr[next] = arr[next+1];
                // next_num->i = temp; //arr[next+1] = temp;
            }
            if((*head) == NULL){
                break;
            }
        
        }
    
    }
    

}
void remove_element(linkedlist_element **head)//pass the head here.
{

linkedlist_element *temp = (*head)->next;
free((*head));
(*head) = temp;


}
//besides inserting the correct datatypes into functions, dont change anything here.
int main(int argc, char* argv[])
{
linkedlist_element* head = NULL; //this time not global, but local.
new_element(5, &head);//pass the head here
new_element(2, &head);
new_element(9, &head);
new_element(4, &head);
new_element(0, &head);


// bubble_sort( &head );//pass the head here too. small to big!

// for (linkedlist_element* p = head; p; p = p->next)
// cout << p->i << endl; //should print 0,2,4,5,9

remove_element(&head);//pass the head here.
remove_element(&head);//pass the head here.
for (linkedlist_element* p = head; p; p = p->next)
cout << p->i << endl; //should print 4,5,9
//delete the whole list
return 0;
}
