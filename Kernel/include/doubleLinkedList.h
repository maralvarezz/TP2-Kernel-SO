
typedef struct linkedListCDT * linkedListADT;


linkedListADT createList();

int addNode(linkedListADT list, void * info);

int removeNode(linkedListADT list, void * info);

void freeList(linkedListADT list);

int setFirst(linkedListADT list, void * info);



