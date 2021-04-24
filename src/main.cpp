#include <iostream>
#include <vector>

#include "game.h"

using namespace std;

void init_random_seed() {
  struct timespec tm;
  clock_gettime(CLOCK_REALTIME, &tm);
  srand((unsigned) tm.tv_nsec);
}

int main(int argc, const char *argv[]) {
  init_random_seed();

  GameState game = GameState::create_random();

  // print game
  cout << game << endl;

  vector<Move> moves_to_win;
  bool result = solve_game(game, moves_to_win);

  if (result) {
    // print moves in reverse
    for (auto i = moves_to_win.end(); i-- != moves_to_win.begin(); ) {
      cout << *i << ", ";
    }
    cout << endl;
  } else {
    cout << "Unsolvable" << endl;
  }

  return 0;
}
