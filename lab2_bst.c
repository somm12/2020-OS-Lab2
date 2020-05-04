/*
 *	Operating System Lab
 *	    Lab2 (Synchronization)
 *	    Studend id : 32182490
 *	    Student name : An Somin
 *	    Student id : 32182110
 *	    Student name : Seo Museong
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

void lab2_node_inorder(lab2_tree *tree, lab2_node *cur) { // 중위순회 구현 : 왼쪽 하위 트리 -> 루트 -> 오른쪽 하위트리 순서로 이동
	if (cur == NULL)return;
	lab2_node_inorder(tree, cur->left);
	lab2_node_inorder(tree, cur->right);
	
}

int lab2_node_print_inorder(lab2_tree *tree) { // lab2_node_inorder 함수와 나눠서 구현
	lab2_node_inorder(tree, tree->root);
	return LAB2_SUCCESS;
}


lab2_tree *lab2_tree_create() {// 트리생성 함수
	
	lab2_tree *tree = (lab2_tree *)malloc(sizeof(lab2_tree));
	tree->root = NULL;
	return tree;	
}


lab2_node  *lab2_node_create(int key) {// key값을 인자로 가지는  노드생성함수
	
	lab2_node *node = (lab2_node *)malloc(sizeof(lab2_node));
	node->left = NULL;
	node->right = NULL;
	node->key = key;
	return node;
}

_Bool search_key(lab2_tree* tree, int key){//트리에 해당 key값이 존재하는지 확인하기 위한 함수 
	lab2_node* cur = tree->root;
	while(1){                //cur 포인터를 이용해서 해당key값과 비교하면서 while문으로 오른쪽 또는 왼쪽 서브트리로 이동
		if(cur->key == key){ // key값을 찾았을 때 종료
			return true;
		}
		else if(cur->key < key){// key값이 더 클때 오른쪽으로 이동
			if (cur->right == NULL)// NULL이 나올 때 까지 key값이 없다면 false로 종료
				return false;
			cur = cur->right;
		}
		else{
			if (cur->left == NULL) //key값이 더 작을 때 왼쪽으로 이동 후 NULL이 나올 때까지 key값이 없다면 false로 종료
				return false;
			cur = cur->left;
		}
	}
}

int lab2_node_insert(lab2_tree *tree, lab2_node *new_node){ // 트리에 노드를 추가하는 함수	
	if (tree->root == NULL) {// 루트 NULL값일 때 해당자리에 추가
		tree->root = new_node;
		return LAB2_SUCCESS;
	}
	if (search_key(tree,new_node->key)) {// 해당 key값이 이미 존재할 때  error 반환
		return LAB2_ERROR;
	}
	lab2_node* cur = tree->root;       // 노드를 추가할 위치를 가리키는 포인터 cur 선언
	while (1) {
		if (cur->key < new_node->key) {// key값이 해당 노드 보다 크다면  cur->right이 NULL 이 될 때까지 오른쪽으로 이동
			if (cur->right == NULL) {
				cur->right = new_node;// 노드추가
				break;
			}
			cur = cur->right;
		}
		else {						  // key값이 해당 노드 보다 작다면 cur->left가 NULL 이 될 때까지 왼쪽으로 이동
			if (cur->left == NULL) {
				cur->left = new_node;// 노드추가
				break;
			}
			cur = cur->left;
		}
	}
       return LAB2_SUCCESS;
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
				break;
			}
			cur = cur->right;
		}
		else {
			if (cur->left == NULL) {
				pthread_mutex_lock(&lock);
				cur->left = new_node;
				pthread_mutex_unlock(&lock);
				break;
			}
			cur = cur->left;
		}
	}
       return LAB2_SUCCESS;

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
				 break;
			}
			cur = cur->right;
		}
		else {
			if (cur->left == NULL) {
				cur->left = new_node;
				 break;
			}
			cur = cur->left;
		}
	}
	   pthread_mutex_unlock(&lock);
       return LAB2_SUCCESS;

}

int lab2_node_remove(lab2_tree *tree, int key) {
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

	if(cur->left == NULL && cur->right == NULL){ // if it is a terminal node
		if (cur == tree->root){
			tree->root = NULL;
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
		}
		else if(parent->left == cur){
			parent->left = NULL;
		}
		else if(parent->right == cur)
		{
			parent->right = NULL;
		}

	}

	else if(cur->left == NULL || cur->right == NULL){ // only has one child
		child = (cur->left != NULL)? cur->left : cur->right;
		if (cur == tree->root){
			tree->root = child;
		}
		else if(parent->left == cur)
		{
			parent->left = child;
		}
		else
		{
			parent->right = child;
		}

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

		if (parent->left == cur2){
			parent->left = cur2->right;
		}
		else
		{
			
			parent->right = cur2->right;
		}

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

int lab2_tree_delete(lab2_tree *tree) {// 트리 삭제함수
	free(tree);
	return LAB2_SUCCESS;
}

int lab2_node_delete(lab2_node *node) {// 노드 삭제함수
	free(node);
	return LAB2_SUCCESS;
}
