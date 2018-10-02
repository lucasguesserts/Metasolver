/*
 * State.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <functional>
#include <math.h>

#include "boost/math/distributions/students_t.hpp"



#ifndef STATE_H_
#define STATE_H_


#include "ActionEvaluator.h"
#include <algorithm>


using namespace std;
using namespace boost::math;

namespace metasolver{

class Action{
public:
	virtual Action* clone() const=0;

	virtual ~Action(){}

};



/**
 * Represent a partial solution of the problem. It consists in
 * the container (block) with the located blocks, a map of the left boxes and
 * a list of valid blocks which can be put in the container.
 */
class State {
public:

	State() : parent (NULL), id(count_states++){}

	virtual State* clone() const = 0;


	State(const State& S) : parent(&S), id(count_states++), var(0.0), mean(0.0), promise(0.0), children_size(0){

		list<const Action*>::iterator it=S.get_path().begin();
		for(;it!=S.path.end();it++)
			path.push_back((*it)->clone());
	}

	virtual State* create_neighbor(State* s0){
		return NULL;
	}

	virtual ~State() {
		while(!path.empty()){
			delete path.front();
			path.pop_front();
		}
	}

	/**
	 * The value of the objective function
	 */
	virtual double get_value() const = 0;

	/**
	 * The value of the second objective function
	 */
	virtual double get_value2() const { return 0.0; }

	void transition(const Action& action) {
		path.push_back(action.clone());
		_transition(action);
	}
	
	/*
	* Rearranges the elements in the path randomly
	*/
	virtual int shuffle_path(){
	    cout << "why!!" << endl;
	    return 0;
	   // std::random_shuffle ( path.begin(), path.end() );
	}

	// Probabilidad de generar simulaciones mejores a la mejor de acuerdo a mean y sd
	//FIXME: THIS CLASS?
	//TODO:eliminar cout
	void calculate_promise(double best_value) const{
		
		if(var==0.0) {promise=0; return;}

		double z_value=stadistic_test(best_value+delta);
		students_t dist( children.size() );

		promise= cdf(dist, z_value);
		cout << "[SATE]{calculate_promise} BEST_VALUE: ( " << best_value << ") Z_VALUE: ( "<< z_value << ") " <<
				/*"DIST: ( "<< dist <<")*/ " PROMISE( " <<promise<<")" << endl ;
	}

	//TODO: eliminar los cout
	double stadistic_test(double best_value) const {

		double z = ( ( mean - best_value) / sqrt(var) );

		cout <<"[STATE] {stadistic_test} ESTADO: " << id << "BEST VALUE: ("<< best_value <<")	MEAN: ("<<mean<<") VAR: ("<<var <<") BEFORE Z: ("<< z <<")" << endl;;

		z = z* sqrt(children.size()); //FIXME: SQRT ? POW

		cout << "Z afeter: ("<< z <<")"<< endl;

		return z;
	}



	virtual void get_actions(list< Action* >& actions) const = 0;

	virtual Action* next_action(State& final);

	bool is_root(){ return (path.size()==0); }

	list<const Action*>& get_path() const{ return path;}

	virtual void print() {  }

	// Actualiza los valores mean y sd de acuerdo al nuevo valor
	//TODO: eliminar cout
	virtual void update_values(double new_value) const{
		if(children.size()==0){
			cout << "[STATE] {update_values}¿children==0? MEAN: " << mean << "NEW_VALUE: " << new_value << endl;

			mean=new_value;
		}else if(children.size() >= 1){

			cout << "[STATE] {update_values}¿children>=1? MEAN before: " << mean<< endl;

			mean = (mean*(children.size()-1)+new_value)/children.size();

			cout << "[STATE] {update_values} MEAN after: " << mean<< endl;
		}

		if(children.size()>=2){
			cout << "[STATE] {update_values} ¿children>=2? VAR before: "<< var<< endl;

			var=( (var*(children.size()-2)) + pow( (new_value-mean),2) ) / ( children.size()-1 ); //actualiza la varianza
			if(var<=1e-6) var=1e-6;

			cout << "[STATE] {update_values} ¿children>=2? VAR after: "<< var<< endl;
		}
	}

	inline double get_promise() const{return promise;}

	double get_mean() const {return mean;}
	double get_var() const {return var;}

	const list<const State*>& get_children() const { return children;}

	const int get_children_size() const { return children_size;}

	void add_children(State* s) const{ children.push_back(s); children_size++;}


	int get_id() const { return id; }

	const State* get_parent() const{ return parent; }


	static int count_states;

	mutable int children_size;

protected:

	virtual void _transition(const Action& action) = 0;

	const State* parent;
	//bool root;

	//list of actions for reconstructing the state from scratch
	mutable list<const Action*> path;
	mutable list<const State*> children;
	mutable double var; // variance
	mutable double mean; // mean arithmetics
	//double sd; // standard deviation
	mutable double promise; // value can be the best of the best


	double delta=0.0001; // 
	int id;



};


}

#endif /* STATE_H_ */
