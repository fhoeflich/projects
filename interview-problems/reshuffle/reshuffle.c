/*
 * Question:
 * You are given a deck containing N cards. While holding the deck:
 *
 * 1. Take the top card off the deck and set it on the table.
 * 2. Take the next card off the top and put it on the bottom of the deck in
 *      your hand.
 * 3. Continue steps 1 and 2 until all cards are on the table. This is a round.
 * 4. Pick up the deck from the table and repeat steps 1-3 until the deck is in
 *      the original order.
 *
 * Part A
 * Write a program to determine how many rounds it will take to put a deck back
 * into the original order. This will involve creating a data structure to
 * represent the order of the cards. This program can be written in the
 * language that you are most comfortable with. It should take a number of
 * cards in the deck as a command line argument and write the result to stdout.
 * Please ensure the program compiles and runs correctly (no pseudo-code). 
 *
 * Part B
 * Please describe (in English, not code), how you would modify the code if you
 * were writing this for a low powered device (e.g. MCU).
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

typedef unsigned int card_t;

typedef struct item item_t;
struct item {
    card_t card;
    item_t *prev;
    item_t *next;
};

typedef struct deck {
    unsigned int count;
    item_t *cards;
} deck_t;

static int usage(void);
static unsigned int parse_args(int argc, char *argv[]);
static item_t *create_item(card_t card);
static item_t *nip_top(deck_t *deck);
static void tuck_bottom(item_t *tuck, deck_t *deck);
static void init_deck(deck_t *deck, unsigned int count);
static void one_move(deck_t *hand, deck_t *table);
static void round(deck_t *hand, deck_t *table);
static int is_orig_deck(deck_t *deck);
static void print_deck(char *label, deck_t *deck);

static int debug = 0;
static unsigned int decksize;   /* size of hand and table decks */

int
main(int argc, char *argv[])
{
    unsigned int rounds;
    deck_t hand;        /* Part B optimisation: globals to use less stack */
    deck_t table;
    deck_t *decka = &hand;
    deck_t *deckb = &table;
    deck_t *tmp;
    extern unsigned int decksize;

    decksize = parse_args(argc, argv);
    if (decksize == 0) {
        usage();
    }

    /*
     * Initialise the hand and table decks.
     */
    init_deck(&hand, decksize);
    init_deck(&table, 0);

    /*
     * Loop doing rounds and comparisons against the original hand deck
     * until we get it back.
     */
    for (rounds = 0; ; rounds++) {
        if (debug) {
            fprintf(stderr, "Top of rounds loop\n");
            print_deck("hand", &hand);
            print_deck("table", &table);
        }

        round(decka, deckb);

        if (is_orig_deck(deckb)) {
            /*
             * Print the number of rounds it took to get here.
             */
            rounds++;
            printf("It took %u rounds to get back the deck of %u cards\n",
                    rounds, decksize);
            break;
        }

        /*
         * Otherwise "pick up" the table deck and continue with another round.
         */
        tmp = decka;
        decka = deckb;
        deckb = tmp;
    }

    exit(0);
}

