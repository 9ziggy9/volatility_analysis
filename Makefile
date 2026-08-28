CC=gcc
CFLAGS=-Wall -pedantic
EXE=main
SRC=main.c

-include config.mk

ifdef NUM_DECKS
CFLAGS += -DNUM_DECKS=$(NUM_DECKS)
endif

ifdef ENTRY_POINT
CFLAGS += -ENTRY_POINT=$(ENTRY_POINT)
endif

all: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXE)

config:
	@decks=$$(dialog --backtitle "bj build config" --stdout			\
		--radiolist "Select number of decks in the shoe:" 15 40 6 \
		1 "1 deck"           off																	\
		2 "2 decks"          off																	\
		4 "4 decks"          off																	\
		6 "6 decks (standard)" on																	\
		8 "8 decks"          off);																\
	mode=$$(dialog --backtitle "bj build config" --stdout				\
		--radiolist "Entry point:" 15 50 4												\
		entry_dealer_demo   "Dealer demo"          on							\
		entry_random        "RNG TESTING"          off)						\
	clear;																											\
	if [ -n "$$decks" ]; then																		\
		echo "NUM_DECKS=$$decks" > config.mk;											\
		echo "NUM_DECKS set to $$decks";													\
	fi                                                          
	if [ -n "$$mode" ]; then																		\
		echo "ENTRY_POINT=$$mode" > config.mk;									  \
		echo "ENTRY_POINT set to $$mode";													\
	fi

clean:
	rm -rf $(EXE)

distclean: clean
	rm -f config.mk

.PHONY: all clean config distclean
