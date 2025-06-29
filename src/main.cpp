#include "game.h"
#include "time.h"

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    cout << "Usage: solitaire <seed> [max_depth]" << endl;
    cout << "  seed of 0 will choose randomly" << endl;
    cout << "  max_depth defaults to 1000" << endl;
    return 1;
  }

  int seed = atoi(argv[1]);
  if (seed <= 0) {
    srand(time(NULL));
  } else {
    srand(seed);
  }

  int max_depth = 1000;
  if (argc > 2) {
    max_depth = atoi(argv[2]);
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
