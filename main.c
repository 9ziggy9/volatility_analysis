#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#ifndef NUM_DECKS
#define NUM_DECKS 6
#endif

typedef enum {
  ACE = 1,
  TWO,   THREE, FOUR, FIVE, SIX,   SEVEN,
  EIGHT, NINE,  TEN,  JACK, QUEEN, KING
} CardRank;

#define RANK_TO_POINTS(RANK) ((uint8_t)(RANK) < 10 ? (uint8_t)(RANK) : 10)

typedef enum { HEARTS, CLUBS, DIAMONDS, SPADES } CardSuit;

typedef struct Card { CardRank rank; CardSuit suit; } Card;

typedef struct Hand {
  Card  *cards[12];
  uint8_t hits;
  uint8_t total;
  bool  is_soft;
} Hand;

Hand init_hand(Card *c1, Card *c2) {
  uint8_t init_total = 0;
  bool init_soft = true;
  if (c1->rank == ACE && c2->rank == ACE) {
    init_total = 12;
  } else if (c1->rank == ACE || c2->rank == ACE) {
    CardRank other = c1->rank == ACE ? c2->rank : c1->rank;
    init_total = RANK_TO_POINTS(other) + 11;
  } else {
    init_total = RANK_TO_POINTS(c1->rank) + RANK_TO_POINTS(c2->rank);
    init_soft  = false;
  }
  return (Hand) {
    .cards   = {[0] = c1, [1] = c2},
    .hits    = 0,
    .total   = init_total,
    .is_soft = init_soft,
  };
}

typedef struct Shoe {
  Card cards[NUM_DECKS * 52];
  uint16_t cards_out;
} Shoe;

Shoe init_ordered_shoe(void) {
  Shoe shoe = { .cards_out = 0 };
  for (int d = 0; d < NUM_DECKS; d++) {
    for (CardSuit s = HEARTS; s <= SPADES; s++) {
      for (CardRank r = ACE; r <= KING; r++) {
        shoe.cards[d * 52 + (uint8_t)(r - 1) + (uint8_t) s * 13] =
         (Card){
          .rank = r, .suit = s
        };
      }
    }
  }
  return shoe;
}

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

// Need better choice of random (of course)
void shuffle_shoe(Shoe *shoe) {
  int n = NUM_DECKS * 52;
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    Card tmp = shoe->cards[i];
    shoe->cards[i] = shoe->cards[j];
    shoe->cards[j] = tmp;
  }
  shoe->cards_out = 0;
}

Card *draw_card(Shoe *shoe) { return &shoe->cards[shoe->cards_out++]; }

bool dealer_should_hit(Hand *dealer_hand) {
  return dealer_hand->total < 17
    || (dealer_hand->total == 17 && dealer_hand->is_soft);
} 

// A return value of "false" means the dealer busted
bool hit_proc(Shoe *shoe, Hand *dealer_hand) {
  while (dealer_should_hit(dealer_hand)) {
    Card *hit_card = draw_card(shoe);
    dealer_hand->cards[(dealer_hand->hits++) + 2] = hit_card;
    if (hit_card->rank == ACE && dealer_hand->total + 11 <= 21) {
      dealer_hand->total += 11;
      dealer_hand->is_soft = true;
    } else { // What if this goes over 21 and "breaks" the soft hand?
      dealer_hand->total += RANK_TO_POINTS(hit_card->rank);
      if (dealer_hand->is_soft && dealer_hand->total > 21) {
        dealer_hand->total -= 10;
        dealer_hand->is_soft = false;
      }
    }
  }
  return dealer_hand->total <= 21;
}

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

  bool dealer_busted = !hit_proc(&shoe, &dealer_hand);
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
