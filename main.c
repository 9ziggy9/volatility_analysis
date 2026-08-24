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
typedef struct Shoe { Card cards[NUM_DECKS * 52]; uint16_t cards_out; } Shoe;

// forward declared print stuff
void print_card(Card *);
void print_shoe(Shoe *);

/* BEGIN HAND DS
  This is a bit of a semantical rant, but what do we really mean by a hand which
  is "soft"? 

  For our purposes, we take a "soft hand" to mean that it contains at least one*
  ace which can be played as an 11, without busting the hand. We call an ace
  which can play as both an 11 or a 1 a "soft ace".

  By contrast, a "hard hand" is one which has exactly zero soft aces. That is,
  if any ace were to play as an 11, the hand would be bust.

  The motivation of the structure below is to encapsulate hardness/softness in
  one simple boolean expression: [ soft_aces > 0 ]. If this boolean expr returns
  true, the hand is soft, if it returns false, the hand is hard.

  A critical example of note is the soft 17. A6.
  We want an initialization procedure which recognizes that the ace can play as
  an 11, then, in say a dealing procedure, the goal would be to deplete the soft
  ace. This is the meaning of "dealer hits soft seventeen".

  * In standard blackjack, of course it is the case that there can only ever be
  on soft ace, as two aces acting as an 11 => 22 (which busts). I however want
  to leave the door open to more general blackjack games, i.e. ones where a
  total greater than 21 may have meaning. */
typedef struct Hand {
  Card  *cards[12];
  uint8_t hits;
  uint8_t total;
  uint8_t soft_aces;
} Hand;

Hand init_hand(Card *c1, Card *c2) {
  // to begin, we treat all aces as soft
  uint8_t soft_aces = (c1->rank == ACE) + (c2->rank == ACE);
  uint8_t total = RANK_TO_POINTS(c1->rank) + RANK_TO_POINTS(c2->rank);
  total += soft_aces * 10; // treating all aces as 11
  /* when the treatment of any remaining ace as an eleven no longer breaks the
     hand, we have successfully ascertained the number of soft aces.
     we can actually generalize to total > [any number], but for now, this is
     fine. */
  while (total > 21 && soft_aces > 0) { total -= 10; soft_aces--; }
  return (Hand) {
    .cards     = {[0] = c1, [1] = c2},
    .hits      = 0,
    .total     = total,
    .soft_aces = soft_aces,
  };
}
//END: HAND DS

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
bool hit_hand(Hand *h, Card *c) { // FALSE means busted
  h->cards[((h->hits++) + 2)] = c;
  h->total += RANK_TO_POINTS(c->rank);
  if (c->rank == ACE && h->total + 10 <= 21) {
    h->total += 10;
    h->soft_aces++;
  }
  // Same soft ace depletion method used in initialization
  while (h->total > 21 && h->soft_aces > 0) {
    h->total -= 10;
    h->soft_aces--;
  }
  return h->total <= 21;
}

bool dealer_should_hit(Hand *h) {
  return h->total < 17 || (h->total == 17 && h->soft_aces > 0);
} 

bool dealer_hit_proc(Shoe *s, Hand *h) {
  while(dealer_should_hit(h)) hit_hand(h, draw_card(s));
  return h->total <= 21;
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
