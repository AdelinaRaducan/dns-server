#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
  #define DEBUG_PRINT(a) printf a
#else
  #define DEBUG_PRINT(a) (void)0
#endif

typedef void (*TF)(void *);
typedef int (*TFHash)(void*, size_t);
typedef int (*TFComp)(void *, void *);

typedef struct Ht_item {
    char* key;
    char* ip;
    int freq;
} Ht_item;

typedef struct celula {
	void* info;
	struct celula *urm, *pre;
} TCel, *TLDI, **ALDI;

typedef struct {
	size_t size;
	int count;
	TFHash hashFunction;
	TLDI* v;
} TH;

int 		compareKey				(void *a, void *b);
int 		compareFreq				(void *a, void *b);
int 		CodHash					(void *key, size_t M);
Ht_item* 	InsertInfoInCel			(char *key, char *ip);
TLDI 		AlocCel					(char *key, char *ip) ;
TH* 		InitializeDNSserver		(int size, TFHash hashFunction);
int 		NrOfElemInBucket 		(ALDI bucket_address);
void 		SortElemInBucketByFreq	(ALDI bucket_address);
int 		KeyExist				(TH *DNS_server, char *key);
void 		ClearLastElemInBucket	(TH *DNS_server, char *key);
void 		InsertElemInBucket		(TH *DNS_server, char *key, char *ip);
void 		InsertInNewArray		(TH *DNS_server, TLDI *array, char *key, char *ip);
void 		ResizeDNSserver			(TH *DNS_server);
void 		InsertIpAddress			(TH *DNS_server, char *key, char *ip); 
char* 		IncreaseFrequency		(TH *DNS_server, char *key);
void 		FreeHtItem				(ALDI bucket_address);
void 		DeleteFirstElement		(ALDI bucket_address);
void 		DeleteKeyFromBucket		(TH *DNS_server, char *key);
void 		RemoveKey				(TH *DNS_server, char *key);
void 		PrintIp					(void *value, FILE *out);
void 		PrintDNSserver			(TH *DNS_server, FILE *out);
void 		PrintBucket				(TH *DNS_server, int index, FILE *out); 
void 		DestroyDNS				(TH **DNS_server); 
void 		ReadOneInputValue 		(FILE *inputFile, char **value, int *size);

int main(int argc, char const *argv[]) {
	int HTSize = atoi(argv[1]);

	TH *DNS_server = InitializeDNSserver(HTSize, CodHash);

	FILE *inputFile = fopen(argv[2], "rt");
    if (!inputFile) {
        printf("Error! Can't open input file\n");
        return -1;
    }

    FILE *outputFile = fopen(argv[3], "wt");
    if (!outputFile) {
    	printf("Error! Can't open output file\n");
        return -1;
    }

    char operation[10], key[120], ip[20];
    int index;

    fscanf(inputFile, "%s", operation);
    do {
    	
    	if (strcmp(operation, "set") == 0) {
    		fscanf(inputFile, "%s", key);
    		fscanf(inputFile, "%s", ip);
    		DEBUG_PRINT(("key: %s, ip: %s\n", key, ip));
    		InsertIpAddress(DNS_server, key, ip);
    	} else if (strcmp(operation, "print") == 0) {
    		PrintDNSserver(DNS_server, outputFile);
    	} else if (strcmp(operation, "get") == 0) {
    		fscanf(inputFile, "%s", key);
    		char* val = IncreaseFrequency(DNS_server, key);
    		DEBUG_PRINT(("get value: %s", val));
    		if (!val) {
    			fprintf(outputFile, "NULL\n");
    		} else {
    			fprintf(outputFile, "%s\n", val);
    		}
    	} else if (strcmp(operation, "print_list") == 0) {
    		fscanf(inputFile, "%d", &index);
			DEBUG_PRINT(("index: %d\n", *index));
			PrintBucket(DNS_server, index, outputFile);
    	} else if (strcmp(operation, "remove") == 0) {
 			fscanf(inputFile, "%s", key);
 			RemoveKey(DNS_server, key);
    	}

    	fscanf(inputFile, "%s", operation);
	} while (!feof(inputFile));
	
	DestroyDNS(&DNS_server);
	fclose(inputFile);
	fclose(outputFile);
	return 0;
}

void ReadOneInputValue(FILE *inputFile, char **value, int *size) {
	int ch;
	int nr_ch = 0;

    while ((ch = fgetc(inputFile) != ' ') && (ch != EOF) && (ch != '\n')) {
    	if (nr_ch == (*size-1)) {
       		(*size) *= 2;
        	(*value) = realloc((*value), *size);
        }
        
        (*value)[nr_ch++] = (char) ch;
    }

    (*value)[nr_ch] = '\0';
}

int compareKey(void *a, void *b) { 
	return strcmp(((Ht_item*)a)->key, ((Ht_item*)b)->key); 
}

int compareFreq(void *a, void *b) { 
	return (((Ht_item*)a)->freq - ((Ht_item*)b)->freq); 
}

