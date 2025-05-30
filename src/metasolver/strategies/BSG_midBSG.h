#ifndef BSG_MIDBSG
#define BSG_MIDBSG

#include <iostream>
#include <list>

#include "metasolver/strategies/BSG.h"

using namespace std;

namespace metasolver {

class BSG_midBSG : public BSG {
public:
    BSG_midBSG(ActionEvaluator * evl, SearchStrategy & greedy, int beams, double p_elite = 0.0, int max_level_size = 0)
        : BSG(evl, greedy, beams, p_elite, max_level_size) {}

    virtual ~BSG_midBSG() {}

    /**
     * Run the strategy
     */
    virtual double run(State & s, double tl = 99999.9, shared_ptr<boost::timer::cpu_timer> t = nullptr) {

        State * s0 = s.clone();

        list<State *> S;

        BSG::run(s, tl, t);

        map<double, pair<State *, State *>>::iterator state_action = state_actions.begin();

        for (int i = 0; state_action != state_actions.end(); i++, state_action++) {
            State * s1 = state_action->second.second->create_neighbor(s0);

            state_action->second.first = s1;
            S.push_back(s1);
            break;
        }
        delete s0;

        cout << "midBSG(" << S.size() << ")" << endl;
        return BSG::run(S, tl, t);
    }
};

} // namespace metasolver

#endif
