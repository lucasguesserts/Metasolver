#include <iostream>
#include <list>
#include <memory>
#include <stdio.h>
#include <time.h>

#include <boost/timer/timer.hpp>

#include "State.h"

#ifndef SEARCHSTRATEGY_H_
#define SEARCHSTRATEGY_H_

using namespace std;

namespace metasolver {

// TODO: refactorizar
class SearchStrategy {
public:
    SearchStrategy(ActionEvaluator * evl = NULL)
        : evl(evl)
        , best_state(NULL)
        , timelimit(0.0)
        , timer(nullptr) {};

    virtual ~SearchStrategy() {
    }

    double get_time() {
        return static_cast<double>(timer->elapsed().user + timer->elapsed().system) / 1.0e+9;
    }

    /*
     * Initialize the variables of the specific strategy
     */
    virtual void initialize(State * s = NULL) { best_state = s; }

    virtual void clean(list<State *> & S) {}

    /**
     * Run the strategy
     */
    virtual double run(State & s, double tl = 99999.9, shared_ptr<boost::timer::cpu_timer> t = nullptr) {
        list<State *> S;
        S.push_back(&s);
        initialize(&s);

        return run(S, tl, t);
    }

    /**
     * Run the strategy
     */
    virtual double run(list<State *> & S, double tl = 99999.9, shared_ptr<boost::timer::cpu_timer> t = nullptr) {
        timer = t;
        timelimit = tl;

        do {
            if (timelimit != 0.0 && get_time() > timelimit) {
                clean(S);
                return get_best_value();
            }
            list<State *> R = next(S);
            clean(S);
            S = R;
        } while (S.size() > 0);

        return get_best_value();
    }

    /**
     * Performs an iteration of the strategy
     * @returns true if the search strategy has not finished yet
     */
    virtual list<State *> next(list<State *> & S) {}

    virtual double get_best_value() const {
        if (best_state)
            return best_state->get_value();
        else return 0;
    }

    virtual const State * get_best_state() {
        return best_state;
    }

    /*
     * Esta funcion "duplica" el esfuerzo de la estrategia.
     * Si el esfuerzo es duplicado satisfactoriamente retorna true.
     * (Implementar si la estrategia es compatible con el
     * algoritmo de doble esfuerzo.)
     */
    virtual bool double_effort() { return false; }

    /**
     * \brief return the n best actions according to the ActionEvaluator
     */
    virtual int get_best_actions(const State & s, list<Action *> & bactions, int n);

protected:
    virtual Action * best_action(const State & s);

    double timelimit;
    State * best_state;
    shared_ptr<boost::timer::cpu_timer> timer;

    ActionEvaluator * evl;
};

} // namespace metasolver

#endif
