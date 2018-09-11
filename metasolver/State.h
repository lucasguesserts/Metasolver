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




#ifndef STATE_H_
#define STATE_H_


#include "ActionEvaluator.h"
#include <algorithm>


using namespace std;
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

	State() : parent (NULL){}

	virtual State* clone() const = 0;


	State(const State& S) : parent(&S){
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

	//FIXME: THIS CLASS?
	double promise(double best_value) 
	{
		double z_value=stadistic_test(best_value);
		students_t dist( children.size() ) ;
		promise= cdf(dist, z_value);
	}

	double stadistic_test(double best_value)
	{
		double z = ( ( mean - best_value) / sd );
		z = z* sqrt(children.size(),2); //FIXME: SQRT ? POW

		return z;
	}
	


	virtual void get_actions(list< Action* >& actions) const = 0;

	virtual Action* next_action(State& final){
	    if(get_path().size() >= final.get_path().size() ) return NULL; 
	    
	    list< const Action* >::iterator act=final.get_path().begin();
	    advance(act,get_path().size());
    	
    	return (*act)->clone();
	}

	bool is_root(){ return (path.size()==0); }

	list<const Action*>& get_path() const{ return path;}

	virtual void print() {  }

	/**
	 *  Probabilidad de generar simulaciones mejores a la mejor de acuerdo a mean y sd
	 */
	virtual void calculate_promise(double best_value) const {

		return 0.0;
	}

	// Actualiza los valores mean y sd de acuerdo al nuevo valor
	virtual void update_values(double new_value){
		if(children.size()) mean=new_value;
		else
			mean = (mean*(children.size()-1)+new_value)/children.size();
			var=( (var*children.size()-2) + pow( (new_Value-mean),2) ) / ( children_size()-1 ); //actualiza la varianza
	}



	const list<const State*>& get_children() const { return children;}

	void add_children(State* s){ children.push_back(s);}



protected:

	virtual void _transition(const Action& action) = 0;

	const State* parent;
	//bool root;

	//list of actions for reconstructing the state from scratch
	mutable list<const Action*> path;

	list<const State*> children;

	double mean; // mean arithmetics
	double sd; // standard deviation
	double promise; // value can be the best of the best
	double var; // variance
	double delta=0.0001; // 

};


}

#endif /* STATE_H_ */