int CodHash(void *key, size_t M) {
	unsigned long sum;
	char *x = (char*) key;

	for (sum = 0; *x != '\0'; x++) {
		sum += *x;
	}

	return (sum % M);
}

Ht_item* InsertInfoInCel(char *key, char *ip) {
    Ht_item *item = (Ht_item*) malloc(sizeof(Ht_item));
    item->key = (char*) malloc(strlen(key) + 1);
    item->ip = (char*) malloc(strlen(ip) + 1);
     
    strcpy(item->key, key);
    strcpy(item->ip, ip);
    item->freq = 0;
 
    return item;
}

//Creates a pointer to a new cel of the bucket
TLDI AlocCel(char *key, char *ip) {
	TLDI aux = (TLDI) calloc(1, sizeof(TCel));

	if (aux) {
		aux->urm = NULL;
		aux->pre = NULL;
		aux->info = InsertInfoInCel(key, ip);
	}

	return aux;
}


TH* InitializeDNSserver(int size, TFHash hashFunction) {
	TH *DNS_server = (TH*) calloc(1, sizeof(TH));

	if (!DNS_server) return NULL;
	
	DNS_server->v = (TLDI*) calloc(size, sizeof(TLDI));
	
	if (!DNS_server->v) {
		free(DNS_server);
		return NULL;
	}

	DNS_server->size = size;
	DNS_server->hashFunction = hashFunction;

	return DNS_server;
}

int NrOfElemInBucket(ALDI bucket_address) {
	TLDI head_bucket = *bucket_address;
	int nr = 0;

	if (!head_bucket) return nr;

	nr++;

	while (head_bucket->urm != *bucket_address) {
		nr++;
		head_bucket = head_bucket->urm;
	}

	return nr;
}

void SortElemInBucketByFreq(ALDI bucket_address) { 
	TLDI current = *bucket_address, next = NULL; 
	Ht_item *aux = NULL;

	if (current == NULL) 
		return;

	do {
		next = current->urm;
		while (next != *bucket_address) {
			if (compareFreq(current->info, next->info) < 0) {
				aux = current->info;
				current->info = next->info;
				next->info = aux;
			} else if (compareFreq(current->info, next->info) == 0) {
				if (compareKey(current->info, next->info) > 0) {
				aux = current->info;
				current->info = next->info;
				next->info = aux;
				}
			}
			next = next->urm;	
		}
		current = current->urm;
	} while (current->urm != *bucket_address);
}

int KeyExist(TH *DNS_server, char *key) {
	int cod = DNS_server->hashFunction(key, DNS_server->size);
	TLDI el = DNS_server->v[cod];
	Ht_item *address_info;

	for (el = DNS_server->v[cod]; el != NULL; el = el->urm) {
		address_info = (Ht_item*) el->info;

		if (strcmp(address_info->key, key) == 0) {
			return 1;
		}

		if (el->urm == DNS_server->v[cod]) break;
	}
	return 0;
}

void ClearLastElemInBucket(TH *DNS_server, char *key) {
	int cod = DNS_server->hashFunction(key, DNS_server->size);
	TLDI copy = DNS_server->v[cod];

	while (copy->urm != DNS_server->v[cod]) {
		copy = copy->urm;
	}

	(copy->pre)->urm = copy->urm;
	(copy->urm)->pre = copy->pre;
	FreeHtItem(&copy);
}

void InsertElemInBucket(TH *DNS_server, char *key, char *ip) {
	int cod = DNS_server->hashFunction(key, DNS_server->size);
	int nrElem = NrOfElemInBucket(&DNS_server->v[cod]);
	TLDI new_node = AlocCel(key, ip);
	ALDI head_ref = &DNS_server->v[cod];
	TLDI last;

	if (*head_ref == NULL) {
		new_node->pre = new_node->urm = new_node;
		*head_ref = new_node;
		DNS_server->count++;
	} else {
		if (nrElem >= DNS_server->size) {
			ClearLastElemInBucket(DNS_server, key);
			DNS_server->count--;
		} 

		last = (*head_ref)->pre;
		new_node->urm = *head_ref;
		(*head_ref)->pre = new_node;
		new_node->pre = last;
		last->urm = new_node;
		SortElemInBucketByFreq(&DNS_server->v[cod]);
		DNS_server->count++;
	}
}

void InsertInNewArray(TH *DNS_server, TLDI *array, char *key, char *ip) { 
	int cod = DNS_server->hashFunction(key, DNS_server->size);
	TLDI new_node = AlocCel(key, ip);
	TLDI last;
	
	if (!array[cod]) {
		new_node->pre = new_node->urm = new_node;
		array[cod] = new_node;
	} else {
		last = (array[cod])->pre;
		new_node->urm = array[cod];
		(array[cod])->pre = new_node;
		new_node->pre = last;
		last->urm = new_node;
	}
}

