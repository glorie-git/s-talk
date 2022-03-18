//  list.c
//  assingment-1
//  Created by _anotherone on 2021-05-19.

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

static Node nodePool[LIST_MAX_NUM_NODES];
static List headPool[LIST_MAX_NUM_HEADS];
static Node heads[LIST_MAX_NUM_HEADS];

static int numNodesAvailable = LIST_MAX_NUM_NODES;
static int numHeadsAvailable = LIST_MAX_NUM_HEADS;

static int headCount = 0;
static int nodeCount = 0;
static int nextAvailableHeadIndex = 0;

static int initialized = 0;

static bool nodesAvailable(void) {
    return numNodesAvailable;
}

static bool headsAvailable(void) {
    return numHeadsAvailable;
}

static void intialize ( void ) { // intialize head pool
    for ( int i = 0; i < LIST_MAX_NUM_HEADS; i++) {
        headPool[i].head = &heads[i];
        headPool[i].headVal = AVAILABLE;
    }
}

List *List_create(void) {
    // Check if headPool has been initialzed
    if ( initialized != true ) {
        intialize();
        initialized = 1;
    }
    
    // Initialize next available head
    if ( nodesAvailable() && headsAvailable() ) {
        int index = nextAvailableHeadIndex;
        headPool[index].head = headPool[index].current = &heads[index];
        headPool[index].headVal = index;
        headPool[index].numItems = 0;
        headPool[index].feet = headPool[index].head;
        
        // After initialization of list is done
        headCount++;
        numHeadsAvailable--;
        nextAvailableHeadIndex = headCount;
        return &headPool[index];
    } else {
        return NULL;
    }
}

int List_count(List *pList) {
    return pList->numItems;
}

void *List_first( List *pList ) {
    if (pList->numItems > 0) {
        pList->current = pList->head->next;
        return pList->current->item;
    } else {
        return NULL;
    }
}

void *List_last(List *pList) {
    if (pList->numItems > 0) {
        pList->current = pList->feet;
        return pList->current->item;
    } else {
        return NULL;
    }
}

void *List_next(List *pList) {
    if ( pList->numItems > 0 ) {
        pList->current = pList->current->next;
        if ( pList->current == NULL ) {
            pList->current = pList->feet;
            return NULL;
        } else {
            return pList->current->item;
        }
    } else {
        return NULL;
    }
}

void *List_prev(List *pList) {
    if ( pList->numItems > 0 ) {
        pList->current = pList->current->prev;
        if ( pList->current == NULL ) {
            pList->current = pList->head;
            return NULL;
        } else {
            return pList->current->item;
        }
    } else {
        return NULL;
    }
}

void *List_curr(List *pList) {
    if ( pList->current == pList->head ) {
        printf("Current item is beyond the start of the pList: ");
        return pList->current->item;
    } else if ( pList->current == NULL ) {
        printf("Current item is beyond the end of the pList: ");
        return NULL;
    } else {
        return pList->current->item;
    }
}

int List_add(List *pList, void *pItem) {
    if (nodesAvailable() && headsAvailable()) {
        nodePool[nodeCount].item = pItem;
        
        nodePool[nodeCount].prev = pList->current;
        if ( pList->current->next != NULL ) { // if not beyond end of list
            pList->current->next->prev = &nodePool[nodeCount];
            nodePool[nodeCount].next = pList->current->next;
        } else { // current is at the end of the list
            pList->feet = &nodePool[nodeCount];
        }
        // place item after current item
        pList->current->next = &nodePool[nodeCount];
        pList->current = &nodePool[nodeCount];
        
        pList->numItems++;
        numNodesAvailable--;
        nodeCount++;
        return LIST_SUCCESS;
    } else {
        return LIST_FAIL;
    }
}

int List_insert(List *pList, void *pItem) {
    if (nodesAvailable() && headsAvailable()) {
        nodePool[nodeCount].item = pItem;
        if ( pList->current == pList->head ) {
            // add to the start of the pList
            pList->head->next = &nodePool[nodeCount];
            pList->current = &nodePool[nodeCount];
        } else {
            nodePool[nodeCount].next = pList->current;
            nodePool[nodeCount].prev = pList->current->prev;
            pList->current->prev->next = &nodePool[nodeCount];
            pList->current->prev = &nodePool[nodeCount];
            
            pList->current = &nodePool[nodeCount];
        }
        pList->numItems++;
        numNodesAvailable--;
        nodeCount++;
        return LIST_SUCCESS;
    } else {
        return LIST_FAIL;
    }
}

