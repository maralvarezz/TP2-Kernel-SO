#include "doubleLinkedList.h"
#include "video.h"

typedef struct Node {
    void * info; //se va a usar para guardar estructuras de control de procesos (TPCB)
    struct Node * next;
    struct Node * prev;
} Node;

typedef struct Node * TNode;

typedef struct linkedListCDT{
    TNode current;
    TNode first;
    TNode last;
    int size;
} linkedListCDT;

linkedListADT createList(){
    linkedListADT newList = allocMemory(sizeof(linkedListCDT));
    if(newList == NULL){
        return NULL;
    }
    printf("creando lista en %d\n",(uint64_t)newList);
    newList->current = NULL;
    newList->first = NULL;
    newList->last = NULL;
    newList->size = 0;
    return newList;
}

int addNode(linkedListADT list, void * info){
    printf("recibimos lista en %d\n",(uint64_t)((void *)list));
    if(list == NULL){
        return 0;
    }
    printf("agregando nodo\n");
    TNode newNode = allocMemory(sizeof(Node));
    if(newNode == NULL){
        return 0;
    }
    printf("agregando info2\n");
    newNode->info = info;
    newNode->next = NULL;
    printf("agregando info3\n");
    printf("list->size%d\n", list->size);
    if(list->size == 0){ //soy el primero 
        printf("agregando info3.5\n");
        list->first = newNode;
    }
    else{
        printf("agregando info3.6\n");
        list->last->next = newNode;
    }
    printf("agregando info4\n");
    newNode->prev = list->last;
    list->last = newNode;
    list->size++;
    printf("agregando info5\n");
    return 1;
}

int removeNode(linkedListADT list, void * info){
    if(list == NULL || list->size == 0){
        return 0;
    }

    TNode iter = list->first;
    while(iter != NULL){
        if(iter->info == info){
            if(iter == list->first){ //si soy el primero
                list->first = iter->next;
                list->first->prev = NULL;
            }
            else if(iter == list->last){ //si soy el ultimo
                list->last = iter->prev;
                list->last->next = NULL;
            }
            else{
                iter->prev->next = iter->next;
                iter->next->prev = iter->prev;
            }
            freeMemory(iter);
            list->size--;
            return 1;
        }
        iter = iter->next;
    }

    return 0;
}

static void freeRec(TNode nodo){
    if(nodo == NULL){
        return;
    }
    TNode toFree = nodo->next;
    freeMemory(nodo);
    freeRec(toFree);
}

void freeList(linkedListADT list){
    if(list == NULL){
        return;
    }
    freeRec(list->first);
    freeMemory(list);
}

int isEmpty(linkedListADT list){
    return (list == NULL || list->size == 0);
}

int setFirst(linkedListADT list, void * info){
    if(list == NULL){
        return 0;
    }
    TNode newNode = allocMemory(sizeof(Node));
    newNode->info = info;
    newNode->next = list->first;
    newNode->prev = NULL;
    list->first = newNode;
    return 1;
}

void * getFirst(linkedListADT list){
    if(list == NULL || list->first == NULL){
        return NULL;
    }
    void * info = list->first->info;
    removeNode(list, info);
    list->first = list->first->next;
    return info;
}

void toBegin(linkedListADT list){
    if(list == NULL){
        return;
    }
    list->current = list->first;
}

int hasNext(linkedListADT list){
    return !(list == NULL || list->current == NULL);
}

void * next(linkedListADT list){
    if(!hasNext(list)){
        return NULL;
    }
    void * info = list->current->info;
    removeNode(list, info);
    list->current = list->current->next;
    return info;
}
