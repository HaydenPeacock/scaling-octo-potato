#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lab5.h"

/* ========== Node Functions ========== */

/* TODO 1: Implement create_question_node
 * - Allocate memory for a Node structure
 * - Use strdup() to copy the question string (heap allocation)
 * - Set isQuestion to 1
 * - Initialize yes and no pointers to NULL
 * - Return the new node
 */
Node *create_question_node(const char *question) {
	//Allocate memory for a Node structure
    Node* qNode = (Node*) malloc(sizeof(Node));

    //Use strdup() to copy the question string (heap allocation)
    char* dupeQ = (char*) strdup(question);
    qNode->text = dupeQ;

    //Set isQuestion to 1
    qNode->isQuestion = 1;

    //Initialize yes and no pointers to NULL
    qNode->yes = NULL;
    qNode->no = NULL;

	//Return the new node
    return qNode;
}

/* TODO 2: Implement create_animal_node
 * - Similar to create_question_node but set isQuestion to 0
 * - This represents a leaf node with an animal name
 */
Node *create_animal_node(const char *animal) {
    //Similiar to create_question_node but set isQuestion to 0
    Node* aNode = (Node*) malloc(sizeof(Node));
    char* dupeA = (char*) strdup(animal);
    aNode->text = dupeA;
    aNode->isQuestion = 0;
    aNode->yes = NULL;
    aNode->no = NULL;
    return aNode;
}

/* TODO 3: Implement free_tree (recursive)
 * - This is one of the few recursive functions allowed
 * - Base case: if node is NULL, return
 * - Recursively free left subtree (yes)
 * - Recursively free right subtree (no)
 * - Free the text string
 * - Free the node itself
 * IMPORTANT: Free children before freeing the parent!
 */
void free_tree(Node *node) {
    //Base case: if node is NULL, return
    if(node == NULL){
	return;
    }

    //Recursively free left subtree (yes)
    free_tree(node->yes);

    //Recursively free right subtree (no)
    free_tree(node->no);

    //Free the text string
    free(node->text);

    //Free the node itself
    free(node);
    return;
}

/* TODO 4: Implement count_nodes (recursive)
 * - Base case: if root is NULL, return 0
 * - Return 1 + count of left subtree + count of right subtree
 */
int count_nodes(Node *root) {
    //Base case: if root is NULL, return 0
    if(root == NULL){
	    return 0;
    }

    //Return 1 + count of left subtree + count of right subtree
    return 1 + count_nodes(root->yes) + count_nodes(root->no);
}

/* ========== Frame Stack (for iterative tree traversal) ========== */

/* TODO 5: Implement fs_init
 * - Allocate initial array of frames (start with capacity 16)
 * - Set size to 0
 * - Set capacity to 16
 */
void fs_init(FrameStack *s) {
	//Allocate initial array of frames (start with capacity 16)
    s->frames = (Frame*) malloc(sizeof(Frame) * 16);

    //Set size to 0
    s->size = 0;

    //Set capacity to 16
    s->capacity = 16;
}

/* TODO 6: Implement fs_push
 * - Check if size >= capacity
 *   - If so, double the capacity and reallocate the array
 * - Store the node and answeredYes in frames[size]
 * - Increment size
 */
void fs_push(FrameStack *s, Node *node, int answeredYes) {
    //Check if size >= capacity and realloc if so
    if(s->size >= s->capacity){
	    s->capacity = s->capacity*2;
	    s->frames = (Frame*) realloc(s->frames, s->capacity * sizeof(Frame));
    }

    //Store the node and answeredYes in frames[size]
    s->frames[s->size].node = node;
    s->frames[s->size].answeredYes = answeredYes;

    //Increment size
    s->size = s->size + 1;
    return;
}

/* TODO 7: Implement fs_pop
 * - Decrement size
 * - Return the frame at frames[size]
 * Note: No need to check if empty - caller should use fs_empty() first
 */
Frame fs_pop(FrameStack *s) {
    //Frame dummy = {NULL, -1};
    //Decerement size
    s->size = s->size - 1;

    //Return the frame at frames[size] 
    return s->frames[s->size];
}

/* TODO 8: Implement fs_empty
 * - Return 1 if size == 0, otherwise return 0
 */
int fs_empty(FrameStack *s) {
	//return 1 if size == 0	
    if(s->size == 0){
	    return 1;
    }
    //return 0 otherwise
    return 0;
}

/* TODO 9: Implement fs_free
 * - Free the frames array
 * - Set frames pointer to NULL
 * - Reset size and capacity to 0
 */
