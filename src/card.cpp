#include "card.h"
#include "colors.h"

using namespace std;

Card::Card() : suit(0), value(0) {
}

Card::Card(int suit, int value) : suit(suit), value(value) {
}

bool Card::present() const {
  return (suit != 0) || (value != 0);
}

bool Card::dragon() const {
  return (suit >= 0) && (value < 0);
}

bool Card::dragon_done() const {
  return (suit >= 0) && (value < -1);
}

bool Card::blank() const {
  return (suit < 0);
}

bool Card::normal() const {
  return (suit >= 0) && (value >= 0);
}

bool operator==(const Card &c1, const Card &c2)
{
  return (c1.suit == c2.suit) && (c1.value == c2.value);
}

bool operator!=(const Card &c1, const Card &c2)
{
    return !(c1 == c2);
}

bool operator<(const Card &c1, const Card &c2)
{
    if (c1.value != c2.value) return (c1.value < c2.value);
    return (c1.suit < c2.suit);
}

// no card with space
// normal cards with colored number
// dragon cards with inverse @
// blank card with white #`
ostream& operator<<(ostream& os, const Card& card) {
  if (card.suit < 0) {
    // blank card
    os << "#";
  } else if (card.value == 0) {
    // no card
    os << " ";
  } else if (card.value < 0) {
    // dragon
    switch (card.suit) {
      case 0:
        os << inverse_red;
      break;

      case 1:
        os << inverse_green;
      break;

      case 2:
        os << inverse_blue;
      break;
    }
    os << "@" << reset;
  } else {
    // normal card
    switch (card.suit) {
      case 0:
        os << red;
      break;

      case 1:
        os << green;
      break;

      case 2:
        os << blue;
      break;
    }
    os << card.value << reset;
  }
  return os;
}
