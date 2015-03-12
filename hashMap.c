#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashMap.h"

struct hashLink {
   void* key; /*the key is what you use to look up a hashLink*/
   void* value; /*the value stored with the hashLink, a pointer to int in the case of concordance*/
   struct hashLink * next; /*notice how these are like linked list nodes*/
};

typedef struct hashLink hashLink;

struct hashMap {
   hashLink ** table; /*array of pointers to hashLinks*/
   int tableSize; /*number of buckets in the table*/
   int count; /*number of hashLinks in the table*/
};
typedef struct hashMap hashMap;


/* initialize the supplied hashMap struct*/
void _initMap (struct hashMap * ht, int tableSize)
{
   int index;
   if(ht == NULL)
      return;
   ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
   ht->tableSize = tableSize;
   ht->count = 0;
   for(index = 0; index < tableSize; index++)
      ht->table[index] = NULL;
}

/* allocate memory and initialize a hash map*/
hashMap *createMap(int tableSize) {
   assert(tableSize > 0);
   hashMap *ht;
   ht = malloc(sizeof(hashMap));
   assert(ht != 0);
   _initMap(ht, tableSize);
   return ht;
}

/*
   Free all memory used by the buckets.
   */
void _freeMap (struct hashMap * ht)
{
   assert(ht!= 0);
   hashLink *temp;
   for(int i = 0; i < ht->tableSize; i++){
      while(ht->table[i] != 0){
	 temp = ht->table[i];
	 ht->table[i] = ht->table[i]->next;
	 free(temp);
      }
   }
}

/* Deallocate buckets and the hash map.*/
void deleteMap(hashMap *ht) {
   assert(ht!= 0);
   /* Free all memory used by the buckets */
   _freeMap(ht);
   /* free the hashMap struct */
   free(ht);
}

/* 
   Resizes the hash table to be the size newTableSize 
   */
void _setTableSize(struct hashMap * ht, int newTableSize, comparator keyCompare, hashFuncPtr hashFunc)

{
   int i;
   int idx;
   int temp_count = ht->count;
   hashLink **newTable = malloc(sizeof(hashLink *) * newTableSize);
   hashLink *tempOld;
   hashLink *tempNew;
   for(i = 0; i < ht->tableSize; i++){
      tempOld = ht->table[i];
      while(tempOld != 0){
	 idx = (*hashFunc)(tempOld->key) % newTableSize;
	 tempNew = malloc(sizeof(hashLink*));
	 tempNew->value = tempOld->value;
	 tempNew->key = tempOld->key;
	 tempNew->next = newTable[idx];
	 newTable[idx] = tempNew;
	 tempOld = tempOld->next;
      }
   }
   _freeMap(ht);
   ht->tableSize = newTableSize;
   ht->count = temp_count;
   ht->table = newTable;
}

/*
   insert the following values into a hashLink, you must create this hashLink but
   only after you confirm that this key does not already exist in the table. For example, you
   cannot have two hashLinks for the word "taco".

   if a hashLink already exists in the table for the key provided you should
   replace that hashLink--this requires freeing up the old memory pointed by hashLink->value
   and then pointing hashLink->value to value v.

   also, you must monitor the load factor and resize when the load factor is greater than
   or equal LOAD_FACTOR_THRESHOLD (defined in hashMap.h).
   */
void insertMap (struct hashMap * ht, void* k, void* v, comparator keyCompare, hashFuncPtr hashFunc)
{
   int idx = (*hashFunc)(k) % ht->tableSize;
   hashLink *temp = ht->table[idx];
   while(temp != 0){
      if((*keyCompare)(temp->key, k) == 0){
	 //free(temp->value);
	 temp->value = v;
	 return;
      }
      temp = temp->next;
   }
   temp = malloc(sizeof(hashLink*));
   temp->value = v;
   temp->key = k;
   temp->next = ht->table[idx];
   ht->table[idx] = temp;
   ht->count++;
   float lf = ((float)ht->count)/ht->tableSize;
   if(lf >= LOAD_FACTOR_THRESHOLD)
      _setTableSize(ht, ht->tableSize*2, keyCompare, hashFunc);
}

/*
   this returns the value (which is void*) stored in a hashLink specified by the key k.

   if the user supplies the key "taco" you should find taco in the hashTable, then
   return the value member of the hashLink that represents taco.

   if the supplied key is not in the hashtable return NULL.
   */
void* atMap (struct hashMap * ht, void* k, comparator keyCompare, hashFuncPtr hashFunc)
{ 
   int idx = (*hashFunc)(k) % ht->tableSize;
   hashLink *temp = ht->table[idx];
   while(temp != 0){
      if((*keyCompare)(temp->key, k) == 0)
	 return temp->value;
      temp = temp->next;
   }
   return 0;
}

/*
   a simple yes/no if the key is in the hashtable. 
   0 is no, all other values are yes.
   */
