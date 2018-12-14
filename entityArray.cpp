#include "entityArray.h"

EntityList :: EntityList() : first(NULL), last(NULL), curr(NULL), size(0), removedFirst(false) {

}

void EntityList :: pushBack(Entity &E) {
    node *newNode = new node(E);

    size++;

    if (!first) {
        first = last = newNode;
    }

    else {
        newNode->prev = last;
        last->next = newNode;

        last = newNode;
    }
}

Entity& EntityList :: peekCurrent() {
    return curr->E;
}

Entity& EntityList :: removeCurrent() {
    node *prev = curr->prev,
        *next = curr->next,
        *toDelete = curr;


    size--;

    Entity &retRef = curr->E;

    //both next and prev are NULL
     if (curr == first && first == last) {
        curr = NULL;
    }
    //prev is NULL. update first
    else if (curr == first) {
        //if I removed the first and then use setNext don't want to skip
        //the second element so we don't move in the list
        removedFirst = true;
        next->prev = NULL;

        curr = first = next;
    }

    //next is NULL. update last
    else if (curr == last) {
        prev->next = NULL;

        curr = last = prev;
    }

    //we are inside the list
    else {
        prev->next = next;
        next->prev = prev;

        curr = prev;
    }

    delete toDelete;

    //delete (&retRef);

    return retRef;
}

void EntityList :: setOnFirst() {
    curr = first;
}

void EntityList :: setNext() {
    if (removedFirst) {
        removedFirst = false;
        return;
    }
    if (curr && curr->next)
        curr = curr->next;
}

bool EntityList :: onLast() {
    return (curr == last);
}

bool EntityList :: currSetted() {
    return (curr != NULL);
}

int EntityList :: getSize() {
    return size;
}
