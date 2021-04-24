#include "game.h"
#include "colors.h"

// state = init_state()
// moves_to_win = []
// result = solve_game(state, moves_to_win)
// if result == win
//   print moves_to_win in reverse
// else
//   print "unsolvable"

// solve_game(state, moves_to_win)
//   if state is in winning position
//     return win
//
//   for each legal next_move in state
//     next_state = make_move(next_move)
//     result = solve_game(next_state, moves_to_win)
//     if result == win
//       moves_to_win.append(next_move)
//       return win
//
//   return lose  // no win from here


// no card with space
// normal cards with colored number
// dragon cards with inverse @
// blank card with white #
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

ostream& operator<<(ostream& os, const GameState& game) {
  // +--------+
  os << "+"; for (int p=0; p < num_piles; p++) { os << "-"; } os << "+"; os << endl;

  os << "|";
  for (int i=0; i < num_suits; i++) {
    os << game.slots[i];
  }

  os << "|";
  os << (game.blank_done ? blank_card : no_card);

  for (int i=0; i < num_suits; i++) {
    os << Card(i, game.done[i]);
  }
  os << "|" << endl;

  // |--------|
  os << "|"; for (int p=0; p < num_piles; p++) { os << "-"; } os << "|"; os << endl;

  for (int h=0; h < max_pile_size; h++) {
    bool any = false;
    for (int p=0; p < num_piles; p++) {
      if (h < game.pile_sizes[p]) {
        any = true;
      }
    }

    if (any) {
      os << "|";
      for (int p=0; p < num_piles; p++) {
        if (h < game.pile_sizes[p]) {
          os << game.piles[p][h];
        } else {
          os << " ";
        }
      }
      os << "|" << endl;
    }

    if (!any)
      break;
  }

  // +--------+
  os << "+"; for (int p=0; p < num_piles; p++) { os << "-"; } os << "+"; os << endl;

  return os;
}

ostream& operator<<(ostream& os, const Move& move) {
  os << "<move " << move.from << "," << move.to << ">";
  return os;
}

Card::Card() : suit(0), value(0) {
}

Card::Card(int suit, int value) : suit(suit), value(value) {
}

Move::Move(int from, int to) : from(from), to(to) {
}

GameState::GameState() {
  for (int i=0; i < num_suits; i++) {
    slots[i] = Card(0, 0);
    done[i] = 0;
  }
  blank_done = 0;

  for (int i=0; i < num_piles; i++) {
    pile_sizes[i] = init_pile_size;
  }

  int p = 0;
  int h = 0;

  auto add_card = [&] (auto card) {
      piles[p][h] = card;
      p++;
      if (p >= num_piles) {
        p = 0;
        h++;
      }
  };

  for (int s=0; s < num_suits; s++) {
    for (int v=1; v <= max_value; v++) {
      add_card(Card(s, v));
    }

    for (int d=0; d < num_dragons; d++) {
      add_card(Card(s, -1));
    }
  }

  for (int b=0; b < num_blanks; b++) {
    add_card(Card(-1, 0));
  }
}


GameState GameState::create_random() {
  GameState result;

  // shuffle
  const int n = num_piles * init_pile_size;
  for (int i = 0; i < n * 3; i++) {
    int r1 = rand() % n;
    int r2 = rand() % n;

    int h1 = r1 / num_piles;
    int p1 = r1 % num_piles;

    int h2 = r2 / num_piles;
    int p2 = r2 % num_piles;

    Card tmp = result.piles[p1][h1];
    result.piles[p1][h1] = result.piles[p2][h2];
    result.piles[p2][h2] = tmp;
  }

  return result;
}


bool solve_game(const GameState &game, vector<Move> &moves_to_win) {
  moves_to_win.push_back(Move(1, 2));
  moves_to_win.push_back(Move(3, 4));

  return true;
}
