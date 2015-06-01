#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashMap.h"



/*the first hashing function you can use*/
/* RETURNS THE SUM OF THE DECIMAL ASCII VALUES OF EACH INDIVIDUAL CHAR OF THE CHAR LINKED LIST ARGUMENT */
int stringHash1(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += str[i];
	return r;
}

/*the second hashing function you can use*/
/* RETURNS THE SUM OF ((index + 1) * ASCII decimalValue at char index i) OF EACH INDIVIDUAL CHAR OF THE CHAR LINKED LIST ARGUMENT*/
/* THE VALUE OF INDEX SHIFTED, MULTIPLIED BY THE ASCII VALUE OF THE CHAR */
/* CREATES MORE UNIQUE HASH INDEX WHICH CAN BE MOD'D TO VALUE INDEX */
/* EXAMPLE: 
	* i = 0 : 1 * ACSII
	* i = 1 : 2 * ASCII
	*...
*/ 
int stringHash2(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += (i+1) * str[i]; /*the difference between stringHash1 and stringHash2 is on this line*/
	return r;
}

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
 Note: Before freeing up a hashLink, free the memory occupied by key and value
 */
void _freeMap (struct hashMap * ht)
{  
	/*write this*/		
	assert(ht != 0);
	hashLink* iter = NULL;
	hashLink* temp = NULL;
	for(int i = 0; i < ht->tableSize; i++){
		iter = ht->table[i];
		while(iter != 0){	
			removeKey(ht, iter->key);
			temp = iter;
			iter = iter->next;
			free(temp);
			temp = NULL;
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
void _setTableSize(struct hashMap * ht, int newTableSize)
{
	/*write this*/			
	assert(ht != 0);
	hashMap* newMap = createMap(newTableSize);;
	for(int i = 0; i < ht->count; i++){
		if(ht->table[i] != NULL){
			hashLink* iter = ht->table[i];
			do{
				insertMap(newMap, iter->key, iter->value);
				iter = iter->next;
			}while(iter != 0);
		}
	}	
	ht = newMap;
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
void insertMap (struct hashMap * ht, KeyType k, ValueType v)
{  
	/*write this*/	
	assert(ht != 0);
	if(containsKey(ht, k) <= 0){
		int hashIndex;
		if(HASHING_FUNCTION <= 1){
			hashIndex = stringHash1(k) % ht->tableSize;	
		}else{
			hashIndex = stringHash2(k) % ht->tableSize;	
		}
		if(hashIndex < 0){ hashIndex += ht->tableSize; }
		hashLink* newLink = NULL;
		newLink = (hashLink*)malloc(sizeof(hashLink));
		assert(newLink != 0);
		newLink->key = k;
		newLink->value = v;
		newLink->next = ht->table[hashIndex];
		ht->table[hashIndex] = newLink;
		++ht->count;
		if(tableLoad(ht) >= LOAD_FACTOR_THRESHOLD){ _setTableSize(ht, (ht->tableSize *= 2)); }
	}else if(containsKey(ht, k) > 0){
	        ValueType* value = atMap(ht, k);
		(*value)++;
	}
}

/*
 this returns the value (which is void*) stored in a hashLink specified by the key k.
 
 if the user supplies the key "taco" you should find taco in the hashTable, then
 return the value member of the hashLink that represents taco.
 
 if the supplied key is not in the hashtable return NULL.
 */
ValueType* atMap (struct hashMap * ht, KeyType k)
{ 
	/*write this*/
	assert(ht != 0);
	int hashIndex;
	if(HASHING_FUNCTION <= 1){
		hashIndex = stringHash1(k) % ht->tableSize;	
	}else{
		hashIndex = stringHash2(k) % ht->tableSize;	
	}
	if(hashIndex < 0){ hashIndex += ht->tableSize; }

	hashLink* iter = NULL;
	iter = ht->table[hashIndex];
	ValueType* temp = NULL;
	while(iter != 0){
		if(iter->key == k){
			return temp = &iter->value;
		}else{
			iter = iter->next;
		}
	}
	
	return NULL;
	
}

/*
 a simple yes/no if the key is in the hashtable. 
 0 is no, all other values are yes.
 */
int containsKey (struct hashMap * ht, KeyType k)
{  
	/*write this*/

	assert(ht != 0);
	int hashIndex;
	if(HASHING_FUNCTION <= 1){
		hashIndex = stringHash1(k) % ht->tableSize;	
	}else{
		hashIndex = stringHash2(k) % ht->tableSize;	
	}
	if(hashIndex < 0){ hashIndex += ht->tableSize; }

	hashLink* iter = NULL;
	iter = ht->table[hashIndex];
	while(iter != 0){
		if(strcmp((KeyType)iter->key, (KeyType)k) == 0){
			return 1;
		}else{
			iter = iter->next;
		}
	}

	return 0;
}

/*
 find the hashlink for the supplied key and remove it, also freeing the memory
 for that hashlink. it is not an error to be unable to find the hashlink, if it
 cannot be found do nothing (or print a message) but do not use an assert which
 will end your program.
 */
void removeKey (struct hashMap * ht, KeyType k)
{  
	/*write this*/	

if(containsKey(ht, k) == 1){
	assert(ht != 0);
	int hashIndex;
	if(HASHING_FUNCTION <= 1){
		hashIndex = stringHash1(k) % ht->tableSize;	
	}else{
		hashIndex = stringHash2(k) % ht->tableSize;	
	}
	if(hashIndex < 0){ hashIndex += ht->tableSize; }

	hashLink* iterHead = ht->table[hashIndex];
	hashLink* iterNext = ht->table[hashIndex]->next; 
	if(strcmp(iterHead->key, k) == 0){
		hashLink* tmp = iterHead;
		ht->table[hashIndex] = iterNext;
		free(&(tmp->key));
		free(tmp->key);
		--ht->count;
	}else{
		do{
			if(strcmp(iterNext->key, k) == 0){
				hashLink* tmp = iterNext;
				iterHead->next = iterNext->next;
				free(&(tmp->key));
				free(tmp->key);
				--ht->count;
				break;
			}		
			iterHead = iterHead->next;
			iterNext = iterNext->next;
		}while(iterNext != 0);
	}
}
}

/*
 returns the number of hashLinks in the table
 */
int size (struct hashMap *ht)
{  
	/*write this*/

	return ht->count;
	
}

/*
 returns the number of buckets in the table
 */
int capacity(struct hashMap *ht)
{  
	/*write this*/
	
	return ht->tableSize;
}

/*
 returns the number of empty buckets in the table, these are buckets which have
 no hashlinks hanging off of them.
 */
int emptyBuckets(struct hashMap *ht)
{  
	/*write this*/
	assert(ht != 0);
	return (ht->tableSize - ht->count);
}

/*
 returns the ratio of: (number of hashlinks) / (number of buckets)
 
 this value can range anywhere from zero (an empty table) to more then 1, which
 would mean that there are more hashlinks then buckets (but remember hashlinks
 are like linked list nodes so they can hang from each other)
 */
float tableLoad(struct hashMap *ht)
{  
	/*write this*/

	float loadFactor = (ht->count / (float) ht->tableSize);
	return loadFactor;
}
void printMap (struct hashMap * ht)
{
	int i;
	struct hashLink *temp;	
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		if(temp != 0) {		
			printf("\nBucket Index %d -> ", i);		
		}
		while(temp != 0){			
			printf("Key:%s|", temp->key);
			printValue(temp->value);
			printf(" -> ");
			temp=temp->next;			
		}		
	}
}


