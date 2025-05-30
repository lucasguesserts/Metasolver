
#include "metasolver/strategies/Greedy.h"

namespace metasolver {

list<State *> Greedy::next(list<State *> & S) {
    State & s = **S.begin();

    Action * action = best_action(s);

    if (action) {
        // const auto & b = dynamic_cast<const clpAction*>(action)->block;
        // const Space& sp = dynamic_cast<const clpAction*>(action)->space;
        // std::cout << "selected box:" << b << endl;
        // std::cout << "selected space:" << sp << endl;

        s.transition(*action);
        delete action;
        // cout << "valid_blocks:" << s.valid_blocks.size() << endl;
        // cout << (double)s.cont.getOccupiedVolume() / (double)s.cont.getVolume()  << endl;
        return S;
    } else {
        S.clear();
        return S;
    }
}

} // namespace metasolver
