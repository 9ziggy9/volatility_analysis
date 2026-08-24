#include <stdio.h>

#define CARDS_IMPLEMENTATION
#include "cards.h"

// forward declared print stuff
void print_card(Card *);
void print_shoe(Shoe *);

int main(void) {
  srand((unsigned int)time(NULL));
  Shoe shoe = init_ordered_shoe();
  shuffle_shoe(&shoe);
  printf("Shuffled shoe:\n");
  print_shoe(&shoe);

  Hand dealer_hand = init_hand(draw_card(&shoe), draw_card(&shoe));

  printf("\nDealer\n");
  print_card(dealer_hand.cards[1]);

  printf("\n");
  for (int n = 0; n < dealer_hand.hits + 2; n++) {
    print_card(dealer_hand.cards[n]);
  }
  printf("\n");

  bool dealer_busted = !dealer_hit_proc(&shoe, &dealer_hand);
  for (int n = 0; n < dealer_hand.hits + 2; n++) {
    print_card(dealer_hand.cards[n]);
  }
  if (dealer_hand.total == 21 && dealer_hand.hits == 0)
    printf("\033[32mBLACKJACK\033[0m");
  if (dealer_busted) printf("\033[31mBUSTED\033[0m");
  printf("\nDealer total: %u", dealer_hand.total);
  printf("\n");
  return 0;
}

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