void fs_free(FrameStack *s) {
	//Free the frames array
	free(s->frames);
	
	//Set frames pointer to NULL
	s->frames = NULL;

	//Reset size and capacity to 0
	s->size = 0;
	s->capacity = 0;
	return;
}

/* ========== Edit Stack (for undo/redo) ========== */

/* TODO 10: Implement es_init
 * Similar to fs_init but for Edit structs
 */
void es_init(EditStack *s) {
           //Allocate initial array of Edits (start with capacity 16)
    s->edits = (Edit*) malloc(sizeof(Edit) * 16);

    //Set size to 0
    s->size = 0;

    //Set capacity to 16
    s->capacity = 16;  
}

/* TODO 11: Implement es_push
 * Similar to fs_push but for Edit structs
 * - Check capacity and resize if needed
 * - Add edit to array and increment size
 */
void es_push(EditStack *s, Edit e) {
   //Check if size >= capacity and realloc if so
    if(s->size >= s->capacity){
            s->capacity = s->capacity*2;
            s->edits = (Edit*) realloc(s->edits, s->capacity * sizeof(Edit));
    }

    //Store the edit in edits[s->size]
    s->edits[s->size] = e;

    //Increment size
    s->size = s->size + 1;
    return; 
}

/* TODO 12: Implement es_pop
 * Similar to fs_pop but for Edit structs
 */
Edit es_pop(EditStack *s) {
    //Edit dummy = {0};
    //Decerement size
    s->size = s->size - 1;

    //Return the edit at edits[s->size]
    return s->edits[s->size];
}

/* TODO 13: Implement es_empty
 * Return 1 if size == 0, otherwise 0
 */
int es_empty(EditStack *s) {
    //return 1 if size == 0, otherwise 0
    if(s->size == 0){
	    return 1;
    }
    return 0;
}

/* TODO 14: Implement es_clear
 * - Set size to 0 (don't free memory, just reset)
 * - This is used to clear the redo stack when a new edit is made
 */
void es_clear(EditStack *s) {
    //set size to 0 (don't free memory just reset)
    s->size = 0;
    return;
}

void es_free(EditStack *s) {
    free(s->edits);
    s->edits = NULL;
    s->size = 0;
    s->capacity = 0;
}

void free_edit_stack(EditStack *s) {
    es_free(s);
}

/* ========== Queue (for BFS traversal) ========== */

/* TODO 15: Implement q_init
 * - Set front and rear to NULL
 * - Set size to 0
 */
void q_init(Queue *q) {
    //set front and rear to NULL
    q->front = NULL;
    q->rear = NULL;
    
    //set size to 0
    q->size = 0;
}

/* TODO 16: Implement q_enqueue
 * - Allocate a new QueueNode
 * - Set its treeNode and id fields
 * - Set its next pointer to NULL
 * - If queue is empty (rear == NULL):
 *   - Set both front and rear to the new node
 * - Otherwise:
 *   - Link rear->next to the new node
 *   - Update rear to point to the new node
 * - Increment size
 */
void q_enqueue(Queue *q, Node *node, int id) {
    //Allocate a new QueueNode
    QueueNode* newQn = (QueueNode*) malloc(sizeof(QueueNode));

    //Set its treeNode and id fields
    newQn->treeNode = node;
    newQn->id = id;

    //Set its nex pointer to NULL
    newQn->next = NULL;

    //If queue is empty set both front and rear to the new node
    if(q_empty(q)){
	    q->front = newQn;
	    q->rear = newQn;
    }
    
    //else link rear->next to the new node
    //update rear to point to the new node
    else{
	   q->rear->next = newQn;
	   q->rear = newQn;
    }

    //Increment size
    q->size = q->size + 1;

    return;

}

/* TODO 17: Implement q_dequeue
 * - If queue is empty (front == NULL), return 0
 * - Save the front node's data to output parameters (*node, *id)
 * - Save front in a temp variable
 * - Move front to front->next
 * - If front is now NULL, set rear to NULL too
 * - Free the temp node
 * - Decrement size
 * - Return 1
 */
int q_dequeue(Queue *q, Node **node, int *id) {
    //if queue is empty, return 0
    if(q_empty(q)){
	    return 0;
    }

    //Save the front node's data to output parameters
    *node = q->front->treeNode;
    *id = q->front->id;

    //Save front in a temp variable
    QueueNode* temp = q->front;

    //move front to front->next
    q->front = q->front->next;

    //if front is now NULL, set rear to NULL too;
    if(q->front == NULL){
	    q->rear = NULL;
    }

    //free the temp node
    free(temp);

    //decrement size
    q->size = q->size - 1;

    //return 1
    return 1;
}

