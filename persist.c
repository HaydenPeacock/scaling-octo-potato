#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "lab5.h"

extern Node *g_root;

#define MAGIC 0x41544C35  /* "ATL5" */
#define VERSION 1

typedef struct {
    Node *node;
    int id;
} NodeMapping;

/* TODO 27: Implement save_tree
 * Save the tree to a binary file using BFS traversal
 * 
 * Binary format:
 * - Header: magic (4 bytes), version (4 bytes), nodeCount (4 bytes)
 * - For each node in BFS order:
 *   - isQuestion (1 byte)
 *   - textLen (4 bytes)
 *   - text (textLen bytes, no null terminator)
 *   - yesId (4 bytes, -1 if NULL)
 *   - noId (4 bytes, -1 if NULL)
 * 
 * Steps:
 * 1. Return 0 if g_root is NULL
 * 2. Open file for writing binary ("wb")
 * 3. Initialize queue and NodeMapping array
 * 4. Use BFS to assign IDs to all nodes:
 *    - Enqueue root with id=0
 *    - Store mapping[0] = {g_root, 0}
 *    - While queue not empty:
 *      - Dequeue node and id
 *      - If node has yes child: add to mappings, enqueue with new id
 *      - If node has no child: add to mappings, enqueue with new id
 * 5. Write header (magic, version, nodeCount)
 * 6. For each node in mapping order:
 *    - Write isQuestion, textLen, text bytes
 *    - Find yes child's id in mappings (or -1)
 *    - Find no child's id in mappings (or -1)
 *    - Write yesId, noId
 * 7. Clean up and return 1 on success
 */
int save_tree(const char *filename) {
    // TODO: Implement this function
    // This is complex - break it into smaller steps
    // You'll need to use the Queue functions you implemented
    //1. Return 0 if g_root is NULL
    if(g_root == NULL){
	    return 0;
    }

    //2. Open file for writing binary ("wb")
    FILE *fptr;
    fptr = fopen(filename, "wb");
    if(fptr == NULL) return 0;

    //3. Initialize queue and NodeMapping array
    Queue q_tree;
    q_init(&q_tree);
    NodeMapping* nodeMap = NULL;
    int mapCount = 0;
    int mapSize = 0;
    
    //4. Use BFS to assign IDs to all nodes:
    //Enqueue root with id=0
    q_enqueue(&q_tree, g_root, 0);

    //Store mapping[0] = {g_root, 0}
    nodeMap = (NodeMapping*) malloc(2 * sizeof(NodeMapping));
    nodeMap[0].node = g_root;
    nodeMap[0].id = 0;
    mapCount++;
    mapSize = 2;

    //While queue not empty:
    while(!(q_empty(&q_tree))){
		    //Dequeue node and id
		    Node* temp;
		    int id;
		    q_dequeue(&q_tree, &temp, &id);

		    //If node has yes child: add to mappings, enqueue with new id
		    if(temp->yes != NULL){
	    		q_enqueue(&q_tree, temp->yes, mapCount);
			if(mapCount >= mapSize){
				int newCap = mapSize * 2;
				NodeMapping *tmp = realloc(nodeMap, newCap * sizeof(NodeMapping));
				if (!tmp) {
    					q_free(&q_tree);
    					free(nodeMap);
    					fclose(fptr);
    					return 0;
				}
				nodeMap = tmp;
				mapSize = newCap;
			}
			nodeMap[mapCount].node = temp->yes;
			nodeMap[mapCount].id = mapCount;
			mapCount++;
		    }

		    //If node has no child: add to mappings, enqueue with new id
		    if(temp->no != NULL){
			q_enqueue(&q_tree, temp->no, mapCount);
			if(mapCount >= mapSize){
				nodeMap = (NodeMapping*) realloc(nodeMap, mapSize*2*sizeof(NodeMapping));
				mapSize *= 2;
			}
			nodeMap[mapCount].node = temp->no;
			nodeMap[mapCount].id = mapCount;
			mapCount++;
		    }
    	}

	//Write header (magic, version, nodeCount)
	uint32_t magic = MAGIC;
	uint32_t version = VERSION;
	uint32_t nodeCount = (uint32_t) mapCount;
	fwrite(&magic, sizeof(uint32_t), 1, fptr);
	fwrite(&version, sizeof(uint32_t), 1, fptr);
	fwrite(&nodeCount, sizeof(uint32_t), 1, fptr);

	//6. For each node in mapping order:
	Node* temp;
	for(int i = 0; i < mapCount; i++){
		temp = nodeMap[i].node;

		//Write isQuestion, textLen, text bytes
		uint8_t isQuestion = temp->isQuestion;
		uint32_t textLen = strlen(temp->text);
		if(fwrite(&isQuestion, sizeof(uint8_t), 1, fptr) != 1){
			q_free(&q_tree);
			free(nodeMap);
			fclose(fptr);
			return 0;
		}
		if(fwrite(&textLen, sizeof(uint32_t), 1, fptr) != 1){
			q_free(&q_tree);
			free(nodeMap);
			fclose(fptr);
			return 0;
		}
		if(textLen > 0){
			if(fwrite(temp->text, 1, textLen, fptr) != 1){
				q_free(&q_free);
				free(nodeMap);
				fclose(fptr);
				return 0;
			}
		}

		//Find yes child's id in mappings (or -1)
		int32_t yesId = -1;
		for(int j = 0; j < mapCount; j++){
			if(temp->yes == nodeMap[j].node){
				yesId = (int32_t) j;
				break;
			}
		}

		//Find no child's id in mappings (or -1)
		int32_t noId = -1;
                for(int j = 0; j < mapCount; j++){
                        if(temp->no == nodeMap[j].node){
                                noId = (int32_t) j;
				break;
                        }
                }

		//Write yesId, noId
		if(fwrite(&yesId, sizeof(int32_t), 1, fptr) != 1 || fwrite(&noId, sizeof(int32_t), 1, fptr) != 1){
			q_free(&q_tree);
			free(nodeMap);
			fclose(fptr);
			return 0;
		}
	}

	//Clean up and return 1 on success
	q_free(&q_tree);
	free(nodeMap);
	fclose(fptr);

    	return 1;
}

