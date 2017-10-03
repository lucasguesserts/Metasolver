/*
 * BSGMOP.h
 *
 *  Created on: 13 sept. 2017
 *      Author: iaraya
 */

#ifndef PLUGINS_MOP_BSGMOP_H_
#define PLUGINS_MOP_BSGMOP_H_

#include <list>
#include "../../../SearchStrategy.h"
#include "../../../strategies/BSG.h"

using namespace std;
namespace clp {

class BSG_MOP : public BSG {
public:
	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	BSG_MOP(SearchStrategy& greedy, int beams, MO_ActionEvaluator& evl, double p_elite=0.0, int max_level_size=0) :
		BSG(greedy, beams, p_elite, max_level_size), evl(evl){

	}

	virtual ~BSG_MOP();

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<State*> next(list<State*>& S)=0 ;

	//void BSG_MOP::Non_Dominanted_sort(int N,list< pair<State*,State*> >& sorted_list);

private:

	//conjunto de soluciones no dominadas
	list<State*> NDS;

	//evaluador de acciones con parametro alpha
	MO_ActionEvaluator& evl;
	void update(list<State*>& NDS, State& state_copy, double valuef1, double valuef2);
	void select_coeff(list<double>& coeff, int n);
};

struct nd_sort {
  bool operator() (const pair<double, double>& p1, const pair<double, double>& p2) const
  {
	  //no se aceptan repetidos
	  if(p1.first == p2.first && p1.second == p2.second) return false;

	  //p1 domina a p2
	  if(p1.first >= p2.first && p1.second >= p2.second) return true;

	  if(p1.first < p2.first) return true;

	  return false;

  }
};

} /* namespace clp */

#endif /* PLUGINS_MOP_BSGMOP_H_ */
