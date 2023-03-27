#include "word.h"
#include "code.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Word *word_create(uint8_t *syms, uint32_t len) {
    Word *w = (Word *) malloc(sizeof(Word)); // allocate memory for word
    if (!w) { // check if word was created
        return NULL;
    }
    w->syms = (uint8_t *) malloc(sizeof(uint8_t) * len + 1); // allocate memory for array of symbols
    if (!w->syms) { // check if successful
        free(w);
        return NULL;
    }
    for (uint32_t i = 0; i < len; i += 1) { // copy syms to w->syms
        w->syms[i] = syms[i];
    }
    w->len = len; // set w->len to len
    return w; // return Word *
}

// makes a new word with Word w and a symbol sym appended
Word *word_append_sym(Word *w, uint8_t sym) {
    Word *w1 = (Word *) calloc(1, sizeof(Word)); // new word
    if (!w1) { // check if successful
        return NULL;
    }
    w1->len = w->len + 1; // new word length
    w1->syms = (uint8_t *) malloc((w1->len + 1) * sizeof(uint8_t)); // allocate memory for sym
    if (!w1->syms) { // check if successful
        free(w1);
        return NULL;
    }

    memcpy(w1->syms, w->syms, w->len); // copy contents using memcpy
    w1->syms[w->len] = sym; // append the symbol sym to the last position of array w1->syms
    return w1;
}

void word_delete(Word *w) { // destructor for Word
    free(w->syms);
    free(w);
}

WordTable *wt_create(void) { // create a WordTable
    WordTable *wt = (WordTable *) calloc(MAX_CODE, sizeof(Word *));
    if (!wt) { // check if successful
        return NULL;
    }
    wt[EMPTY_CODE] = (Word *) calloc(1, sizeof(Word)); // initialize at index EMPTY_CODE

    return wt;
}

// code below inspired by Gabe's session and Eugene's session

void wt_reset(WordTable *wt) { // resets elements in WordTable wt
    for (uint32_t i = START_CODE; i < MAX_CODE; i++) {
        if (wt[i]) {
            word_delete(wt[i]); // calls word_delete to delete
            wt[i] = NULL; //words in table are set to NULL
        }
    }
}

void wt_delete(WordTable *wt) { // deletes and frees memory of WordTable
    for (uint32_t i = 0; i < MAX_CODE; i++) {
        if (wt[i]) {
            word_delete(wt[i]);
            wt[i] = NULL;
        }
    }
    free(wt);
    wt = NULL;
}