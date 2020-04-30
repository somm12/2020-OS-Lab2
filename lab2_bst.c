/*
*	Operating System Lab
*	    Lab2 (Synchronization)
*	    Student id : 
*	    Student name : 
*
*   lab2_bst.c :
*       - thread-safe bst code.
*       - coarse-grained, fine-grained lock code
*
*   Implement thread-safe bst for coarse-grained version and fine-grained version.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#include "lab2_sync_types.h"
pthread_mutex_t lock;


/*
 * TODO
 *  Implement funtction which traverse BST in in-order
 *  
 *  @param lab2_tree *tree  : bst to print in-order. 
 *  @return                 : status (success or fail)
 */

void lab2_node_inorder(lab2_tree *tree, lab2_node *cur) // inorder function
{
    if (cur != NULL) {
        lab2_node_inorder(tree, cur->left);
        lab2_node_inorder(tree, cur->right);
    }   
}

int lab2_node_print_inorder(lab2_tree *tree) { //inorder print function
    lab2_node_inorder(tree, tree->root);
    return LAB2_SUCCESS; // return success value
}



/*
 * TODO
 *  Implement function which creates struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_tree )
 * 
 *  @return                 : bst which you created in this function.
 */
lab2_tree *lab2_tree_create() {
    // You need to implement lab2_tree_create function.
    lab2_tree *tree = (lab2_tree *)malloc(sizeof(lab2_tree));
    tree->root = NULL;
    return tree;	
}

/*
 * TODO
 *  Implement function which creates struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param int key          : bst node's key to creates
 *  @return                 : bst node which you created in this function.
 */
lab2_node * lab2_node_create(int key) {
    // You need to implement lab2_node_create function.
    lab2_node *node = (lab2_node *)malloc(sizeof(lab2_node));
    node->left = NULL;
	node->right = NULL;
	node->key = key;
    return node;
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST. 
 *  
 *  @param lab2_tree *tree      : bst which you need to insert new node.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                 : satus (success or fail)
 */

_Bool search_key(lab2_tree* tree, int key){
	lab2_node* cur = tree->root;
	while(1){
		if(cur->key == key){
			return true;
		}
		else if(cur->key < key){
			if (cur->right == NULL)
				return false;
			cur = cur->right;
		}
		else{
			if (cur->left == NULL)
				return false;
			cur = cur->left;
		}
	}
}

int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert function.
	if (tree->root == NULL) {
        tree->root = new_node;
        return LAB2_ERROR;
    }
    if (search_key(tree,new_node->key)) {
        return LAB2_ERROR;
    }
    lab2_node* cur = tree->root;
    while (1) {
        if (cur->key < new_node->key) {
            if (cur->right == NULL) {
				cur->right = new_node;
				return LAB2_SUCCESS;
            }
            cur = cur->right;
        }
        else {
			if (cur->left == NULL) {
				cur->left = new_node;
				return LAB2_SUCCESS;
            }
            cur = cur->left;
        }
    }
}
/* 
 * TODO
 *  Implement a function which insert nodes from the BST in fine-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in fine-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */
int lab2_node_insert_fg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert function.
    if (tree->root == NULL) {
        tree->root = new_node;
        return LAB2_ERROR;
    }   
    if (search_key(tree,new_node->key)) {
        return LAB2_ERROR;
    }   
    lab2_node* cur = tree->root;
    while (1) {
		if (cur->key < new_node->key) {
			if (cur->right == NULL) {
				cur->right = new_node;
				return LAB2_SUCCESS;
			}
            cur = cur->right;
        }
        else {
            if (cur->left == NULL) {
				cur->left = new_node;
				return LAB2_SUCCESS;
			}
            cur = cur->left;
        }
    }   
}

/* 
 * TODO
 *  Implement a function which insert nodes from the BST in coarse-garined manner.
 *
 *  @param lab2_tree *tree      : bst which you need to insert new node in coarse-grained manner.
 *  @param lab2_node *new_node  : bst node which you need to insert. 
 *  @return                     : status (success or fail)
 */

