# ifndef HashMap
# define HashMap



typedef void (*keyPrinter)(void *key);
typedef void (*valPrinter)(void *val);

typedef int (*comparator)(void* key1, void* key2);
typedef  int (*hashFuncPtr)(void* key);

/* load factor threshold for resizing the hash table */
# define LOAD_FACTOR_THRESHOLD 8.0

struct hashMap *createMap(int tableSize);

void deleteMap(struct hashMap *ht);

void insertMap (struct hashMap * ht, void* k, void* v, comparator keyCompare, hashFuncPtr hashFunc);

void* atMap (struct hashMap * ht, void* k, comparator keyCompare, hashFuncPtr hashFunc);

int containsKey (struct hashMap * ht, void* k, comparator keyCompare, hashFuncPtr hashFunc);

void removeKey (struct hashMap * ht, void* k, comparator keyCompare, hashFuncPtr hashFunc);

int size (struct hashMap *ht);

int capacity(struct hashMap *ht);

int emptyBuckets(struct hashMap *ht);

float tableLoad(struct hashMap *ht);

void printMap (struct hashMap * ht, keyPrinter kp, valPrinter vp);
void printKeyValues(struct hashMap *ht, keyPrinter kp, valPrinter vp);

/* Iterator Interface */
struct mapItr *createMapIterator(struct hashMap *ht);
void initMapIterator(struct hashMap *ht, struct mapItr *itr);
int hasNextMap(struct mapItr *itr);
void* nextMap(struct mapItr *itr);
void removeMap(struct mapItr *itr);
# endif
