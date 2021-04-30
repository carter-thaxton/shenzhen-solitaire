#pragma once

#include <iostream>

using namespace std;

class Card {
public:
  Card();
  Card(int suit, int value);

  int suit;
  int value;

  bool present() const;
  bool dragon() const;
  bool dragon_done() const;
  bool blank() const;
  bool normal() const;

  friend ostream& operator<<(ostream& os, const Card& card);
  friend bool operator==(const Card &c1, const Card &c2);
  friend bool operator!=(const Card &c1, const Card &c2);
  friend bool operator<(const Card &c1, const Card &c2);
};

const Card no_card(0, 0);
const Card blank_card(-1, 0);
const Card red_dragon(0, -1);
const Card green_dragon(1, -1);
const Card blue_dragon(2, -1);
