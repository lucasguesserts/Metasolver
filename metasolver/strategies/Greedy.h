/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "../ActionEvaluator.h"
#include "../SearchStrategy.h"


#ifndef GREEDY_H_
#define GREEDY_H_

using namespace std;

namespace metasolver {

class Greedy : public SearchStrategy {
public:

	Greedy(ActionEvaluator* evl, double aco_alpha, double aco_beta) : SearchStrategy(evl, aco_alpha, aco_beta) { }

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

};


} /* namespace clp */

#endif /* GREEDY_H_ */
