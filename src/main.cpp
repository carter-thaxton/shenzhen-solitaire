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

  int max_depth = 400;
  if (argc >= 1) {
    max_depth = atoi(argv[1]);
  }

  GameState game = GameState::create_random();

  // print game
  cout << game << endl;

  // solve game
  vector<Move> moves_to_win;
  bool result = solve_game_dfs(game, moves_to_win, max_depth);

  if (result) {
    // print moves in reverse
    for (auto i = moves_to_win.end(); i-- != moves_to_win.begin(); ) {
      auto move = *i;
      cout << move << endl;
      game.make_move(move);
      cout << game << endl;
    }
    cout << "Solution has " << moves_to_win.size() << " moves" << endl;
  } else {
    cout << "Unsolvable" << endl;
  }

  return 0;
}
