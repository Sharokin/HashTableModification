#define _CRT_SECURE_NO_WARNINGS // Behovs for vissa funktioner i visual studio
#include "HashTable.h"
#include "Bucket.h"
#include<assert.h>
#include<stdlib.h>
#include<stdio.h>

	// Anvands for att markera en ledig plats i Hashtabellen


/* Denna funktion tar en nyckel och returnerar ett hash-index
dvs ett index till arrayen som Šr Hashtabellen */
static int hash(Key key, int tablesize)
{
    return key % tablesize;
}

/*Leta framŒt enligt principen šppen adressering
 Antalet krockar returneras via pekaren col i parameterlistan*/
static int linearProbe(const HashTable* htable, Key key, unsigned int *col)
{
    int tempIndex = hash(key, htable->size);
    Key tempKey = key;
    while(htable->table[tempIndex].key != UNUSED)
    {
        tempKey++;
        tempIndex = hash(tempKey, htable->size);
        *col = *col + 1;
        if(*col == htable->size)  //har kollat igenom alla index
            break;
    }
    if(*col == htable->size)
        return *col;
    else
        return tempIndex;
}

/*Allokera minne fšr hashtabellen*/
HashTable createHashTable(unsigned int size)
{
   
    HashTable hashTable;
    hashTable.size = size;
    hashTable.table = (struct Bucket*)calloc(size, sizeof(struct Bucket));
    if(hashTable.table == NULL)
    {
        printf("Memory allocation error\n");
        hashTable.size = 0;
        return hashTable;
    }
    else
        return hashTable;
}

/* Satter in paret {key,data} i Hashtabellen, om en nyckel redan finns ska vardet uppdateras */
/* Returnerar antalet krockar (som rŠknas i linearProbe() )*/
unsigned int insertElement(HashTable* htable, const Key key, const Value value)
{
    int index = 0, indexForCollision = 0;
    unsigned int count = 0;
    index = hash(key, htable->size);
    
    if(htable->table[index].key == UNUSED) //empty
    {
        htable->table[index].key = key;
        htable->table[index].value = value;
    }
    else if(htable->table[index].key == key) //same key
        htable->table[index].value = value;
    else if(htable->table[index].key != key && htable->table[index].key != UNUSED) //collision
    {
        indexForCollision = linearProbe(htable, key, &count);
        if(indexForCollision != htable->size)
        {
            htable->table[indexForCollision].key = key;
            htable->table[indexForCollision].value = value;
        }
        else
            printf("No space left\n");
    }
    
    assert(lookup(htable, key) != NULL);
    
    return count;
}

/* Tar bort datat med nyckel "key" */
void deleteElement(HashTable* htable, const Key key)
{
    int index = hash(key, htable->size);
    
    if(htable->table[index].key != key)
    {
        int tempIndex = index, count = 0;
        Key tempKey = key;
        
        while(htable->table[tempIndex].key != key || htable->table[tempIndex].key == UNUSED )
        {
            tempKey++;
            tempIndex = hash(tempKey, htable->size);
            count++;
            if(count == htable->size-1)
                break;
        }
        if(htable->table[tempIndex].key == UNUSED)
            printf("Key not found\n");
        else
        {
            htable->table[tempIndex].key = UNUSED;
            sortAfterRemoving(htable, tempIndex);
        }
    }
    else
    {
        htable->table[index].key = UNUSED;
        sortAfterRemoving(htable, index);
    }
    
    // Postcondition: inget element med key finns i tabellen (anvand loookup() for att verifiera)
    assert(lookup(htable, key) == NULL);
}

/* Returnerar en pekare till vardet som key ar associerat med eller NULL om ingen sadan nyckel finns */
const Value* lookup(const HashTable* htable, const Key key)
{
    int index = hash(key, htable->size);
    if(htable->table[index].key == key)     // direkt matchning
    {
        Value* pointer = &htable->table[index].value;
        return pointer;
    }
    else
    {
        int tempIndex = index, count = 0;
        Key tempKey = key;
            
        while(htable->table[tempIndex].key != key || htable->table[tempIndex].key == UNUSED)
            {
                tempKey++;
                tempIndex = hash(tempKey, htable->size);
                count++;
                if(count == htable->size)
                    break;
            }
            if(count == htable->size) // Key not found
                return NULL;
            else
            {
                Value* pointer = &htable->table[tempIndex].value;
                return pointer;
            }
    }
}


/* Tommer Hashtabellen */
void freeHashTable(HashTable* htable)
{
    free(htable->table);
    htable->table = NULL;
    htable->size = 0;
    
    assert(getSize(htable) == 0);
}

/* Ger storleken av Hashtabellen */
unsigned int getSize(const HashTable* htable)
{
    int size = htable->size;
    return size;
}

/* Denna for att ni enkelt ska kunna visualisera en Hashtabell */
void printHashTable(const HashTable* htable)
{
 // printPerson()
}

void sortAfterRemoving(HashTable* htable, int index)
{
    index = index + 1; // next index
    if(index == 10)
        index = 0;
    while(htable->table[index].key != UNUSED)
   {
       Value tempValue = htable->table[index].value;
       Key tempKey = htable->table[index].key;
       
       htable->table[index].key = UNUSED;
       
       insertElement(htable, tempKey, tempValue);
       
       index++;
       if(index == 10)
           index = 0;
   }
    
}
/*Genom att skapa en ny hashtabell med den nya storleken och sedan itererat genom den gamla tabellen och kopierat šver alla data till den nya tabellen.*/
HashTable examinationFunc(HashTable* htable, int newSize) //EXAMINATION
{
    /*Pre-condition*/ assert(newSize > 0); 
    int elementsInOldTable = 0;
    for(int i = 0; i < htable->size; i++)
    {
        if(htable->table[i].key != UNUSED)
            elementsInOldTable++;
    }
    if(elementsInOldTable > newSize || newSize == htable->size)
        return *htable;
    else
    {
        HashTable newTable = createHashTable(newSize);
        for(int i = 0; i < htable->size; i++)
        {
            if(htable->table[i].key != UNUSED)
            {
                Value tempValue = htable->table[i].value;
                Key tempKey = htable->table[i].key;
                insertElement(&newTable, tempKey, tempValue);
            }
        }
        freeHashTable(htable);
        return newTable;
        /* Post-condition*/ assert(htable == NULL);
    }

}


