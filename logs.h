#ifndef LOGS_H
#define LOGS_H
#include <stdio.h>

#include "cards.h"

void print_card(Card *);
void print_shoe(Shoe *);

#ifdef LOGS_IMPLEMENTATION
// print stuff implementations, make header only lib later
const char *rank_str[] = {
  [ACE]  = "A", [TWO] = "2", [THREE] = "3", [FOUR]  = "4",
  [FIVE] = "5", [SIX] = "6", [SEVEN] = "7", [EIGHT] = "8",
  [NINE] = "9", [TEN] = "T", [JACK]  = "J", [QUEEN] = "Q", [KING] = "K"
};

const char *suit_str[] = {
  [HEARTS]   = "\033[31m♥\033[0m",
  [CLUBS]    = "\033[32m♣\033[0m",
  [DIAMONDS] = "\033[34m♦\033[0m",
  [SPADES]   = "\033[90m♠\033[0m"
};

void print_card(Card *c) {
  printf("%s%s ", rank_str[c->rank], suit_str[c->suit]);
}

void print_shoe(Shoe *shoe) {
  for (int i = 0; i < NUM_DECKS * 52; i++) {
    print_card(&shoe->cards[i]);
    if ((i + 1) % 13 == 0) printf("\n");
  }
}

#endif
#endif
