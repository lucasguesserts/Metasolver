#include "metasolver/strategies/BSG.h"

namespace metasolver {

BSG::~BSG() {
}

list<State *> BSG::next(list<State *> & S) {

    // cout << "next" << endl;
    // no hay mas estados en el arbol
    if (S.size() == 0) return S;

    // cout << "S size = " << S.size() << endl;
    ++this->expand_calls;
    // se expanden los nodos de la lista S
    int i = 0;
    for (list<State *>::iterator itS = S.begin(); itS != S.end() && get_time() <= timelimit; itS++, i++) {
        // cout << "number of states: " << S.size() << endl;
        State & state = **itS;
        // cout << state.get_value() << endl;

        if (state.is_root()) cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;

        // se obtiene la lista de las mejores acciones a partir del estado actual
        list<Action *> best_actions;

        // each level of the search tree should explore max_level_size nodes, thus...
        int w = (double)max_level_size / (double)S.size() + 0.5;

        // cout << "\n\nselect blocks for expansion" << endl;
        get_best_actions(state, best_actions, w);

        // the actions are evaluated and saved in the sorted map
        list<Action *>::iterator it = best_actions.begin();

        for (; it != best_actions.end() && get_time() <= timelimit; it++) {

            State & state_copy = *state.clone();
            state_copy.transition(**it);
            // cout << state_copy.get_value() << endl;
            delete *it;

            // cout << "--- start greedy ---" << endl;
            // cout << "\n\nselect blocks for greedy search" << endl;
            double value = greedy.run(state_copy, timelimit, timer);
            ++this->greedy_calls;
            // cout << "--- end greedy ---" << endl << endl;

            // best_state update
            if (value > get_best_value()) {
                if (best_state) delete best_state;
                best_state = state_copy.clone();
                cout << "[BSG_path] new best_solution_found (" << get_time() << "): " << value << " " << best_state->get_path().size() << " nodes" << endl;
                // print all best state actions
                // for (auto const & a : best_state->get_path()) {
                // 	const clp::clpAction& act = *dynamic_cast<const clp::clpAction*> (a);
                // 	cout << "block: " << act.block.getL() << ", "<< act.block.getW() << ", "<< act.block.getH();
                // 	cout << " -- in -- ";
                // 	cout << "empty space: " << act.space.getL() << ", " << act.space.getW() << ", " << act.space.getH();
                // 	cout << " -- on -- ";
                // 	auto anchor = act.space.get_anchor();
                // 	cout << "anchor: " << anchor[0] << ", " << anchor[1] << ", " << anchor[2];
                // 	cout << " -- at -- ";
                // 	auto loc = act.space.get_location(act.block);
                // 	cout << "location: " << loc.getX() << ", " << loc.getY() << ", " << loc.getZ();

                // 	cout << endl;
                // }
            }

            if (state_actions.find(-value) == state_actions.end()) {
                // cout << value << endl;
                state_actions[-value] = make_pair(&state, &state_copy);
            } else delete &state_copy;
        }
    }

    // if(global::TRACE){

    /* cout << "pre_filtered" << endl;
     for(auto states:state_actions)
             cout << states.first<< endl;

     cout << endl;*/
    //}

    list<State *> l = get_next_states(state_actions);
    // state_actions.clear();

    // siguiente generacion de estados
    return l;
}

} // namespace metasolver