void ResizeDNSserver(TH *DNS_server) {
    int oldCapacity = DNS_server->size;
    TLDI *newArray;
    Ht_item *address_info;

    if (DNS_server->size == 0) {
        DNS_server->size = 1;
    } else {
        DNS_server->size *= 2;
    }

    newArray = (TLDI*) calloc(DNS_server->size, sizeof(TLDI*));

    if (!newArray) {
        return;
    }

    for (int i = 0; i < oldCapacity; i++) {
        TLDI aux = DNS_server->v[i];
        TLDI nodeToDelete;

        while (aux) {
        	nodeToDelete = aux;
            address_info = aux->info;
            InsertInNewArray(DNS_server, newArray, address_info->key, address_info->ip);
            aux = aux->urm;
            FreeHtItem(&nodeToDelete);
            free(nodeToDelete);
            if (aux == DNS_server->v[i]) break;
        }
    }

    free(DNS_server->v);
    DNS_server->v = newArray;
}

void InsertIpAddress(TH *DNS_server, char *key, char *ip) {
	if (KeyExist(DNS_server, key)) return;
	
	if (DNS_server->count > (DNS_server->size * 2)) {
		ResizeDNSserver(DNS_server);
	}

	InsertElemInBucket(DNS_server, key, ip);
}

//if key exist in DNS server increase frequency and return ip, else return NULL
char* IncreaseFrequency(TH *DNS_server, char *key) { 
	int cod = DNS_server->hashFunction(key, DNS_server->size);
	TLDI el;
	Ht_item *address_info;

	if (!KeyExist(DNS_server, key)) return NULL;

	for (el = DNS_server->v[cod]; el != NULL; el = el->urm) {
		address_info = (Ht_item*) el->info;

		if (strcmp(address_info->key, key) == 0) {
			address_info->freq++;
			SortElemInBucketByFreq(&DNS_server->v[cod]);
			return address_info->ip;
		}

		if (el->urm == DNS_server->v[cod]) break;
	}

	return NULL;
}

void FreeHtItem(ALDI bucket_address) {
	Ht_item *address_info = (*bucket_address)->info;

	free(address_info->key);
	address_info->key = NULL;
	free(address_info->ip);
	address_info->ip = NULL;
	free(address_info);
	address_info = NULL;
	*bucket_address = NULL;
}

void DeleteHeadElement(ALDI bucket_address) { 
	TLDI head_ref = *bucket_address;
	TLDI bucket_elem, last_elem;

	if (head_ref->urm == *bucket_address) {
		FreeHtItem(bucket_address);
		return;
	}

	last_elem = (*bucket_address)->pre;
	bucket_elem = (*bucket_address)->urm;
	bucket_elem->pre = (*bucket_address)->pre;
	last_elem->urm = bucket_elem;
	(*bucket_address) = bucket_elem;
	FreeHtItem(&head_ref);
}

void DeleteKeyFromBucket(TH *DNS_server, char *key) { 
	int cod = DNS_server->hashFunction(key, DNS_server->size);
	TLDI copy = DNS_server->v[cod];
	ALDI bucket = &DNS_server->v[cod];
	Ht_item *address_info = copy->info;

	if (strcmp(key, address_info->key) == 0) {
		DeleteHeadElement(bucket);
		return;
	}

	for (;; copy = copy->urm) {
		address_info = copy->info;

		if (strcmp(key, address_info->key) == 0) {
			(copy->pre)->urm = copy->urm;
			(copy->urm)->pre = copy->pre;
			FreeHtItem(&copy);
			return;
		}

		if (copy->urm == DNS_server->v[cod]) break;
	}
}

void RemoveKey(TH *DNS_server, char *key) {
	if (!KeyExist(DNS_server, key))  return;

	DeleteKeyFromBucket(DNS_server, key);
}

void PrintIp(void *value, FILE *out) {
  Ht_item *address_info = (Ht_item*)value;

  fprintf(out, "(%s)", address_info->ip);
}

void PrintDNSserver(TH *DNS_server, FILE *out) {
	int cod;

	for (cod = 0; cod < DNS_server->size; cod++) {
		if (DNS_server->v[cod]) {
			PrintBucket(DNS_server, cod, out);
		}
	}
}

void PrintBucket(TH *DNS_server, int index, FILE *out) {
	if ((index < 0) || (index >= DNS_server->size)) return;

	TLDI p = DNS_server->v[index];

	if (!DNS_server->v[index]) {
		fprintf(out, "%d: VIDA\n", index);
		return;
	}
	
	fprintf(out, "%d: ", index);

	while (p->urm != DNS_server->v[index]) {
		PrintIp(p->info, out);
		p = p->urm;

		if (p != DNS_server->v[index]) {
			fprintf(out, " ");
		}
	}

	PrintIp(p->info, out);
	fprintf(out, "\n");
}

void DestroyDNS(TH **DNS_server) {
	TLDI *aux_array;
	TLDI bucket_address, aux;

	for (aux_array = (*DNS_server)->v; aux_array < (*DNS_server)->v + (*DNS_server)->size; aux_array++) {
			for (bucket_address = *aux_array; bucket_address != NULL;) {
				aux = bucket_address;
				bucket_address = bucket_address->urm;
				FreeHtItem(&aux);
				free(aux);
				if (bucket_address == *aux_array) break;	
			}	
	}
	free((*DNS_server)->v);
	free(*DNS_server);
	*DNS_server = NULL;		
}