/* TODO 18: Implement q_empty
 * Return 1 if size == 0, otherwise 0
 */
int q_empty(Queue *q){
	//return 1 if size == 0, otherwise 0
    if (q->size == 0){
	    return 1;
    }
    return 0;
}

/* TODO 19: Implement q_free
 * - Dequeue all remaining nodes
 * - Use a loop with q_dequeue until queue is empty
 */
void q_free(Queue *q) {
    //Dequeue all remaining nodes (use a loop with q_dequeue until queue is empty)
    int id;
    Node* node;
    while(q_dequeue(q, &node, &id)){
	    free(node);
    }
    return;
}

/* ========== Hash Table ========== */

/* TODO 20: Implement canonicalize
 * Convert a string to canonical form for hashing:
 * - Convert to lowercase
 * - Keep only alphanumeric characters
 * - Replace spaces with underscores
 * - Remove punctuation
 * Example: "Does it meow?" -> "does_it_meow"
 * 
 * Steps:
 * - Allocate result buffer (strlen(s) + 1)
 * - Iterate through input string
 * - For each character:
 *   - If alphanumeric: add lowercase version to result
 *   - If whitespace: add underscore
 *   - Otherwise: skip it
 * - Null-terminate result
 * - Return the new string
 */
char *canonicalize(const char *s) {
    //allocate result buffer (strlen(s) + 1)
    int length = strlen(s);
    char* result = (char*) malloc(length+1);
    
    //iterate through input string
    int i = 0;
    int j = 0;
    while(s[i]){
	    //uppercase case
	    if(s[i] >= 'A' && s[i] <= 'Z'){
		    result[j] = tolower(s[i]);
		    i++;
		    j++;
		    continue;
	    }

	    //lowercase or number case
	    if((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= '0' && s[i] <= '9')){
		    result[j] = s[i];
		    i++;
		    j++;
		    continue;
	    }

	    //whitespace case
	    if(s[i] == ' '){
		    result[j] = '_';
		    i++;
		    j++;
		    continue;
	    }

	    //otherwise
	    else{
		    i++;
		    continue;
	    }
    }

    //null terminate result
    result[j] = 0;

    //return the new string
    return result;
}

/* TODO 21: Implement h_hash (djb2 algorithm)
 * unsigned hash = 5381;
 * For each character c in the string:
 *   hash = ((hash << 5) + hash) + c;  // hash * 33 + c
 * Return hash
 */
unsigned h_hash(const char *s) {
    //unsigned hash = 5381
    unsigned hash = 5381u;
    
    //iterate through string and do hash function
    int i;
    int len = (int)strlen(s);
    for(i=0;i<len;i++){
	    hash = ((hash << 5) + hash) + (unsigned char)s[i];
    }

    //return hash
    return hash;
}

/* TODO 22: Implement h_init
 * - Allocate buckets array using calloc (initializes to NULL)
 * - Set nbuckets field
 * - Set size to 0
 */
void h_init(Hash *h, int nbuckets) {
    //allocate buckets array using calloc
    h->buckets = (Entry**) calloc((size_t)nbuckets, sizeof(Entry*));

    //set nbuckets field
    h->nbuckets = nbuckets;

    //set size to 0
    h->size = 0;
    return;
}

/* TODO 23: Implement h_put
 * Add animalId to the list for the given key
 * 
 * Steps:
 * 1. Compute bucket index: idx = h_hash(key) % nbuckets
 * 2. Search the chain at buckets[idx] for an entry with matching key
 * 3. If found:
 *    - Check if animalId already exists in the vals list
 *    - If yes, return 0 (no change)
 *    - If no, add animalId to vals.ids array (resize if needed), return 1
 * 4. If not found:
 *    - Create new Entry with strdup(key)
 *    - Initialize vals with initial capacity (e.g., 4)
 *    - Add animalId as first element
 *    - Insert at head of chain (buckets[idx])
 *    - Increment h->size
 *    - Return 1
 */
