#include "bbst.h"


int create_bbst(bbst_t* tree) {
  tree = (bbst_t* ) malloc(sizeof(bbst_t));
  if (tree == NULL)
    return -1;

  return 0;
}


int bbst_add(bbst_t* tree, int value) {
  //nothing here yet, except add the value to the tree's root
  if (tree->root == NULL)
    tree->root = (bbst_node_t* ) malloc(sizeof(bbst_node_t));
  tree->root->value = value;

  return -1;
}


int bbst_remove(bbst_t* tree, int value) {
  //nothing here yet
  return -1;
}


//might not be necessary for what little we really need to do in the application, but have it here just to be safe
bbst_node_t* bbst_search(bbst_t* tree, int value) {
  if (tree == NULL) return NULL;

  bbst_node_t* node = tree->root;
  while (node != NULL) {
    if (value == node->value) return node;
    else if (value < node->value)
      node = node->son;
    else
      node = node->daughter;
  }

  return node;
}
