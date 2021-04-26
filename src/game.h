#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

const int num_suits = 3;
const int num_piles = 8;
const int max_value = 9;
const int num_dragons = 4;
const int num_blanks = 1;

const int init_pile_size = ((max_value * num_suits) + (num_dragons * num_suits) + num_blanks) / num_piles;
const int max_pile_size = init_pile_size + (max_value - 2);
const int move_to_done = -999;

class Card {
public:
  Card();
  Card(int suit, int value);

  int suit;
  int value;

  bool present() const;
  bool dragon() const;
  bool blank() const;
  bool normal() const;

  friend ostream& operator<<(ostream& os, const Card& card);
  friend bool operator==(const Card &c1, const Card &c2);
  friend bool operator!=(const Card &c1, const Card &c2);
};

class Move {
public:
  Move(int from, int to, int size = 1, bool implicit = 1);

  int from;  // non-negative indicates a pile, negative indicates a slot
  int to;    // non-negative indicates a pile, negative indicates a slot, -999 indicates move to done
  int size;  // how many cards to move as a stack
  bool implicit;  // true when it's an automatic move without player choice

  friend ostream& operator<<(ostream& os, const Move& move);
};

class GameState {
public:
  GameState();

  Card piles[num_piles][max_pile_size];
  int  pile_sizes[num_piles];
  Card slots[num_suits];                    // can contain any card.  (once dragons are moved to done, they will have a value of -4 instead of -1, to represent a stack of 4 dragons)
  int  done[num_suits];                     // nonzero indicates top card value for the corresponding suit
  int  blank_done;                          // count of blank cards moved to the blank slot (0 or 1)

  bool win() const;
  std::tuple<bool,bool> check_move(const Move &move) const;
  const Card &top_card_of_pile(int pile) const;

  void make_move(const Move &move);

  static GameState create_random();

  friend ostream& operator<<(ostream &os, const GameState &game);
  friend bool operator==(const GameState &g1, const GameState &g2);
  friend bool operator!=(const GameState &g1, const GameState &g2);
};

template <> class std::hash<GameState> {
public:
  size_t operator()(const GameState &g) const;
};

bool solve_game(const GameState &game, vector<Move> &moves_to_win, unordered_set<GameState> &visited_states, int depth);

const Card no_card(0, 0);
const Card blank_card(-1, 0);
const Card red_dragon(0, -1);
const Card green_dragon(1, -1);
const Card blue_dragon(2, -1);
