#include <iostream>
#include <vector>
#include <unordered_set>

#include "game.h"

using namespace std;

void init_random_seed() {
  struct timespec tm;
  clock_gettime(CLOCK_REALTIME, &tm);
  srand((unsigned) tm.tv_nsec);
}

int main(int argc, const char *argv[]) {
//  init_random_seed();
//  srand(0);
  srand(0);

  GameState game = GameState::create_random();

  // print game
  cout << game << endl;

  // solve game recursively, using efficient set of visited_states to avoid looping
  vector<Move> moves_to_win;
  unordered_set<GameState> visited_states;
  bool result = solve_game(game, moves_to_win, visited_states, 0);

  cout << "Visited " << visited_states.size() << " states" << endl;

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
