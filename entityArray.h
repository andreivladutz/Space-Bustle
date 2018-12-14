#ifndef ENTITYARRAY_H_INCLUDED
#define ENTITYARRAY_H_INCLUDED

#include "Actor.h"

class Entity;

struct node {
    Entity &E;
    node *next, *prev;

    node (Entity &E) : E(E), next(NULL), prev(NULL) {}
};

/*
    doubly linked list

    if not(currSetted()) got to setOnFirst() the current pointer
    and walk with setNext() until onLast() is true
*/
class EntityList {
    node *first, *last, *curr;
    int size;
    bool removedFirst;
public :
    EntityList();

    void pushBack(Entity&);
    int getSize();

    //return the current entity
    Entity& peekCurrent();

    //remove current position and set curr to the previous element
    //if we remove the first element curr becomes the second element
    //if we remove the only element curr becomes NULL
    Entity& removeCurrent();

    void setOnFirst();
    void setNext();

    bool onLast();
    bool currSetted();
};

#endif // ENTITYARRAY_H_INCLUDED
