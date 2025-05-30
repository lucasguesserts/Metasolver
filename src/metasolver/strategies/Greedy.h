#ifndef GREEDY_H_
#define GREEDY_H_

#include "metasolver/ActionEvaluator.h"
#include "metasolver/SearchStrategy.h"

using namespace std;

namespace metasolver {

class Greedy : public SearchStrategy {
public:
    Greedy(ActionEvaluator * evl)
        : SearchStrategy(evl) {}

    /**
     * Performs an iteration of the strategy
     */
    virtual list<State *> next(list<State *> & S);
};

} // namespace metasolver

#endif