/* TODO 28: Implement load_tree
 * Load a tree from a binary file and reconstruct the structure
 * 
 * Steps:
 * 1. Open file for reading binary ("rb")
 * 2. Read and validate header (magic, version, count)
 * 3. Allocate arrays for nodes and child IDs:
 *    - Node **nodes = calloc(count, sizeof(Node*))
 *    - int32_t *yesIds = calloc(count, sizeof(int32_t))
 *    - int32_t *noIds = calloc(count, sizeof(int32_t))
 * 4. Read each node:
 *    - Read isQuestion, textLen
 *    - Validate textLen (e.g., < 10000)
 *    - Allocate and read text string (add null terminator!)
 *    - Read yesId, noId
 *    - Validate IDs are in range [-1, count)
 *    - Create Node and store in nodes[i]
 * 5. Link nodes using stored IDs:
 *    - For each node i:
 *      - If yesIds[i] >= 0: nodes[i]->yes = nodes[yesIds[i]]
 *      - If noIds[i] >= 0: nodes[i]->no = nodes[noIds[i]]
 * 6. Free old g_root if not NULL
 * 7. Set g_root = nodes[0]
 * 8. Clean up temporary arrays
 * 9. Return 1 on success
 * 
 * Error handling:
 * - If any read fails or validation fails, goto load_error
 * - In load_error: free all allocated memory and return 0
 */
