#ifndef ENTITYARRAY_H_INCLUDED
#define ENTITYARRAY_H_INCLUDED

class Entity;

#define MAX_ENTITIES 10

class EntityArray {
    Entity* arr[MAX_ENTITIES];
    int size;
public :
    EntityArray();

    void pushBack(Entity*);
    int getSize();

    void remove(int pos);

    Entity& operator[](int);
};

#endif // ENTITYARRAY_H_INCLUDED
