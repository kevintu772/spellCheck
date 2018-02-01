/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 6
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "dictionary.h"
//#include "murmurhash.h"

#define TABLE 75000

typedef struct node
{
    char* term;
    struct node* next;
}
node;

// create hashtable and its linked lists 
node* hashtable[TABLE];    
node* link;

// records the size of the dictionary
int count = 0;

// records if the dict was loaded in
bool loaded = true;

/**
 * Returns a hash address using the murmurhash3 function.
 */
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) 
{
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;
 
	uint32_t hash = seed;
 
	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	
	for (i = 0; i < nblocks; i++) 
	{
		uint32_t k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;
 
		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}
 
	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;
 
	switch (len & 3) 
	{
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];
 
		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	}
 
	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);
 
	return hash;
} 
    
/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // first convert the word to lowercase
    char* lower = malloc(sizeof(char) * strlen(word));
    strcpy(lower, word);
    
    for(int i = 0; lower[i]; i++)
        lower[i] = tolower(lower[i]);
    
    // find the hash adress
    int hash = murmur3_32(lower,strlen(lower),1) % TABLE;
    
    free(lower);
    
    // if nothing is at the hash adress, return false
    if (hashtable[hash] == NULL)
        return false;
        
    // else, iterate through the adress' linked list 
    else 
    {
        node* cursor = hashtable[hash];
                
        while (cursor != NULL)
        {
            // return true if term is found
            if (strcmp(cursor->term, lower) == 0)
            {
                return true;
                break;
            }
            
            cursor = cursor->next;
        }       
    }
    
    // else return false
    return false;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{   
    // open dictionary
    FILE* fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        printf("Could not open dictionary");
        loaded = false;
        return false;
    }
    
    // used to check fscanf for EOF
    int c;
    
    // hash adress
    int hash;

    do
    {
        // allocate memory for key and set the memory to all NULL 
        char* key = malloc(sizeof(char)*(LENGTH+1));        
        memset( key, '\0', sizeof(char)*(LENGTH+1));
        
        if (key == NULL)
        {
            free(key);
            printf("Error allocating memory\n");
            loaded = false;
            return false;              
        }
        
        // put save a word from the file into key
        c = fscanf(fp, "%s", key);
        
        // convert key to lowercase        
        for(int i = 0; key[i]; i++)
            key[i] = tolower(key[i]);
        
        // make a hash adress for key
        hash = murmur3_32(key,strlen(key),1) % TABLE;
        
        // if nothing is at the adress then put the key into that spot's term
        if (hashtable[hash] == NULL)
        {
            hashtable[hash] = malloc(sizeof(node));
            if (hashtable[hash] == NULL)
            {
                free(hashtable[hash]);
                printf("Error allocating memory\n");
                loaded = false;
                return false;               
            }
            hashtable[hash]->term = key;
            count++; 
        }
        
        // otherwise make a new link, put key into link's term, then put link
        // on the end of the linked list at the adress
        else
        {
            
            node* cursor = hashtable[hash];
                
            while (cursor->next != NULL)          
                cursor = cursor->next;       
            
            link = malloc(sizeof(node));
            if (link == NULL)
            {
                printf("Could not allocate memory");
                loaded = false;
                return false;
            }
            link->term = key;
            cursor->next = link;
            count++;
        }
        free(key);
    }  
    // iterate untill whole dictionary loaded  
    while (c != EOF);
    
    if (hashtable != NULL) 
        return true;
    loaded = false;
    return false;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // counted increments when dictionary loads
    count -= 1;
    if (loaded == true) 
    {
        return count;
    }
    return 0;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{   
    // iterate through the table and its linked lists and free all memory
    for (int i = 0; i < TABLE; i++)
    {
        node* cursor = hashtable[i];
                
        while (cursor != NULL)
        {
            node* tmp = cursor;
            cursor = cursor->next;
            free(tmp);
        }
    }
    return true;
}