int containsKey (struct hashMap * ht, void* k, comparator keyCompare, hashFuncPtr hashFunc)
{  
   int idx = (*hashFunc)(k) % ht->tableSize;
   hashLink *temp = ht->table[idx];
   while(temp != 0){
      if((*keyCompare)(temp->key, k) == 0)
	 return 1;
      temp = temp->next;
   }
   return 0;
}

/*
   find the hashlink for the supplied key and remove it, also freeing the memory
   for that hashlink. it is not an error to be unable to find the hashlink, if it
   cannot be found do nothing (or print a message) but do not use an assert which
   will end your program.
   */
void removeKey (struct hashMap * ht, void* k, comparator keyCompare, hashFuncPtr hashFunc)
{
   int idx = (*hashFunc)(k) % ht->tableSize;
   hashLink *temp = ht->table[idx];
   hashLink *remove;
   while(temp->next != 0){
      if((*keyCompare)(temp->next->key, k) == 0){
	 remove = temp->next;
         temp->next = temp->next->next;
	 free(remove->value);
         free(remove->key);
	 free(remove);
	 ht->count--;
	 return;
      }
      temp = temp->next;
   }
   if((*keyCompare)(ht->table[idx]->key, k) == 0){
      remove = ht->table[idx];
      ht->table[idx] = ht->table[idx]->next;
      free(remove->value);
      free(remove->key);
      free(remove);      
      ht->count--;
      return;
   }
}

/*
   returns the number of hashLinks in the table
   */
int size (struct hashMap *ht)
{  
   return ht->count;
}

/*
   returns the number of buckets in the table
   */
int capacity(struct hashMap *ht)
{  
   return ht->tableSize;
}

/*
   returns the number of empty buckets in the table, these are buckets which have
   no hashlinks hanging off of them.
   */
int emptyBuckets(struct hashMap *ht)
{  
   int numEmptyBuckets = 0;
   for(int i = 0; i < ht->tableSize; i++){
      if(ht->table[i] == 0)
	 numEmptyBuckets++;
   }
   return numEmptyBuckets;
}

/*
   returns the ratio of: (number of hashlinks) / (number of buckets)

   this value can range anywhere from zero (an empty table) to more then 1, which
   would mean that there are more hashlinks then buckets (but remember hashlinks
   are like linked list nodes so they can hang from each other)
   */
float tableLoad(struct hashMap *ht)
{ 
   float tl = (float)ht->count/ht->tableSize; 
   return tl;
}

/* print the hashMap */
void printMap (struct hashMap * ht, keyPrinter kp, valPrinter vp)
{
   int i;
   struct hashLink *temp;
   for(i = 0;i < capacity(ht); i++){
      temp = ht->table[i];
      if(temp != 0) {
	 printf("\nBucket Index %d -> ", i);
      }
      while(temp != 0){
	 printf("Key:");
	 (*kp)(temp->key);
	 printf("| Value: ");
	 (*vp)(temp->value);
	 printf(" -> ");
	 temp=temp->next;
      }
   }
}

/* print the keys/values ..in linear form, no buckets */
void printKeyValues (struct hashMap * ht, keyPrinter kp, valPrinter vp)
{
   int i;
   struct hashLink *temp;
   for(i = 0;i < capacity(ht); i++){
      temp = ht->table[i];
      while(temp != 0){
	 (*kp)(temp->key);
	 printf(":");
	 (*vp)(temp->value);
	 printf("\n");
	 temp=temp->next;
      }
   }
}




/* Iterator Interface */

struct mapItr {
   int count;
   struct hashLink *current;
   struct hashMap *map;
};


struct mapItr *createMapIterator(struct hashMap *ht)
{
   struct mapItr *myItr = malloc(sizeof(struct mapItr)); /* malloc_5 */
   initMapIterator(ht, myItr);
   return myItr;
}

void initMapIterator(struct hashMap *ht, struct mapItr *itr)
{
   itr->count = 0;
   itr->current = ht->table[itr->count];
   itr->map = ht;
}

int  hasNextMap(struct mapItr *itr)
{

   /* skip all the empty buckets */

   while(itr->map->table[itr->count] == 0)
   {
      itr->count++;
      itr->current = itr->map->table[itr->count];
   }

   if (itr->count >= itr->map->tableSize)
      return 0;
   
   return 1;

}

void*  nextMap(struct mapItr *itr)
{
   void* key;
   key = itr->current->key;

   /* set up for subsequent call to has Next */
   itr->current = itr->current->next;
   if(itr->current == 0) /* at end of the list*/
   {
      itr->count++; /*move on to the next bucket */
      itr->current = itr->map->table[itr->count];
   }
   return(key);
}

void removeMap(struct mapItr *itr)
{
   printf("Not yet implemented \n");
   /* Actually only a convenience since I can iterate through, get keys, and then call removeKey */
   /* This is, in fact, how I wouldimplement it here...I would simply get the last returned key and call remove Key */
   /* A slighlty more efficient solution would include double links and allow a remove iwthout calling removeKey...but it's not worth the effort here! */

}