int List_append(List *pList, void *pItem) {
    if (nodesAvailable() && headsAvailable()) { // make sure there is a head and a node available
        nodePool[nodeCount].item = pItem;
        if ( pList->feet != pList->head ) { // item is not the first item in list
            // add to the end of list
            nodePool[nodeCount].prev = pList->feet;
            pList->feet->next = &nodePool[nodeCount];
            pList->feet = &nodePool[nodeCount];
        } else { // item is the first item in list
            pList->feet = &nodePool[nodeCount];
            pList->head->next = &nodePool[nodeCount];
        }
        pList->current = &nodePool[nodeCount];
        pList->numItems++;
        numNodesAvailable--;
        nodeCount++;
        return LIST_SUCCESS;
    } else {
        return LIST_FAIL;
    }
}

int List_prepend(List *pList, void *pItem) {
    if (nodesAvailable() && headsAvailable()) { // make sure there is a head and a node available
        nodePool[nodeCount].item = pItem;
        if (pList->head->next != NULL) { // not first item added to list
            pList->head->next->prev = &nodePool[nodeCount];
            nodePool[nodeCount].next = pList->head->next;
            pList->head->next = &nodePool[nodeCount];
        } else {
            pList->head->next = &nodePool[nodeCount];
            pList->feet = &nodePool[nodeCount];
            pList->feet->prev = nodePool[nodeCount].prev;
        }
        pList->current = pList->head->next;
        pList->numItems++;
        numNodesAvailable--;
        nodeCount++;
        return LIST_SUCCESS;
    } else {
        return LIST_FAIL;
    }
}

void *List_remove(List *pList) {
    if ( pList->numItems > 1 ) {
        if ( pList->current->prev != NULL ) {
            pList->current->prev->next = pList->current->next;
            if ( pList->current->next != NULL ) {
                pList->current->next->prev = pList->current->prev;
            } else { // is the last item in the list
                pList->feet = pList->current->prev;
            }
        } else {
            pList->head->next = pList->current->next;
            pList->current->next->prev = pList->head->next;
        }
        
        Node *pTmp = pList->current->next;
        void *pItem = pList->current->item;

        pList->current->item = NULL;
        pList->current->prev = NULL;
        pList->current->next = NULL;

        pList->current = pTmp;
        
        numNodesAvailable++;
        pList->numItems--;
        return pItem;
    } else {
        return NULL;
    }
}

void *List_trim(List *pList) {
    if ( pList->numItems > 0 ) {
        pList->current = pList->feet;
        Node *pItem = List_remove(pList);
        return pItem;
    } else {
        return NULL;
    }
}

void List_concat(List *pList1, List *pList2) {
    if ( pList1->headVal != AVAILABLE && pList1->numItems > 0 ) {
        if ( pList2->numItems > 0 ) {
            pList1->numItems = pList1->numItems + pList2->numItems;
            pList1->feet->next = pList2->head->next;
            pList2->head->next->prev = pList1->feet;
            pList1->feet = pList2->feet;
        }
        
        // Here we make the pList2's head's index value the next created
        // list's head's index value. This helps keep available heads
        // at the end of the list after the last created list most of the time
        nextAvailableHeadIndex = pList2->headVal;
    
        pList2->headVal = AVAILABLE;
        pList2->head->next = NULL;
        pList2->current = pList2->feet = NULL;
        pList2->numItems = 0;
        headCount--;
    } else {
        printf("error: pList1 doesn't exist.\n");
    }
}
void List_free(List *pList, FREE_FN pItemFreeFn) {
    FREE_FN free_FN = pItemFreeFn;
    void *pItem = List_first(pList);
    void *pItemTmp = NULL;
    
    for(; pItem != NULL;) {
        pItemTmp = List_next(pList);
        free_FN(pItem);
        pItem = pItemTmp;
    }
}

void *List_search(List *pList, COMPARATOR_FN pComparator, void *pComparisonArg) {
    COMPARATOR_FN comp_FN = pComparator;
    void *pItem = NULL;
    void *pItemTmp = NULL; // = pList->current->item;
    
    
    // If the current pointer is before the start of the pList
    // then start of from beginning of list
    // else start at current pointer
    if (pList->current == pList->head) {
        pItemTmp = pList->head->next;
    } else {
        pItemTmp = pList->current->item;
    }
    for(; pItemTmp != NULL;) {
        if ( !comp_FN(pItem, pComparisonArg) ) {
            pItemTmp = List_next(pList);
        } else {
            pItem = pItemTmp;
            pItemTmp = NULL;
        }
    }
    return pItem;
}
