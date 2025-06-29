#include "move.h"

using namespace std;

Move::Move(int from, int to, int size, bool implicit) : from(from), to(to), size(size), implicit(implicit) {
}

ostream& operator<<(ostream& os, const Move& move) {
  os << "<move " << move.from << ',' << move.to;
  if (move.size != 1)
    os << " (" << move.size << ')';
  if (move.implicit)
    os << " implicit";
  os << '>';
  return os;
}
