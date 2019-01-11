/*
 * clpState.h
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */




#ifndef MCLPSTATE_H_
#define MCLPSTATE_H_

#include <map>
#include <string>
#include <vector>

#include "clpState.h"
#include "State.h"
#include "Block.h"


using namespace std;
using namespace metasolver;

namespace clp {

class BlockSet;
class mclpState;

bool is_constructible(const mclpState& s, const Block& b);

class mclpState : public clpState {
public:

	mclpState(const mclpState& S) : clpState(S)/*, nb_left_boxes(S.nb_left_boxes), valid_blocks(S.valid_blocks) */{


	}

	virtual State* clone() const{
		State* st=new mclpState(*this);
		return st;
	}


	friend mclpState* new_mstate(string file, int instance, double min_fr, int max_bl, bool rot, int nboxes);

	//map<const BoxShape*, int>* nb_left_boxes; //cajas que se pueden colocar en el contenedor
	//list<const Block*>* valid_blocks;
	static map<const BoxShape*, double> priority_boxes;

	void set_boxes(map<const BoxShape*, int>& boxes){

		for(auto lb:nb_left_boxes){
			if(boxes.find(lb.first)!=boxes.end())
				nb_left_boxes[lb.first]=boxes[lb.first];
			else
				nb_left_boxes[lb.first]=0;

		}
		update_valid_blocks();
	}




	//Select a proportion prop of all the boxes considering its priorities
	void select_boxes(map<const BoxShape*, int>* nb_inserted_boxes=NULL){
		double mean=0.0; //mean of priorities
		double total_p=0.0;
		int total_boxes=0;

	/*for(auto p:priority_boxes){
			total_p+=p.second * nb_left_boxes[p.first];
			total_boxes+=nb_left_boxes[p.first];
		}

		mean=total_p/total_boxes;*/

		for(auto p:priority_boxes){
			//probability of selecting a box of type p.first
			double p_i=p.second;

			double nb_boxes=0;
			for(int i=0; i<nb_left_boxes[p.first];i++){
				if((double)rand()/RAND_MAX < p_i)
					if(nb_inserted_boxes==NULL || nb_inserted_boxes->find(p.first)==nb_inserted_boxes->end())
						nb_boxes++;
			}
			nb_left_boxes[p.first]=nb_boxes;
		}

		update_valid_blocks();
	}

	//Once a satisfactory solution has been constructed, priorities of used boxed are reduced
	void update_priorities(double alpha, map<const BoxShape*, int>& nb_boxes) const{
		for(auto b:cont->nb_boxes)
			priority_boxes[b.first] *= alpha*b.second;
			//priority_boxes[b.first] *= (alpha*b.second+(nb_boxes[b.first]-b.second))/nb_boxes[b.first];

	}

	static void initalize_priorities(){
		for(auto b:priority_boxes)
			priority_boxes[b.first] = 0.5;

	}

protected:
	mclpState(Block* p) : clpState(p) {

	}


};



mclpState* new_mstate(string file, int instance, double min_fr=0.98, int max_bl=10000, bool rot=true, int nboxes=1);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