static unsigned int
parse_args(int argc, char *argv[])
{
    int ch;
    extern int debug;

    while ((ch = getopt(argc, argv, "d")) != -1) {
        switch (ch) {
            case 'd':
                debug = 1;
                fprintf(stderr, "debug on\n");
                break;

            case '?':
            default:
                usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (argc < 1) {
        fprintf(stderr, "Must supply a number of cards\n");
        usage();
    }

    return (unsigned int) atoi(argv[0]);
}

static int
usage()
{
    fprintf(stderr, "Usage: aifi [-d] <n>\n");
    fprintf(stderr, "  where n is a positive number of cards in the deck\n");
    exit(1);
}

static item_t *
create_item(card_t card)
{
    item_t *newitem;

        /*
         * Part B optimisation: might make more effort to do calloc() of a
         * slab of fixed-size items instead in order to make best use of
         * limited memory, then create_item() would return an individual chunk.
         */
    newitem = malloc(sizeof (*newitem));
    if (newitem == NULL) {
        fprintf(stderr, "Not enough memory to allocate a card\n");
        exit(1);
    }

    newitem->card = card;
    newitem->prev = NULL;
    newitem->next = NULL;

    return (newitem);
}

static void
init_deck(deck_t *deck, unsigned int count)
{
    unsigned int i;
    item_t *current;
    item_t *prev;

    /*
     * Initialise a deck containing `count' cards with enumerated "values."
     * It is atypical to enumerate from 1 rather than 0, but this makes it
     * easier to compare values when performing debug trials with a real deck.
     */
    deck->count = count;
    deck->cards = current = create_item(1);
    current->prev = NULL;   /* will patch correctly after creating final card */
    current->next = NULL;
    prev = current;

    for (i = 2; i < count + 1; i++) {
        current = create_item(i);
        current->prev = prev;
        current->next = NULL;
        prev->next = current;
        prev = current;
    }

    /*
     * Close up the doubly linked cards list.
     */
    current->next = deck->cards;
    deck->cards->prev = current;
}

static item_t *
nip_top(deck_t *deck)
{
    item_t *nip;

    /*
     * Nip the top card off of a non-empty deck and null out its list pointers
     * for safety.
     * Caller will repopulate them when it tucks the card someplace.
     */
#if 0
    if (debug) {
        fprintf(stderr, "Top of nip_top: deck->count = %u\n", deck->count);
        print_deck("nip_top-top", deck);
    }
#endif
    assert(deck->count > 0);

    nip = deck->cards;
    nip->prev->next = nip->next;
    nip->next->prev = nip->prev;

    deck->cards = nip->next;
    deck->count--;

    nip->prev = nip->next = NULL;
    return (nip);
}

static void
tuck_bottom(item_t *tuck, deck_t *deck)
{
    item_t *current;

    /*
     * Tuck/append a card onto the bottom of `deck', which may be empty.
     */
#if 0
    if (debug) {
        fprintf(stderr, "Top of tuck_bottom: deck->count = %u\n", deck->count);
    }
#endif

    if (deck->count == 0) {
        tuck->prev = tuck;
        tuck->next = tuck;
        deck->cards = tuck;
        deck->count++;
        return;
    }

    current = deck->cards->prev;
    tuck->prev = current;
    tuck->next = current->next;
    current->next->prev = tuck;
    current->next = tuck;
    deck->count++;
}

static void
one_move(deck_t *decka, deck_t *deckb)
{
    item_t *nip;

    if (debug) {
        fprintf(stderr, "Top of one_move()\n");
        print_deck("decka", decka);
        print_deck("deckb", deckb);
    }

    /*
     * A single "move" describes the actions of both steps (1) and (2):
     *
     * (1) The top card of the hand deck is removed and is then added as
     *   the new bottom card of the table deck.
     *   Side effects: hand deck count decrements, table deck count increments
     *   Algorithm hint: suggests the need to nip the top card while leaving
     *      the rest in place, and to tuck/append a card at the bottom of a
     *      different deck.
     */
    nip = nip_top(decka);
    tuck_bottom(nip, deckb);

    /*
     * (2) The new top card of the hand deck is simply moved to the bottom of
     *   the hand deck.
     *   Side effects: No count changes, hand deck sequence rotates by one
     *   Algorithm hint: (conveniently) same as for (1).  In this case the nip
     *      of the top card is followed by a need to tuck/append a card at the
     *      bottom of the *same* deck.
     *
     * Both algorithms require operations involving only the top and bottom
     * cards of decks, which strongly suggests that a doubly-linked list would
     * be beneficial.  This drives the selection of the deck->cards data
     * structure.
     */
    if (decka->count > 1) {
        /*
         * If the deck has 0 cards, there's nothing to nip anyway.
         * This can be a legal/expected condition here (just moved
         * last card onto the table in step (1)).
         *
         * If the deck only has 1 card, nip+tuck is a no-op!
         */
        nip = nip_top(decka);
        tuck_bottom(nip, decka);
    }

    if (debug) {
        fprintf(stderr, "Bottom of one_move()\n");
        print_deck("decka", decka);
        print_deck("deckb", deckb);
    }

    return;
}

static void
round(deck_t *decka, deck_t *deckb)
{
    extern unsigned int decksize;

    /*
     * A "round" as defined in step (3) is to execute one_move() on the hand
     * and table decks until all cards in the hand deck at the start of the
     * round end up in the table deck, and the hand deck ends up empty.
     *
     * Since step (4) involves switching the roles of hand and table decks,
     * all we know here is that the `a' deck (currently in-hand) should start
     * out with all the cards and the `b' deck should be empty.
     */
    assert(decka->count == decksize);
    assert(deckb->count == 0);

    do {
        if (debug) {
            fprintf(stderr, "Top  of round() loop\n");
            fprintf(stderr, "=============================================\n");
        }
        one_move(decka, deckb);
        if (debug) {
            fprintf(stderr, "Bottom of round() loop\n");
            print_deck("decka", decka);
            print_deck("deckb", deckb);
        }
    } while (deckb->count < decksize);

    assert(decka->count == 0);
    return;
}

static int
is_orig_deck(deck_t *deck)
{
    unsigned int i;
    item_t *current;
    extern unsigned int decksize;

    /*
     * There is a tacit assumption here about how the original hand deck
     * was initialised.  It would be more general, but also more expensive
     * in terms of computation and storage, to store the original deck and
     * actually compare it to this deck.
     */
    if (deck->count != decksize) {
        fprintf(stderr, "Not original deck: size is %u (!= %u)\n",
            deck->count, decksize);
        return (0);
    }

    current = deck->cards;
        /*
         * Part B optimisation:  Rather than iterating through this loop of
         * a full deck of cards following each round, it'd be cheaper
         * computationally to manage an additional `sorted' flag per deck.
         *
         * Start off a deck with the `sorted' flag set.  Thereafter any time
         * a tuck is performed, you check its value against that of the
         * previous card; if it's not the next card in ordinal sequence, you
         * must clear the deck's `sorted' flag.  If a full deck ends up with
         * `sorted' still set, your work is already done by just checking the
         * flag.
         */
    for (i = 1; i < deck->count + 1; i++) {
        if (current->card != i) {
            if (debug) {
                fprintf(stderr, "Not original deck: out of order\n");
            }
            return (0);
        }
        current = current->next;
    }

    assert(current == deck->cards);
    return (1);
}

static void
print_deck(char *label, deck_t *deck)
{
    unsigned int i;
    item_t *current;

    /*
     * Print the state of a deck to stderr.  Debug only.
     */
    fprintf(stderr, "%s: cnt: %u |", label, deck->count);
    current = deck->cards;
    for (i = 0; i < deck->count; i++) {
        fprintf(stderr, " %u", current->card);
        current = current->next;
        assert(current != NULL);
    }
    fprintf(stderr, "\n");
}
