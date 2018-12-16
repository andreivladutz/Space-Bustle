#include "entityArray.h"
#include "Actor.h"

EntityArray :: EntityArray() : size(0){

}

void EntityArray :: pushBack(Entity *E) {
    if (size < MAX_ENTITIES)
        arr[size++] = E;
    else
        delete E;
}

int EntityArray :: getSize() {
    return size;
}

Entity& EntityArray :: operator[](int index) {
    return (*arr[index]);
}

void EntityArray :: remove(int pos) {
    delete arr[pos];

    for (int i = pos; i < size - 1; i++)
        arr[i] = arr[i + 1];

    size--;
}
