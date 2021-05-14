#pragma once

#include <iostream>
#include <vector>

#include "card.h"
#include "move.h"

const int num_suits = 3;
const int num_piles = 8;
const int max_value = 9;
const int num_dragons = 4;
const int num_blanks = 1;

const int init_pile_size = ((max_value * num_suits) + (num_dragons * num_suits) + num_blanks) / num_piles;
const int max_pile_size = init_pile_size + (max_value - 2);
const int move_to_done = -999;

enum class WinResult { WIN, LOSE, LOOP, MAX };

class GameState {
public:
  GameState();

  bool normalize();                         // returns true if modified

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

  friend std::ostream& operator<<(std::ostream &os, const GameState &game);
  friend bool operator==(const GameState &g1, const GameState &g2);
  friend bool operator!=(const GameState &g1, const GameState &g2);
};

template <> class std::hash<GameState> {
public:
  size_t operator()(const GameState &g) const;
};

bool solve_game_dfs(const GameState &game, std::vector<Move> &moves_to_win, int max_depth);
bool solve_game_bfs(const GameState &game, std::vector<Move> &moves_to_win);