int h_put(Hash *h, const char *key, int animalId) {
    //compute bucket idx
    int idx = (int)(h_hash(key) % (unsigned)h->nbuckets);

    //Search the chain at buckets[idx] for an entry with matching key
    Entry* temp = h->buckets[idx];
    while(temp){
	    //if found:
	    if (!strcmp(key,temp->key)){
		    //check if animalID already exists in the vals list
		    int j;
		    for(j = 0;j < temp->vals.count;j++){
			    if(animalId == temp->vals.ids[j]){
				    //if yes, return 0 (no change)
				    return 0;
			    }
		    }
		    //If no, add animalId to vals.ids array (resize if needed), return 1
		    if(temp->vals.count >= temp->vals.capacity){
			    temp->vals.capacity = temp->vals.capacity * 2;
			    temp->vals.ids = (int*) realloc(temp->vals.ids, sizeof(int) * temp->vals.capacity);
		    }
		    temp->vals.ids[temp->vals.count] = animalId;
		    temp->vals.count = temp->vals.count + 1;
		    return 1;
	    }
	    temp = temp->next;
    }
    //if not found
    //Create new Entry with strdup(key)
    Entry* new = (Entry*) malloc(sizeof(Entry));
    new->key = strdup(key);

    //Initialize vals with initial capacity (e.g., 4)
    new->vals.capacity = 4;
    new->vals.count =0;
    new->vals.ids = (int*) malloc(new->vals.capacity * sizeof(int));

    //Add animalId as first element
    new->vals.ids[0] = animalId;
    new->vals.count = new->vals.count + 1;

    //Insert at head of chain (buckets[idx])
    new->next = h->buckets[idx]->next;
    h->buckets[idx] = new;

    //Increment h->size
    h->size = h->size + 1;

   //return 1 
    return 1;
}

/* TODO 24: Implement h_contains
 * Check if the hash table contains the given key-animalId pair
 * 
 * Steps:
 * 1. Compute bucket index
 * 2. Search the chain for matching key
 * 3. If found, search vals.ids array for animalId
 * 4. Return 1 if found, 0 otherwise
 */
int h_contains(const Hash *h, const char *key, int animalId) {
    //compute bucket index
    int idx = (int)(h_hash(key) % (unsigned)h->nbuckets);

    //search the chain for matching key
    Entry* temp = h->buckets[idx];
    while(temp){
	if(!strcmp(key, temp->key)){
		//if found search vals.ids array for animalId
		int j;
		for(j = 0;j < temp->vals.count;j++){
                        if(animalId == temp->vals.ids[j]){
                               //if found return 1
                               return 1;
                	}
                }
		return 0;
	}
	temp = temp->next;
    }
    //return 0 otherwise
    return 0;
}

/* TODO 25: Implement h_get_ids
 * Return pointer to the ids array for the given key
 * Set *outCount to the number of ids
 * Return NULL if key not found
 * 
 * Steps:
 * 1. Compute bucket index
 * 2. Search chain for matching key
 * 3. If found:
 *    - Set *outCount = vals.count
 *    - Return vals.ids
 * 4. If not found:
 *    - Set *outCount = 0
 *    - Return NULL
 */
int *h_get_ids(const Hash *h, const char *key, int *outCount) {
    //compute bucket index
    int idx = (int)(h_hash(key) % (unsigned)h->nbuckets);
    //search chain for matching key
    Entry* temp = h->buckets[idx];
    while(temp){
	    if(!strcmp(key, temp->key)){
		    //if found
		    //set *outCount = vals.count
		    *outCount = temp->vals.count;
		    //return vals.ids
		    return temp->vals.ids;
	    }
	    temp = temp->next;
    }
    //if not found
    //set *outCount = 0
    *outCount = 0;
    //return NULL
    return NULL;
}

/* TODO 26: Implement h_free
 * Free all memory associated with the hash table
 * 
 * Steps:
 * - For each bucket:
 *   - Traverse the chain
 *   - For each entry:
 *     - Free the key string
 *     - Free the vals.ids array
 *     - Free the entry itself
 * - Free the buckets array
 * - Set buckets to NULL, size to 0
 */
void h_free(Hash *h) {
    //for each bucket
    int i;
    for(i = 0; i < h->nbuckets; i++){
	    //Traverse the chain
	    //for each entry
	    Entry* temp = h->buckets[i];
	    while(temp){
		    Entry* next = temp->next;
		    //free the key string
		    free(temp->key);
		    //free the vals.id array
		    free(temp->vals.ids);
		    //free the entry itself
		    free(temp);
		    temp = next;
	    }
	    h->buckets[i] = NULL;
    }
    //free the buckets array
    free(h->buckets);
    //set buckets to NULL, size to 0
    h->buckets = NULL;
    h->size = 0;
    h->nbuckets = 0;
}
