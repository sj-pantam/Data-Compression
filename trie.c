#include <stdint.h>
#include <stdio.h>
#include "trie.h"
#include "code.h"
#include <stdlib.h>

TrieNode *trie_node_create(uint16_t index) {
    TrieNode *n = (TrieNode *) malloc(sizeof(TrieNode));
    if (!n) { // check if TrieNode is created
        return NULL;
    }
    for (int i = 0; i < 256; i++) { // makes sure each child node pointer is NULL
        n->children[i] = NULL;
    }
    n->code = index; //nodes code is set to index
    return n;
}

void trie_node_delete(TrieNode *n) { // deconstructor of TrieNode
    free(n); // frees allocated memory
}

TrieNode *trie_create(void) {
    return trie_node_create(EMPTY_CODE); // initializes a TrieNode with root EMPTY_CODE
}

void trie_reset(TrieNode *root) { //resets the TrieNode to the root
    for (int i = 0; i < 256; i++) {
        if (root->children[i]) {
            trie_delete(root->children[i]); // delete its children
            root->children[i] = NULL; // make sure they are null
        }
    }
}

void trie_delete(TrieNode *n) { // deletes the trie and its sub trie or children
    for (int i = 0; i < 256; i++) {
        if (n->children[i] != NULL) { // check if child is not NULL
            trie_delete(n->children[i]); // recursively call on n's children
        }
    }
    trie_node_delete(n); // free
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    return n->children[sym]; // return the symbol sym
}