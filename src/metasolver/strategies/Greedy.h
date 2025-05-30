#include "../ActionEvaluator.h"
#include "../SearchStrategy.h"

#ifndef GREEDY_H_
#define GREEDY_H_

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

#endif /* GREEDY_H_ */
