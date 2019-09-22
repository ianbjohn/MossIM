#include "bbst.h"


int create_bbst(bbst_t* tree) {
  tree = (bbst_t* ) malloc(sizeof(bbst_t));
  if (tree == NULL)
    return -1;

  return 0;
}


int bbst_add(bbst_t* tree, int value) {
  //nothing here yet, except add the value to the tree's root
  if (tree->root == NULL) {
    if ((tree->root = (bbst_node_t* ) malloc(sizeof(bbst_node_t))) == NULL)
      return -1;
  }

  tree->root->value = value;
  tree->size = 1;

  return 0;
}


int bbst_remove(bbst_t* tree, int value) {
  //nothing here yet, except remove the tree's rooot
  if (tree->root != NULL) {
    tree->size = 0;
    free(tree->root);
    tree->root = NULL;
    //this is terrible practice, since we're freeing the root without freeing any of its children,
    //but that won't matter once we actually implement this correctly.
  }

  return 0;
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
