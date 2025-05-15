typedef struct linkedListCDT * linkedListADT;

linkedListADT createList();

int addNode(linkedListADT list, void * info);

int removeNode(linkedListADT list, void * info);

void freeList(linkedListADT list);

int setFirst(linkedListADT list, void * info);

void toBegin(linkedListADT list);

int hasNext(linkedListADT list);

void * next(linkedListADT list);

