#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "lab5.h"

extern Node *g_root;
extern EditStack g_undo;
extern EditStack g_redo;
extern Hash g_index;

/* TODO 31: Implement play_game
 * Main game loop using iterative traversal with a stack
 * 
 * Key requirements:
 * - Use FrameStack (NO recursion!)
 * - Push frames for each decision point
 * - Track parent and answer for learning
 * 
 * Steps:
 * 1. Initialize and display game UI
 * 2. Initialize FrameStack
 * 3. Push root frame with answeredYes = -1
 * 4. Set parent = NULL, parentAnswer = -1
 * 5. While stack not empty:
 *    a. Pop current frame
 *    b. If current node is a question:
 *       - Display question and get user's answer (y/n)
 *       - Set parent = current node
 *       - Set parentAnswer = answer
 *       - Push appropriate child (yes or no) onto stack
 *    c. If current node is a leaf (animal):
 *       - Ask "Is it a [animal]?"
 *       - If correct: celebrate and break
 *       - If wrong: LEARNING PHASE
 *         i. Get correct animal name from user
 *         ii. Get distinguishing question
 *         iii. Get answer for new animal (y/n for the question)
 *         iv. Create new question node and new animal node
 *         v. Link them: if newAnswer is yes, newQuestion->yes = newAnimal
 *         vi. Update parent pointer (or g_root if parent is NULL)
 *         vii. Create Edit record and push to g_undo
 *         viii. Clear g_redo stack
 *         ix. Update g_index with canonicalized question
 * 6. Free stack
 */
void play_game() {
    clear();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(0, 0, "%-80s", " Playing 20 Questions");
    attroff(COLOR_PAIR(5) | A_BOLD);
    
    mvprintw(2, 2, "Think of an animal, and I'll try to guess it!");
    mvprintw(3, 2, "Press any key to start...");
    refresh();
    getch();
    
    // TODO: Implement the game loop
    // Initialize FrameStack
    // Push root
    // Loop until stack empty or guess is correct
    // Handle question nodes and leaf nodes differently
    
    FrameStack stack;
    fs_init(&stack);
    
    // TODO: Your implementation here
    
    fs_free(&stack);
}

/* TODO 32: Implement undo_last_edit
 * Undo the most recent tree modification
 * 
 * Steps:
 * 1. Check if g_undo stack is empty, return 0 if so
 * 2. Pop edit from g_undo
 * 3. Restore the tree structure:
 *    - If edit.parent is NULL:
 *      - Set g_root = edit.oldLeaf
 *    - Else if edit.wasYesChild:
 *      - Set edit.parent->yes = edit.oldLeaf
 *    - Else:
 *      - Set edit.parent->no = edit.oldLeaf
 * 4. Push edit to g_redo stack
 * 5. Return 1
 * 
 * Note: We don't free newQuestion/newLeaf because they might be redone
 */
int undo_last_edit() {
    //Check if g_undo stack is empty, return 0 if so.
    if (es_empty(&g_undo)){
	    return 0;
    }

    //pop edit from g_undo
    Edit temp = es_pop(&g_undo);

    //restore the tree structure:
    //if edit.parent is Null set g_root = edit.oldLeaf
    if(temp.parent == NULL){
	    g_root = temp.oldLeaf;
    }

    //else if edit was yeschild, set edit.parent->yes = edit.oldLeaf
    else if(temp.wasYesChild){
	    temp.parent->yes = temp.oldLeaf;
    }

    //else set edit.parent->no = edit.oldLeaf
    else{
	    temp.parent->no = temp.oldLeaf;
    }

    //push edit to g_redo stack
    es_push(&g_redo, temp);
    return 1;
}

/* TODO 33: Implement redo_last_edit
 * Redo a previously undone edit
 * 
 * Steps:
 * 1. Check if g_redo stack is empty, return 0 if so
 * 2. Pop edit from g_redo
 * 3. Reapply the tree modification:
 *    - If edit.parent is NULL:
 *      - Set g_root = edit.newQuestion
 *    - Else if edit.wasYesChild:
 *      - Set edit.parent->yes = edit.newQuestion
 *    - Else:
 *      - Set edit.parent->no = edit.newQuestion
 * 4. Push edit back to g_undo stack
 * 5. Return 1
 */
int redo_last_edit() {
    //check if g_redo stack is empty, return 0 if so
    if(es_empty(&g_redo)){
	    return 0;
    }

    //pop edit from g_redo
    Edit temp = es_pop(&g_redo);
    
    //reapply the tree modification:
    //if edit.parent is NULL set g.root = edit.newQuestion
    if(temp.parent == NULL){
	    g.root = temp.newQuestion;
    }

    //else if edit.wasYesChild set edit.parent->yes = edit.newQuestion
    else if(temp.wasYesChild){
	    temp.parent->yes = temp.newQuestion;
    }

    //else set edit.parent->no = edit.newQuestion
    else{
	    temp.parent->no = temp.newQuestion;
    }

    //push edit back to g_undo stack
    es_push(&g_undo, temp);
    return 1;
}