int lab2_node_insert_cg(lab2_tree *tree, lab2_node *new_node){
    // You need to implement lab2_node_insert function.
    pthread_mutex_lock(&lock);
	if (tree->root == NULL) {
        tree->root = new_node;
		pthread_mutex_unlock(&lock);
        return LAB2_ERROR;
    }   
    if (search_key(tree,new_node->key)) {
        pthread_mutex_unlock(&lock);
		return LAB2_ERROR;
    }   
    lab2_node* cur = tree->root;
    while (1) {
        if (cur->key < new_node->key) {
            if (cur->right == NULL) {
                cur->right = new_node;
                pthread_mutex_unlock(&lock);
                return LAB2_SUCCESS;
            }
            cur = cur->right;
        }
        else {
            if (cur->left == NULL) {
                cur->left = new_node;
                pthread_mutex_unlock(&lock);
                return LAB2_SUCCESS;
            }
            cur = cur->left;
        }
    }   
}


/* 
 * TODO
 *  Implement a function which remove nodes from the BST.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */

int lab2_node_remove(lab2_tree *tree, int key) {
	// You need to implement lab2_node_remove function.
	lab2_node* cur2 = NULL;
	lab2_node* cur = tree->root;
	lab2_node* par_parent = NULL;
	lab2_node* parent = NULL;
	lab2_node* child = NULL;
	lab2_node* left_temp = NULL;
	lab2_node* temp = NULL;
	if(!search_key(tree, key)){
		return LAB2_ERROR;
	}

	while(cur->key != key){
		if(key > cur->key)
		{
			par_parent = parent;
			parent = cur;
			cur = cur->right; 
		}
		else{
			par_parent = parent;
			parent = cur;
			cur = cur->left;
		}
	} // find the node

	if(cur->left == NULL && cur->right == NULL){ // if it is a terminal node
		if (cur == tree->root){
			tree->root = NULL;
			return LAB2_SUCCESS;
		}
		else if(parent->left == cur)
			parent->left = NULL;
		else if(parent->right == cur)
			parent->right = NULL;
	}
   
	else if(cur->left == NULL || cur->right == NULL){ // only has one child
		child = (cur->left != NULL)? cur->left : cur->right;
		if (cur == tree->root)
			tree->root = child;
		else if(parent->left == cur)
			parent->left = child;
		else
			parent->right = child;
	}	

	else if((cur->left !=NULL) && (cur->right !=NULL)) // if it has two child
	{
      parent = cur;
	  cur = cur->right;
	  temp = cur;
      
	  while(1){
		  if ((cur->right) == NULL)
			  break;
		  else{
			  cur2 = cur;
			  cur = cur->right;
		  }
	  }
	  (parent->key) = (cur->key);
	  if (cur2 != cur){
		  if ((cur->left) != NULL)
			  temp->right = cur->left;
		  else
			  temp->right = NULL;
	  }
	  else
		  cur2->left = cur->left;
	}
   return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in fine-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in fine-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */
int lab2_node_remove_fg(lab2_tree *tree, int key) {
	// You need to implement lab2_node_remove function.
	lab2_node* cur2 = NULL;
	lab2_node* cur = tree->root;
	lab2_node* par_parent = NULL;
	lab2_node* parent = NULL;
	lab2_node* child = NULL;
	lab2_node* left_temp = NULL;
	
	if(!search_key(tree, key)){
		return LAB2_ERROR;
	}
	while(cur->key != key){
		if(key > cur->key)
		{
			par_parent = parent;
			parent = cur;
			cur = cur->right; 
		}
		else{
			par_parent = parent;
			parent = cur;
			cur = cur->left;
		}
	} // find the node
	pthread_mutex_lock(&lock);
	if(cur->left == NULL && cur->right == NULL){ // if it is a terminal node
		if(parent->left == cur)
			parent->left = NULL;
		else if(parent->right == cur)
			parent->right = NULL;
	}
   
	else if(cur->left == NULL || cur->right ==NULL){ // only has one child
		child = (cur->left != NULL)? cur->left : cur->right;
		if(parent->left == cur)
			parent->left = child;
		else
			parent->right = child;
	}	

	else if(cur->left !=NULL && cur->right !=NULL) // if it has two child
	{
      cur2 = cur;
      cur2 = cur2->right;
      if(cur2->left == NULL){
         left_temp = cur->left;
         child = cur2;
         if(parent->right == cur){
            parent->right = child;
            child->left = left_temp;
         }
         else if(parent->left == cur){
            parent->left = child;
            child->left = left_temp;
         }
      }
      while(cur2->left !=NULL){
         parent = cur2;
         cur2 = cur2->left;
      }
      parent->left = NULL;
      cur->key = cur2->key;
   }
	pthread_mutex_unlock(&lock);
	return LAB2_SUCCESS;
}

/* 
 * TODO
 *  Implement a function which remove nodes from the BST in coarse-grained manner.
 *
 *  @param lab2_tree *tree  : bst tha you need to remove node in coarse-grained manner from bst which contains key.
 *  @param int key          : key value that you want to delete. 
 *  @return                 : status (success or fail)
 */

int lab2_node_remove_cg(lab2_tree *tree, int key) {
	// You need to implement lab2_node_remove function.
	pthread_mutex_lock(&lock);
	lab2_node* cur2 = NULL;
	lab2_node* cur = tree->root;
	lab2_node* par_parent = NULL;
	lab2_node* parent = NULL;
	lab2_node* child = NULL;
	lab2_node* left_temp = NULL;
	
	if(!search_key(tree, key)){
		pthread_mutex_unlock(&lock);
		return LAB2_ERROR;
	}
	while(cur->key != key){
		if(key > cur->key)
		{
			par_parent = parent;
			parent = cur;
			cur = cur->right; 
		}
		else{
			par_parent = parent;
			parent = cur;
			cur = cur->left;
		}
	} // find the node

	if(cur->left == NULL && cur->right == NULL){ // if it is a terminal node
		if(parent->left == cur)
			parent->left = NULL;
		else if(parent->right == cur)
			parent->right = NULL;
	}
   
	else if(cur->left == NULL || cur->right ==NULL){ // only has one child
		child = (cur->left != NULL)? cur->left : cur->right;
		if(parent->left == cur)
			parent->left = child;
		else
			parent->right = child;
	}	

	else if(cur->left !=NULL && cur->right !=NULL) // if it has two child
	{
      cur2 = cur;
      cur2 = cur2->right;
      if(cur2->left == NULL){
         left_temp = cur->left;
         child = cur2;
         if(parent->right == cur){
            parent->right = child;
            child->left = left_temp;
         }
         else if(parent->left == cur){
            parent->left = child;
            child->left = left_temp;
         }
      }
      while(cur2->left !=NULL){
			parent = cur2;
			cur2 = cur2->left;
		}
		parent->left = NULL;
		cur->key = cur2->key;
	}
	pthread_mutex_unlock(&lock);
	return LAB2_SUCCESS;
}

/*
 * TODO
 *  Implement function which delete struct lab2_tree
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst which you want to delete. 
 *  @return                 : status(success or fail)
 */
int lab2_tree_delete(lab2_tree *tree) {
    // You need to implement lab2_tree_delete function.
	free(tree);
	return LAB2_SUCCESS; // success
}

/*
 * TODO
 *  Implement function which delete struct lab2_node
 *  ( refer to the ./include/lab2_sync_types.h for structure lab2_node )
 *
 *  @param lab2_tree *tree  : bst node which you want to remove. 
 *  @return                 : status(success or fail)
 */
int lab2_node_delete(lab2_node *node) {
    // You need to implement lab2_node_delete function.
	free(node);
	return LAB2_SUCCESS;
}