int load_tree(const char *filename) {
    // TODO: Implement this function
    // This is the most complex function in the lab
    // Take it step by step and test incrementally
    //Open file for reading binary ("rb")
    FILE* fptr;
    fptr = fopen(filename, "rb");
    if(fptr == NULL) return 0;

    //Read and validate header (magic, version, count)
    uint32_t magic;
    uint32_t version;
    uint32_t count;

    fread(&magic, sizeof(uint32_t), 1, fptr);
    fread(&version, sizeof(uint32_t), 1, fptr);
    fread(&count, sizeof(uint32_t), 1, fptr);

    if(magic != MAGIC || version != VERSION || count == 0){
	    fclose(fptr);
	    return 0;
    }

    int nodeCount = (int) count;

    //3. Allocate arrays for nodes and child IDs:
    //Node **nodes = calloc(nodeCount, sizeof(Node*))
    Node **nodes = calloc(nodeCount, sizeof(Node*));

    //int32_t *yesIds = calloc(count, sizeof(int32_t))
    int32_t *yesIds = calloc(nodeCount, sizeof(int32_t));

    //int32_t *noIds = calloc(count, sizeof(int32_t))
    int32_t *noIds = calloc(nodeCount, sizeof(int32_t));

    if(!nodes || !yesIds || !noIds){
	    goto load_error;
    }

    //Read each node:
    for(int i = 0; i < nodeCount; i++){
	    //Read isQuestion, textLen
	    uint8_t isQ;
	    uint32_t textL;
	    if(fread(&isQ, sizeof(uint8_t), 1, fptr) != 1)
		    goto load_error;
	    if(fread(&textL, sizeof(uint32_t), 1, fptr) != 1)
		    goto load_error;

	    //Validate textLen (e.g., < 10000)
	    if(textL >= 10000){
		    goto load_error;
	    }

	    int length = (int) textL;
	    //Allocate and read text string (add null terminator!)
	    char* text = NULL;
	    text = (char*) malloc(length + 1);
	    if(fread(text, 1, length, fptr) != (size_t) length)
		    goto load_error;
	    text[length] = '\0';

	    //Read yesId, noId
	    int32_t yesId;
	    int32_t noId;
	    if(fread(&yesId, sizeof(int32_t), 1, fptr) != 1)
		    goto load_error;
	    if(fread(&noId, sizeof(int32_t), 1, fptr) != 1)
		    goto load_error;

	    //Validate IDs are in range [-1, count)
	    if(yesId < -1 || yesId >= nodeCount){
		    free(text);
		    goto load_error;
	    }

	    if(noId < -1 || noId >= nodeCount){
		    free(text);
		    goto load_error;
	    }

	    //Create Node and store in nodes[i]
	    Node* n = (Node*) malloc(sizeof(Node));
	    n->isQuestion = (isQ ? 1: 0);
	    n->text = text;
	    n->yes = NULL;
	    n->no = NULL;
	    nodes[i] = n;
	    yesIds[i] = yesId;
	    noIds[i] = noId;
    }

    //5. Link nodes using stored IDs:
    //For each node i:
    for(int i = 0; i < nodeCount; i++){
	    //If yesIds[i] >= 0: nodes[i]->yes = nodes[yesIds[i]]
	    if(yesIds[i] >= 0){
		    nodes[i]->yes = nodes[yesIds[i]];
	    }
	    //If noIds[i] >= 0: nodes[i]->no = nodes[noIds[i]]
	    if(noIds[i] >= 0){
		    nodes[i]->no = nodes[noIds[i]];
	    }
    }

    //Free old g_root if not NULL
    if(g_root != NULL){
	    free_tree(g_root);
    }

    //7. Set g_root = nodes[0]
    g_root = nodes[0];

    //Clean up temporary arrays
    fclose(fptr);
    free(nodes);
    free(yesIds);
    free(noIds);
    return 1;

    //If any read fails or validation fails, goto load_error
    //In load_error: free all allocated memory and return 0
load_error:
    if(nodes){
	    for(int i = 0; i < nodeCount; i++){
		    if(nodes[i]){
			    nodes[i]->yes = NULL;
			    nodes[i]->no = NULL;
			    free(nodes[i]->text);
			    free(nodes[i]);
		    }
	    }
    }

    fclose(fptr);
    free(nodes);
    free(yesIds);
    free(noIds);
    return 0;
}
