/*
 * SearchStrategy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <time.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include "State.h"

#ifndef SEARCHSTRATEGY_H_
#define SEARCHSTRATEGY_H_

using namespace std;

namespace metasolver {



class tau_matrix{

private:
	//distribucion de feromona
	//la clave del mapa es el estado codificado
	//el valor es un vector de pares (media del valor del parámetro, cantidad de veces modificado)
	map<long, vector<pair <double, int> > > ph_distribution;
	vector<pair <double, double> > parameter_ranges;

	double fRand(double fMin, double fMax)
	{
	    double f = (double)rand() / RAND_MAX;
	    return fMin + f * (fMax - fMin);
	}


public:

	tau_matrix(vector<pair <double, double> >& p) : parameter_ranges(p) {

	}

	//samplea parametros de acuerdo a distribucion normal asociada al estado
	vector <double> sample_param_vector(const State* s){

		//TODO: se obtiene el el estado codificado
		long state_code=0;// = s->get_code();
		vector <double> values(parameter_ranges.size());

		if(ph_distribution.find(state_code)!=ph_distribution.end()){
			vector<pair <double, int> >& dist_params = ph_distribution[state_code];
			for(int i=0; i<dist_params.size();i++ ){
				//TODO: samplear de normal con media dist_params[i].first y desviación M/dist_params[i].second
				//truncar dentro del rango [parameter_ranges[i].first,parameter_ranges[i].second]
				//values[i]= truncated_normal(dist_params[i].first, dist_params[i].second, parameter_ranges[i]);
				values[i]=fRand(parameter_ranges[i].first,parameter_ranges[i].second);
			}

		}else //random values in the range
			for(int i=0;i<values.size();i++)
				values[i]=fRand(parameter_ranges[i].first,parameter_ranges[i].second);

		return values;

	}

	//actualiza los parametros de distribucion (media y n) asociados al estado de
	// acuerdo a los valores de los parametros
	void add_pheromone(const long state_code, const vector<double>& parameter_values){

		if(ph_distribution.find(state_code)!=ph_distribution.end()){
			vector<pair <double, int> >& dist_params = ph_distribution[state_code];
			for(int i=0; i<dist_params.size();i++ ){
				double media = dist_params[i].first;
				double n = dist_params[i].second;
				dist_params[i].first = (n*media+parameter_values[i])/(n+1);
				dist_params[i].second++;
			}
		}else{
			vector<pair <double, int> > dist_params(parameter_values.size());
			for(int i=0; i<dist_params.size();i++ ){
				dist_params[i].first = parameter_values[i];
				dist_params[i].second = 1;
				ph_distribution[state_code]=dist_params;
			}
		}

	}
};

//TODO: refactorizar
class SearchStrategy {
public:
	SearchStrategy(ActionEvaluator* evl=NULL, double aco_alpha=0.0, double aco_beta=0.0, tau_matrix* tauM=NULL) :
		evl(evl), best_state(NULL), timelimit(0.0), begin_time(clock()), aco_alpha(aco_alpha), aco_beta(aco_beta), tauM(tauM) {} ;

	virtual ~SearchStrategy() {

	}

	double get_time(){
		return (double(clock()-begin_time)/double(CLOCKS_PER_SEC));
    }



	/*
	 * Initialize the variables of the specific strategy
	 */
	virtual void initialize (State* s=NULL) { best_state=s; }

	virtual void clean(list<State*>& S){ }

	/**
	 * Run the strategy
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock()){
		list<State*> S;
		S.push_back(&s);
		initialize (&s);

		return run(S, tl, bt);
	}

	/**
	 * Run the strategy
	 */
	virtual double run(list<State*>& S, double tl=99999.9, clock_t bt=clock()){
		begin_time=bt;
		timelimit=tl;

		do{
			if(timelimit != 0.0 && get_time()>timelimit ){
				clean(S);
				return get_best_value() ;
			}
			list<State*> R=next(S);
			clean(S);
			S=R;
		}while(S.size()>0);

		return get_best_value() ;
	}

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<State*> next(list<State*>& S) { };

	virtual double get_best_value() const {
		if(best_state)
			return best_state->get_value();
		else return 0;
	}

	virtual const State* get_best_state() {
		return best_state;
	}

	/*
	 * Esta funcion "duplica" el esfuerzo de la estrategia.
	 * Si el esfuerzo es duplicado satisfactoriamente retorna true.
	 * (Implementar si la estrategia es compatible con el
	 * algoritmo de doble esfuerzo.)
	 */
	virtual bool double_effort() { return false; }

	int get_best_actions_ACO(const State& s, list< Action* >& bactions, int n);

	/**
	 * \brief return the n best actions according to the ActionEvaluator
	 */
	virtual int get_best_actions(const State& s, list< Action* >& bactions, int n);

protected:

	/*
	 * \brief return the best action according to the ActionEvaluator
	 */
	virtual Action* best_action(const State& s);

	//virtual Action* best_action_ACO(const State& s);



	double timelimit;
	State* best_state;
	clock_t begin_time;

	ActionEvaluator* evl;

	double aco_alpha;
	double aco_beta;


	tau_matrix* tauM;

};

} /* namespace clp */

#endif /* SEARCHSTRATEGY_H_ */
