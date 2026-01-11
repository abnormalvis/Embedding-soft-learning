#ifndef BINARY_TREE_H
#define BINARY_TREE_H

typedef struct binary_tree_node {
  struct binary_tree_node *left;
  struct binary_tree_node *right;
  struct binary_tree_node *parent;
} binary_tree_node;

binary_tree_node *create_binary_tree_node(int value);
#endif // !BINARY_TREE_H