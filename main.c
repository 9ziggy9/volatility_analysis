#define CARDS_IMPLEMENTATION
#include "cards.h"

#define LOGS_IMPLEMENTATION
#include "logs.h"

// This is how we choose modes
#ifndef ENTRY_POINT
  #define ENTRY_POINT entry_dealer_demo
#endif



int entry_ui(void) {
  printf("Hello from UI\n");
  return 0;
}

int entry_dealer_demo(void) {
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

int main(void) {
  return ENTRY_POINT();
}
