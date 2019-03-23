/*
 * A_star.h
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#ifndef STRATEGIES_A_STAR_H_
#define STRATEGIES_A_STAR_H_


#include <SearchStrategy.h>
#include <State.h>
#include <queue>
#include <unordered_set>
#include <set>
#include <fstream>
#include "mctsNode.h"

namespace metasolver {

    class MCTS : public SearchStrategy {
        public:

    	MCTS(ActionEvaluator* evl, SearchStrategy& greedy) :
    		SearchStrategy(evl), greedy(greedy), nb_simulations(0){}

	    virtual ~MCTS() {}



	    void dfsPrintChild(const mctsNode* node, ofstream& file);

	    void pointsToTxt(mctsNode* root, int it);

		/**
		 * Run the strategy and return the best found value
		 */
		 double run(State& s, double tl=99999.9, clock_t bt=clock());

		mctsNode* next_node();

		// performs a simulation and returns the corresponding child
		void simulate(mctsNode* n, const State* s0);

    double nb_simulations;



		private:

    const State* s0;

		SearchStrategy& greedy;

		set< pair<double,double> > evals;

		//nodos seleccionables del arbol ordenados por promesa
		std::multiset<mctsNode* , Compare> nodes;

};



} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */