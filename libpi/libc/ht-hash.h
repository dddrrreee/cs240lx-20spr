#ifndef __HT_HASH_H__
#define __HT_HASH_H__
/* 
 * engler: simple x:uint32->y:uint32 hash table based on code from Torek. 
 *
 * You need to define:
 *  HT_ALLOC
 *  HT_FREE
 * to allocate and free.
 *
 * XXX: need to add really soon:
 *  HT_KEY_T for key type
 *  HT_VAL_T for val type.
 *  HT_NIL_VAL for invalid val constant
 *
 * actually we kind of want a [set] -> void*  so that we can lookup sets, and then 
 * do intersections, etc.
 *
 * we also want a tuple hash table so we can lookup intersection and union?
 * this is such a pain in the ass.
 *
 */

// default: if you want to redefine them, first undef.
#define HT_NIL_VAL -1
#define HT_KEY_T void *
#define HT_VAL_T uint32_t

// used just for testing on unix.
//  gcc -DTEST -O -Wall ht-hash.h
#ifdef TEST
#   include <assert.h>
#   include <stdlib.h>
#   include <stdint.h>
#   include <stdio.h>
#   define HT_ALLOC malloc
#   define HT_FREE free
#   define output printf 

#else
#   include "rpi.h"
#   include "pi-random.h"
#   define HT_ALLOC kmalloc
#   define HT_FREE kfree
#   define random pi_random
#endif


// client must define all of these.
#ifndef HT_KEY_T
#   error "Must define HT_KEY_T"
#endif
#ifndef HT_ALLOC
#   error "Must define HT_ALLOC"
#endif
#ifndef HT_FREE
#   error "Must define HT_FREE"
#endif
#ifndef output
#   error "must define output"
#endif
#ifndef HT_NIL_VAL
#   error "must dfeine HT_NIL_VAL"
#endif


/* Hash table entries keep track of (uint32_t,uint32_t) pairs.  */
typedef struct hashent {
    struct  hashent *h_next;/* next in chain */
    int     h_hash;         /* hash value or ID */
    HT_KEY_T key;   // key->val
    HT_VAL_T val;		/* result (physical address */
} hash_t;

typedef struct hashtab {
    int     ht_size;        /* size (power of 2) */
    int     ht_mask;        /* == ht_size - 1 */
    int     ht_used;        /* number of entries used */
    int     ht_lim;         /* when to expand */
    hash_t  **ht_tab;	/* base of table */


    char    ht_name[1];     /* table name; actually larger */
} htab_t;

static hash_t **h_alloc(int sz);
static unsigned h_hash(HT_KEY_T va);
static void ht_expand(htab_t *ht);

uint32_t ht_search(htab_t *ht, HT_KEY_T va);

/* used by the High Quality Hash(tm) function ala' andru */
#define HASHMULT 3474701533U
#define HT_INT_TYPE unsigned long
// unsigned int nslots = 31;
enum { 
    PRECBITS = (sizeof(HT_INT_TYPE long) * 8),
    slotBits = 5 
};

/* allocate an initialize a table of hash entries */
static hash_t **h_alloc(int sz) {
    int i;
    hash_t **newh = (hash_t **)HT_ALLOC(sz * sizeof *newh), **h;
    for (h = newh, i = sz; --i >= 0;)
        *h++ = NULL;
    return newh;
}
/* multiply and eliminate zeros */
static unsigned h_hash(HT_KEY_T va) {
    return ( ( (HT_INT_TYPE)va) *HASHMULT) >> (PRECBITS - slotBits);
}

/* allocate a hash table.  sz must be power of two */
htab_t *ht_alloc(int sz) {
    htab_t *ht = HT_ALLOC(sizeof(*ht));
    assert((sz & -sz) == sz);	/* is power of two */
    ht->ht_tab = h_alloc(sz);
    ht->ht_mask = sz-1;
    ht->ht_size = sz;
    ht->ht_lim = (sz * 2) / 3;
    output("limit = %d\n", ht->ht_lim);
	return ht;
}

/* delete a key -> val mapping */
void ht_delete(htab_t *ht, HT_KEY_T key) {
	int n = h_hash(key);
	hash_t *h,*t,**p;

	p = &ht->ht_tab[n & ht->ht_mask];
	h = *p;

	for(t = NULL; h && h->key != key; t = h, h = h->h_next) 
		;
	/* there was some element that matched */
	assert(h);
	/* the first element matched */
	if(!t)
		*p = h->h_next;
	else
		t->h_next = h->h_next;
	HT_FREE(h);
    ht->ht_used--;
    assert(ht->ht_used >= 0);
}

/* Expand an existing hash table.  */
static void ht_expand(htab_t *ht) {
    int n = ht->ht_size * 2, i;
    hash_t *p, **newh, **oldh, *q;

    output("expanding hash table\n");
    newh = h_alloc(n);
    oldh = ht->ht_tab;
    n--;                    /* change to mask */
    for (i = ht->ht_size; --i >= 0;) {
        for (p = *oldh++; p != NULL; p = q) {
            q = p->h_next;
            p->h_next = newh[p->h_hash & n];
            newh[p->h_hash & n] = p;
        }
    }
    HT_FREE((char *)ht->ht_tab);
    ht->ht_tab = newh;
    ht->ht_mask = n;
    ht->ht_size = ++n;
    ht->ht_lim = (n * 2) / 3;
}

void ht_free(htab_t *ht) {
    int i;
    for (i = ht->ht_size; --i >= 0;) {
        hash_t *p,*q;
        for (p = ht->ht_tab[i]; p != NULL; p = q) {
            q = p->h_next;
            HT_FREE(p);
        }
    }
    HT_FREE(ht->ht_tab);
    HT_FREE(ht);
}

/* insert element into given hash table */
void ht_insert(htab_t *ht, HT_KEY_T key, HT_VAL_T val) {
	int n = h_hash(key);
	hash_t *h;

	/* it's just too crowded */
	if(ht->ht_lim < ht->ht_used) 
		ht_expand(ht);

	/* key is not already being mapped */
	assert(ht_search(ht, key) == HT_NIL_VAL);

	/* allocate and initialize */
	h = HT_ALLOC(sizeof *h);
	h->key = key;
	h->val = val;
	assert(ht_search(ht, key) == val);

	h->h_hash = n;
    ht->ht_used++;
	/* insert */
	h->h_next = ht->ht_tab[n & ht->ht_mask];
	ht->ht_tab[n & ht->ht_mask] = h;
}

/* search the hash table for the given virtual address */
uint32_t ht_search(htab_t *ht, HT_KEY_T key) {
	int n = h_hash(key);
	hash_t *h = ht->ht_tab[n & ht->ht_mask];

	for(; h && h->key != key; h = h->h_next)
		;
	return h ? h->val : HT_NIL_VAL;
}

#ifdef TEST
/* test the hash table implementation */
void ht_test(int trials) {
    struct { HT_KEY_T va; HT_VAL_T pa; } t[trials];
    htab_t *ht = ht_alloc(64);
    int i;

    /* initialize trial array */
    for(i=0;i<trials; i++) {
        t[i].va = (HT_KEY_T)random();
        t[i].pa = (uint32_t)random();
        ht_insert(ht,t[i].va,t[i].pa);
        assert(ht_search(ht,t[i].va) == t[i].pa);
    }

    for(i=0;i<trials*8;i++) {
        int n = random() % trials;
        assert(ht_search(ht,t[n].va) == t[n].pa);
    }
    for(i=0;i<trials-1;i++) {
        ht_delete(ht,t[i].va);
        assert(ht_search(ht,t[i+1].va) == t[i+1].pa);
        assert(ht_search(ht,t[i].va) == HT_NIL_VAL);
    }
    ht_free(ht);
}

int main(void) { ht_test(10000);  return 0; }
#endif

#endif
