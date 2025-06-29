#include "game.h"
#include "magic_enum.hpp"

#include <unordered_set>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

ostream& operator<<(ostream& os, const GameState& game) {
  // +--------+
  os << '+' << std::string(num_piles, '-') << '+' << endl;

  os << '|';
  for (int i=num_suits-1; i >= 0; i--) {
    os << game.slots[i];
  }

  os << '|';
  os << (game.blank_done ? blank_card : no_card);

  for (int i=0; i < num_suits; i++) {
    os << Card(i, game.done[i]);
  }
  os << '|' << endl;

  // |--------|
  os << '|' << std::string(num_piles, '-') << '|' << endl;

  for (int h=0; h < max_pile_size; h++) {
    bool any = false;
    for (int p=0; p < num_piles; p++) {
      if (h < game.pile_sizes[p]) {
        any = true;
      }
    }

    if (any) {
      os << '|';
      for (int p=0; p < num_piles; p++) {
        if (h < game.pile_sizes[p]) {
          os << game.piles[p][h];
        } else {
          os << ' ';
        }
      }
      os << '|' << endl;
    }

    if (!any)
      break;
  }

  // +--------+
  os << '+' << std::string(num_piles, '-') << '+' << endl;

  return os;
}


// provide specialization of std::hash<GameState>()
size_t std::hash<GameState>::operator()(const GameState& g) const
{
  // computes the hash of the game state using a variant
  // of the Fowler-Noll-Vo hash function
  size_t result = 2166136261;

  for (int p = 0; p < num_piles; p++) {
    for (int h = 0; h < g.pile_sizes[p]; h++) {
      result = (result * 16777619) ^ g.piles[p][h].suit;
      result = (result * 16777619) ^ g.piles[p][h].value;
    }
  }

  for (int s = 0; s < num_suits; s++) {
    result = (result * 16777619) ^ g.slots[s].suit;
    result = (result * 16777619) ^ g.slots[s].value;
    result = (result * 16777619) ^ g.done[s];
  }

  return result ^ (g.blank_done << 1);
}


bool operator==(const GameState& g1, const GameState& g2)
{
  if (g1.blank_done != g2.blank_done)
    return false;

  for (int s = 0; s < num_suits; s++) {
    if (g1.slots[s] != g2.slots[s]) return false;
    if (g1.done[s] != g2.done[s]) return false;
  }

  for (int p = 0; p < num_piles; p++) {
    if (g1.pile_sizes[p] != g2.pile_sizes[p]) return false;
    for (int h = 0; h < g1.pile_sizes[p]; h++) {
      if (g1.piles[p][h] != g2.piles[p][h]) return false;
    }
  }

  return true;
}

bool operator!=(const GameState& g1, const GameState& g2)
{
    return !(g1 == g2);
}


GameState::GameState() {
  for (int i=0; i < num_suits; i++) {
    slots[i] = Card(0, 0);
    done[i] = 0;
  }
  blank_done = 0;

  for (int i=0; i < num_piles; i++) {
    pile_sizes[i] = 0;
  }
}

bool GameState::win() const {
  for (int s = 0; s < num_suits; s++) {
    if (done[s] != max_value) return false;
  }
  for (int s = 0; s < num_suits; s++) {
    if (!slots[s].dragon_done()) return false;
  }
  if (blank_done != num_blanks) return false;
  return true;
}

const Card &GameState::top_card_of_pile(int pile) const {
  if (pile_sizes[pile] <= 0) return no_card;
  return piles[pile][pile_sizes[pile]-1];
}

