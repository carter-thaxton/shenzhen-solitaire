#pragma once

#include <iostream>

class Move {
public:
  Move(int from, int to, int size = 1, bool implicit = 1);

  int from;  // non-negative indicates a pile, negative indicates a slot
  int to;    // non-negative indicates a pile, negative indicates a slot, -999 indicates move to done
  int size;  // how many cards to move as a stack
  bool implicit;  // true when it's an automatic move without player choice

  friend std::ostream& operator<<(std::ostream& os, const Move& move);
};
