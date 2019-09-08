//Rolling my own balanced binary search tree
//Specifically I need to map socket file descriptors to indices in some way
  //i.e client #25 has socket 40162
//BBSTs are guaranteed to be O(log_2 n) time, which is great

#include <stddef.h>

typedef struct bbst_node {
  int value;
  struct bbst_node* parent;
  struct bbst_node* son;       //left and right child nodes, respectively
  struct bbst_node* daughter;
} bbst_node_t;


typedef struct bbst {
  int size;
  bbst_node_t* root;
} bbst_t;


int bbst_add(bbst_t* tree, int value);
int bbst_remove(bbst_t* tree, int value);
bbst_node_t* bbst_search(bbst_t* tree, int value);
void bbst_inorder_traverse(bbst_t* tree, void (*function)());
