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

void lab2_node_inorder(lab2_tree *tree, lab2_node *cur) { // inorder function
	// You need to implement lab2_node_print_inorder function.
	if (cur != NULL) {
		lab2_node_inorder(tree, cur->left);
		lab2_node_inorder(tree, cur->right);
	}
}

int lab2_node_print_inorder(lab2_tree *tree) { //inorder print function
	lab2_node_inorder(tree, tree->root);
	return LAB2_SUCCESS; // return success value
}

lab2_tree *lab2_tree_create() {
	// You need to implement lab2_tree_create function.
	lab2_tree *tree = (lab2_tree *)malloc(sizeof(lab2_tree));
	tree->root = NULL;
	return tree;	
}

lab2_node * lab2_node_create(int key) {
	// You need to implement lab2_node_create function.
	lab2_node *node = (lab2_node *)malloc(sizeof(lab2_node));
	node->left = NULL;
	node->right = NULL;
	node->key = key;
	return node;
}

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
				pthread_mutex_lock(&lock);
				cur->right = new_node;
				pthread_mutex_unlock(&lock);
				return LAB2_SUCCESS;
			}
			cur = cur->right;
		}
		else {
			if (cur->left == NULL) {
				pthread_mutex_lock(&lock);
				cur->left = new_node;
				pthread_mutex_unlock(&lock);
				return LAB2_SUCCESS;
			}
			cur = cur->left;
		}
	}   
}


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

int lab2_node_remove(lab2_tree *tree, int key) {
	// You need to implement lab2_node_remove function.
	lab2_node* cur = tree->root; // 현재 위치를 알려주는 변수 cur
	lab2_node* parent = NULL;
	lab2_node* par_parent = NULL;
	lab2_node* child = NULL;
	lab2_node* cur2 = NULL;
	if(!search_key(tree, key)){ // 찾는 값인 key가 tree에 없으면 error 리턴
		return LAB2_ERROR;
	}

	while(cur->key != key){ // key 의 값을 key 로 가지는 노드를 찾는 과정
		if(key > cur->key) // 현재 노드의 key 값보다 더 크다면, 오른쪽으로 이동
		{
			par_parent = parent;
			parent = cur;
			cur = cur->right; 
		}
		else{ // 작다면, 왼쪽으로 이동
			par_parent = parent;
			parent = cur;
			cur = cur->left;결
		}
	}

	if(cur->left == NULL && cur->right == NULL){ // 자식 노드가 없을 때
		if (cur == tree->root){ // 노드를 바로 null 값으로 바꿔주면 된다
			tree->root = NULL;
			return LAB2_SUCCESS;
		}
		else if(parent->left == cur)
			parent->left = NULL;
		else if(parent->right == cur)
			parent->right = NULL;
	}

	else if(cur->left == NULL || cur->right == NULL){ // 자식 노드가 하나일 때
		child = (cur->left != NULL)? cur->left : cur->right; // cur 노드의 자식 노드를 child 로 선언
		if (cur == tree->root) // cur 노드의 부모 노드가 없으면 root 로 선언
			tree->root = child;
		else if(parent->left == cur) // 부모 노드인 parent 가 존재하면 parent 와 연결
			parent->left = child;
		else
			parent->right = child;
	}	

	else if (cur->left != NULL && cur->right != NULL) // 자식 노드가 두 개일 때 (오른쪽 자식 중 가장 작은 노드를 위로 올린다)
	{
		parent = cur;
		cur2 = cur->right;
		while (cur2->left != NULL) // 왼쪽의 노드가 존재하지 않을 때까지 오른쪽으로 계속 이동 (오른쪽 자식 중 가장 작은 노드 탐색)
		{
			parent = cur2;
			cur2 = cur2->left;
		}

		if (parent->left == cur2) // 그 노드를 부모 노드와 연결한다
			parent->left = cur2->right;
		else
			parent->right = cur2->right;


		cur->key = cur2->key;
		cur = cur2; // cur2 노드와 키 값을 다시 cur 로 초기화
		free(cur);

	}
	return LAB2_SUCCESS;
}

int lab2_node_remove_fg(lab2_tree *tree, int key) {
	// You need to implement lab2_node_remove function.
	lab2_node* cur = tree->root;
	lab2_node* parent = NULL;
	lab2_node* par_parent = NULL;
	lab2_node* child = NULL;
	lab2_node* cur2 = NULL;
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

	else if (cur->left != NULL && cur->right != NULL)
	{   
		parent = cur;
		cur2 = cur->right;
		while (cur2->left != NULL)
		{
			parent = cur2;
			cur2 = cur2->left;
		}   

		if (parent->left == cur2)
			parent->left = cur2->right;
		else
			parent->right = cur2->right;


		cur->key = cur2->key;
		cur = cur2;
		free(cur);
	}   
	pthread_mutex_unlock(&lock);
	return LAB2_SUCCESS;
}

int lab2_node_remove_cg(lab2_tree *tree, int key) {
	// You need to implement lab2_node_remove function.
	pthread_mutex_lock(&lock);
	lab2_node* cur = tree->root;
	lab2_node* parent = NULL;
	lab2_node* par_parent = NULL;
	lab2_node* child = NULL;
	lab2_node* cur2 = NULL;
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
		if (cur == tree->root){
			tree->root = NULL;
			pthread_mutex_unlock(&lock);
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
	else if (cur->left != NULL && cur->right != NULL)
	{   
		parent = cur;
		cur2 = cur->right;
		while (cur2->left != NULL)
		{
			parent = cur2;
			cur2 = cur2->left;
		}   

		if (parent->left == cur2)
			parent->left = cur2->right;
		else
			parent->right = cur2->right;


		cur->key = cur2->key;
		cur = cur2;
		free(cur);
	}   
	pthread_mutex_unlock(&lock);
	return LAB2_SUCCESS;
}

int lab2_tree_delete(lab2_tree *tree) {
	// You need to implement lab2_tree_delete function.
	free(tree);
	return LAB2_SUCCESS; // success
}

int lab2_node_delete(lab2_node *node) {
	// You need to implement lab2_node_delete function.
	free(node);
	return LAB2_SUCCESS;
}
