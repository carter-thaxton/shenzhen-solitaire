#pragma once

#include <iostream>
#include <vector>

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

  friend ostream& operator<<(ostream& os, const Card& card);
};

class GameState {
public:
  GameState();

  Card piles[num_piles][max_pile_size];
  int  pile_sizes[num_piles];
  Card slots[num_suits];
  int  done[num_suits];
  int  blank_done;

  static GameState create_random();

  friend ostream& operator<<(ostream& os, const GameState& game);
};

class Move {
public:
  Move(int from, int to);

  int from;  // non-negative indicates a pile, negative indicates a slot
  int to;    // non-negative indicates a pile, negative indicates a slot, -999 indicates move to done

  friend ostream& operator<<(ostream& os, const Move& move);
};


bool solve_game(const GameState &game, vector<Move> &moves_to_win);

const Card no_card(0, 0);
const Card blank_card(-1, 0);
const Card red_dragon(0, -1);
const Card green_dragon(1, -1);
const Card blue_dragon(2, -1);