GameState GameState::create_random() {
  GameState result;

  for (int i=0; i < num_piles; i++) {
    result.pile_sizes[i] = init_pile_size;
  }

  // initial cards in order
  int p = 0;
  int h = 0;

  auto add_card = [&] (auto card) {
      result.piles[p][h] = card;
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

  // shuffle
  const int n = num_piles * init_pile_size;
  for (int i = 0; i < n * 3; i++) {
    int r1 = rand() % n;
    int h1 = r1 / num_piles;
    int p1 = r1 % num_piles;

    int r2 = rand() % n;
    int h2 = r2 / num_piles;
    int p2 = r2 % num_piles;

    Card tmp = result.piles[p1][h1];
    result.piles[p1][h1] = result.piles[p2][h2];
    result.piles[p2][h2] = tmp;
  }

  return result;
}

static void move_dragons_to_done(GameState& game, int suit) {
  // check if the destination slot is already in use
  // and swap slots to free up the slot corresponding to the dragon suit
  auto dest = game.slots[suit];
  if (dest.present() && (!dest.dragon() || dest.suit != suit)) {
    int other = -1;
    // find another slot to swap with
    for (int i=0; i < num_suits; i++) {
      if (i != suit) {
        auto card = game.slots[i];
        if (!card.present() || (card.dragon() && (card.suit == suit))) {
          other = i;
          break;
        }
      }
    }
    if (other >= 0) {
      auto tmp = game.slots[other];
      game.slots[other] = game.slots[suit];
      game.slots[suit] = tmp;
    } else {
      // No available slot - not a legal move
      abort();
    }
  }

  // remove all dragons showing
  int dragons_to_move = num_dragons;
  for (int s=0; (s < num_suits) && (dragons_to_move > 0); s++) {
    if (s != suit) {
      auto card = game.slots[s];
      if (card.dragon() && (card.suit == suit)) {
        game.slots[s] = no_card;
        dragons_to_move--;
      }
    }
  }
  for (int p=0; (p < num_piles) && (dragons_to_move > 0); p++) {
    int h = game.pile_sizes[p]-1;
    auto card = game.piles[p][h];
    if (card.dragon() && (card.suit == suit)) {
      game.piles[p][h] = no_card;
      game.pile_sizes[p]--;
      dragons_to_move--;
    }
  }

  // put all of the dragons in the corresponding done slot, with value of -4
  game.slots[suit].suit = suit;
  game.slots[suit].value = -num_dragons;
}

void GameState::make_move(const Move& move) {
  int to = move.to;
  int from = move.from;

  if (from < 0) {
    if (to == move_to_done) {
      // slot to done  (according to suit)
      int s = -from-1;
      int suit = slots[s].suit;
      int value = slots[s].value;
      if (suit < 0) {
        blank_done += 1;
        slots[s] = no_card;
      } else if (value < 0) {
        // move multiple dragons to done
        move_dragons_to_done(*this, suit);
      } else {
        // normal card to done - update value of top card in done pile
        done[suit] = value;
        slots[s] = no_card;
      }
    } else if (to < 0) {
      // slot to slot
      if (from != to) {
        int from_s = -from-1;
        int to_s = -to-1;
        Card tmp = slots[to_s];
        slots[to_s] = slots[from_s];
        slots[from_s] = tmp;
      }
    } else {
      // slot to pile
      int s = -from-1;
      int h = pile_sizes[to];
      piles[to][h] = slots[s];
      pile_sizes[to]++;
      slots[s] = no_card;
    }
  } else {
    if (to == move_to_done) {
      // pile to done  (according to suit)
      int h = pile_sizes[from]-1;
      int suit = piles[from][h].suit;
      int value = piles[from][h].value;
      if (suit < 0) {
        // blank card done
        blank_done += 1;
        piles[from][h] = no_card;
        pile_sizes[from]--;
      } else if (value < 0) {
        // move multiple dragons to done
        move_dragons_to_done(*this, suit);
      } else {
        // normal card to done - update value of top card in done pile
        done[suit] = value;
        piles[from][h] = no_card;
        pile_sizes[from]--;
      }
    } else if (to < 0) {
      // pile to slot
      int s = -to-1;
      int h = pile_sizes[from]-1;
      slots[s] = piles[from][h];
      piles[from][h] = no_card;
      pile_sizes[from]--;
    } else {
      // pile to pile  (moving size cards)
      int size = move.size;
      int from_h = pile_sizes[from] - size;
      int to_h = pile_sizes[to];
      pile_sizes[from] -= size;
      pile_sizes[to] += size;
      while (size > 0) {
        piles[to][to_h] = piles[from][from_h];
        piles[from][from_h] = no_card;
        from_h++;
        to_h++;
        size--;
      }
    }
  }
}

static bool can_move_dragon_to_done(const GameState& game, int suit) {
  int dragons_showing = 0;
  int free_slots = 0;

  for (int p = 0; p < num_piles; p++) {
    auto card = game.top_card_of_pile(p);
    if (card.dragon() && card.suit == suit)
      dragons_showing++;
  }

  for (int s = 0; s < num_suits; s++) {
    auto card = game.slots[s];
    if (card.dragon() && card.suit == suit) {
      dragons_showing++;
      free_slots++;  // a dragon with appropriate suit in a slot is a free slot
    }
    if (!card.present()) {
      free_slots++;
    }
  }

  return (free_slots > 0) && (dragons_showing == num_dragons);
}

static bool can_move_normal_to_done(const GameState& game, int suit, int value, bool implicit) {
  if (game.done[suit] != (value - 1))
    return false;

  // implicit moves to done are only performed when they don't exceed the other done piles by one
  if (implicit) {
    for (int i=0; i < num_suits; i++) {
      if (game.done[i] < (value - 1)) {
        return false;
      }
    }
  }

  return true;
}

static bool can_move_card_onto_card(const Card& card, const Card& onto_card) {
  if (!card.present()) return false;

  if (!onto_card.present()) {
    return true;  // can always place onto empty pile
  } else if (!onto_card.normal()) {
    return false; // can't place onto dragon or blank
  } else if (!card.normal()) {
    return false; // can't move dragon or blank onto another card
  } else {
    // ascending order of different suit
    return (card.value == (onto_card.value - 1)) && (card.suit != onto_card.suit);
  }
}

// return true if legal move, and also return whether to check higher stack sizes, when moving pile to pile
std::tuple<bool,bool> GameState::check_move(const Move& move) const {
  int to = move.to;
  int from = move.from;
  int size = move.size;
  bool implicit = move.implicit;

  if (to == move_to_done) {
    if (size != 1) return {false, false};  // only ever move one card at a time to done
    if (from < 0) {
      // slot to done
      int from_slot = -from-1;
      auto card = slots[from_slot];
      if (!card.present()) return {false, false};
      if (card.dragon_done()) return {false, false};
      if (card.dragon()) {
        // dragon to done
        if (implicit) return {false, false};
        return {can_move_dragon_to_done(*this, card.suit), false};
      } else {
        // normal to done
        return {can_move_normal_to_done(*this, card.suit, card.value, implicit), false};
      }
    } else {
      // pile to done
      if (pile_sizes[from] <= 0) return {false, false};
      auto card = piles[from][pile_sizes[from]-1];
      if (!card.present()) return {false, false};
      if (card.blank()) {
        // blank to done
        return {true, false};    // always legal  (should always be implicit, too)
      } else if (card.dragon()) {
        // dragon to done
        if (implicit) return {false, false};
        return {can_move_dragon_to_done(*this, card.suit), false};
      } else {
        // normal to done
        return {can_move_normal_to_done(*this, card.suit, card.value, implicit), false};
      }
    }
  } else if (to < 0) {
    if (size != 1) return {false, false};  // slot can only contain one card
    if (from < 0) {
      // slot to slot
      return {false, false};     // never necessary - just consider always illegal
    } else {
      // pile to slot
      if (implicit) return {false, false};
      if (pile_sizes[from] <= 0) return {false, false};
      int to_slot = -to-1;
      return {!slots[to_slot].present(), false};  // legal as long as slot is empty
    }
  } else if (from < 0) {
    // slot to pile
    if (size != 1) return {false, false};  // slot can only contain one card
    if (implicit) return {false, false};
    int from_slot = -from-1;
    auto card = slots[from_slot];
    if (card.dragon_done()) return {false, false};
    auto onto_card = top_card_of_pile(to);
    return {can_move_card_onto_card(card, onto_card), false};
  } else {
    // pile to pile
    if (implicit) return {false, false};
    auto onto_card = top_card_of_pile(to);

    // size can be 1 or more.  check the bottom card of the given stack
    if (size < 1) return {false, false};
    int h = pile_sizes[from]-size;
    if (h < 0) return {false, false};
    auto card = piles[from][h];

    bool legal = can_move_card_onto_card(card, onto_card);

    // if next card forms an ascending sequence of alternating suits, then that's another move to try, using the same from/to
    auto next_card = (h > 0) ? piles[from][h-1] : no_card;
    bool try_next_size = card.normal() && next_card.normal() && (next_card.suit != card.suit) && (next_card.value == card.value + 1);

    return {legal, try_next_size};
  }
}

bool GameState::normalize() {
  int pile_indexes[num_piles];
  int slot_indexes[num_suits];
  for (int p=0; p < num_piles; p++) {
    pile_indexes[p] = p;
  }
  for (int s=0; s < num_suits; s++) {
    slot_indexes[s] = s;
  }

  // sort slot and pile indexes
  sort(begin(slot_indexes), end(slot_indexes),
      [&] (int l, int r) {
    auto slot_l = slots[l];
    auto slot_r = slots[r];
    return slot_l < slot_r;
  });

  sort(begin(pile_indexes), end(pile_indexes),
      [&] (int l, int r) {
    if (pile_sizes[l] != pile_sizes[r]) return (pile_sizes[l] < pile_sizes[r]);
    auto top_l = this->top_card_of_pile(l);
    auto top_r = this->top_card_of_pile(r);
    bool result = top_l < top_r;
    return result;
  });

  // check if changed
  bool changed = false;
  for (int s=0; !changed && (s < num_suits); s++) {
    if (slot_indexes[s] != s)
      changed = true;
  }
  for (int p=0; !changed && (p < num_piles); p++) {
    if (pile_indexes[p] != p)
      changed = true;
  }

  // if indexes changed, move the actual data, using a copy of the original
  if (changed) {
    GameState orig = *this;
    for (int p=0; p < num_piles; p++) {
      int from_p = pile_indexes[p];
      pile_sizes[p] = orig.pile_sizes[from_p];
      for (int h=0; h < orig.pile_sizes[from_p]; h++) {
        piles[p][h] = orig.piles[from_p][h];
      }
    }
    for (int s=0; s < num_suits; s++) {
      int from_s = slot_indexes[s];
      slots[s] = orig.slots[from_s];
    }
  }

  return changed;
}

static WinResult solve_game_recursive(const GameState& state, vector<Move>& moves_to_win, unordered_set<GameState>& visited_states, int depth, int max_states, int max_depth) {
  // Base case - we found a winning state!
  if (state.win())
    return WinResult::WIN;

  // DEBUG: stop after N visits
  if (visited_states.size() >= max_states) {
//    cout << "Max states reached: " << visited_states.size() << endl;
    return WinResult::MAX;
  }
  if (depth >= max_depth) {
//    cout << "Max depth reached: " << depth << endl;
    return WinResult::MAX;
  }

  // Create a copy of the state, in normalized form
  GameState normalized_state = state;
  normalized_state.normalize();

  // Check if we have already visited this normalized state, to avoid loops
  if (visited_states.find(normalized_state) != visited_states.end()) {
    return WinResult::LOOP;
  }
  visited_states.insert(normalized_state);

  // Counters to loop through each next move
  bool implicit = true;
  int from = -num_suits;
  int to = move_to_done;
  int size = 1;

  int move_count = 0;

  // Try each next move in turn
  while (1) {
    Move move(from, to, size, implicit);

    // Check if move is legal, and whether we should also check further stack sizes
    auto [legal, try_next_size] = state.check_move(move);

    // Iterate through each move one at a time, first checking all implicit moves
    // from:  slots, piles
    // to:    done, piles, slots
    if (try_next_size) {
      size += 1;
    } else if (to == move_to_done) {
      to = 0;
      size = 1;
    } else if (to >= num_piles-1) {
      to = -num_suits;
      size = 1;
    } else if (to == -1) {
      from += 1;
      to = move_to_done;
      size = 1;
      if (from >= num_piles) {
        if (implicit) {
          implicit = false;
          from = -num_suits;
        }
        else
          break;  // done looping
      }
    } else {
      to += 1;
      size = 1;
    }

    // Only proceed with legal moves
    if (!legal)
      continue;

    move_count++;

    // Make the move on a copy of the game state
    GameState next_state = state;
    next_state.make_move(move);

    // Recursively check the state after making this move
    WinResult result = solve_game_recursive(next_state, moves_to_win, visited_states, depth+1, max_states, max_depth);

    if (result == WinResult::WIN) {
      // Found a winning line, append this move to the result as we unwind the stack
      moves_to_win.push_back(move);

      // Also remove all normalized states that were reached as part of a non-losing line,
      // so that visited_states only contains states to ignore on future searches
      visited_states.erase(normalized_state);

      return result;
    } else if (implicit) {
      // If we tried an implicit move that resulted in anything but a win, then this entire line can't be solved for the same reason

      if (result != WinResult::LOSE) {
        // Also remove all normalized states that were reached as part of a non-losing line,
        // so that visited_states only contains states to ignore on future searches
        visited_states.erase(normalized_state);
      }

      return result;
    }
  }

  // No more legal moves, or all legal moves from this state result in a loss
  return WinResult::LOSE;
}

bool solve_game_dfs(const GameState& game, std::vector<Move>& moves_to_win, int max_depth) {
  unordered_set<GameState> visited_states;
  WinResult result = solve_game_recursive(game, moves_to_win, visited_states, 0, 10000000, max_depth);
  return result == WinResult::WIN;
}

bool solve_game_bfs(const GameState& game, vector<Move>& moves_to_win) {
  unordered_set<GameState> visited_states;
  vector<tuple<Move, int, int>> all_moves;
  queue<pair<GameState, int>> states_to_visit;
  unordered_set<GameState> lookahead_states;

  states_to_visit.emplace(game, -1);

  int prev_depth = -1;
  int max_depth = 500;

  while (!states_to_visit.empty()) {
    auto [state, prev_move_index] = states_to_visit.front();
    states_to_visit.pop();

    // Get depth of game to reach this state, which is stored along with each move
    int depth = (prev_move_index < 0) ? 0 : get<2>(all_moves[prev_move_index]);

    // Print some statistics each time we reach a new depth
    if (depth != prev_depth) {
      cout << "depth: " << depth << "/" << max_depth << "  visited: " << visited_states.size() << "  moves: " << all_moves.size() << "  to_visit: " << states_to_visit.size() << endl;
      prev_depth = depth;
    }

    // Every so often, check if this state can be solved at all, using depth-first-search
    if (depth % 3 == 0) {
      vector<Move> lookahead_moves;
      WinResult result = solve_game_recursive(state, lookahead_moves, lookahead_states, depth, 10000000, max_depth);
      if (result == WinResult::WIN) {
        cout << "YES - solution with " << lookahead_moves.size() << " moves, lookahead_states: " << lookahead_states.size() << endl;
        max_depth = lookahead_moves.size();
      } else {
        cout << "NO - lookahead_states: " << lookahead_states.size() << " result: " << magic_enum::enum_name(result) << endl;
        continue;
      }
    }

    // Counters to loop through each next move
    bool implicit = true;
    int from = -num_suits;
    int to = move_to_done;
    int size = 1;

    // Try each next move in turn
    while (1) {
      Move move(from, to, size, implicit);

      // Check if move is legal, and whether we should also check further stack sizes
      auto [legal, try_next_size] = state.check_move(move);

      // Iterate through each move one at a time, first checking all implicit moves
      // from:  slots, piles
      // to:    done, piles, slots
      if (try_next_size) {
        size += 1;
      } else if (to == move_to_done) {
        to = 0;
        size = 1;
      } else if (to >= num_piles-1) {
        to = -num_suits;
        size = 1;
      } else if (to == -1) {
        from += 1;
        to = move_to_done;
        size = 1;
        if (from >= num_piles) {
          if (implicit) {
            implicit = false;
            from = -num_suits;
          }
          else
            break;  // done looping
        }
      } else {
        to += 1;
        size = 1;
      }

      // Only proceed with legal moves
      if (!legal)
        continue;

      // Make the move on a copy of the game state
      GameState next_state = state;
      next_state.make_move(move);

      // Base case - we found a winning state!
      if (next_state.win()) {
        // collect winning moves to get to this state
        moves_to_win.push_back(move);

        int i = prev_move_index;
        while (i >= 0) {
          auto [prev_move, prev_index, depth] = all_moves[i];
          moves_to_win.push_back(prev_move);
          i = prev_index;
        }
        return true;
      }

      // Check if we have already visited this normalized state, to avoid loops
      GameState normalized_state = next_state;
      normalized_state.normalize();
      if (visited_states.find(normalized_state) != visited_states.end()) {
        continue;
      }
      visited_states.insert(normalized_state);

      // Add the new state and the move it took to get here
      int move_index = all_moves.size();
      all_moves.emplace_back(move, prev_move_index, depth+1);
      states_to_visit.emplace(next_state, move_index);

      // Only add one legal implicit move from this state
      if (implicit)
        break;
    }
  }

  // no solution found
  return false;
}
