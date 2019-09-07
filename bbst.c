#include "bbst.h"

int bbst_add(bbst_t* tree, int value) {
  //nothing here yet
  return -1;
}


int bbst_remove(bbst_t* tree, int value) {
  //nothing here yet
  return -1;
}


bbst_node_t* bbst_search(bbst_t* tree, int value) {
  if (tree == NULL) return NULL;

  bbst_node_t* node = tree.root;
  while (node != NULL) {
    if (value == node.value) return node;
    else if (value < node.value)
      node = node.son;
    else
      node = node.daughter;
  }

  return node;
}
