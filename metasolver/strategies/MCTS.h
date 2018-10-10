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
#include <fstream>

//TODO: Al seleccionar un nodo se debe aplicar el mecanismo correspondiente para poder calcular promise
//TODO: Implementar funcion State::promise que calcula la probabilidad de que si lanzo una nueva
//simulacion del nodo, la evaluacion sera mayor a la mejor obtenida más un delta pequeño (0.0001).
//Para calcular el promise se necesita saber el promedio de las simulaciones desde el nodo y
//la desviacion estandar. Por lo que se requieren a lo menos 2 evaluaciones.
//Si el nodo tiene solo una evaluacion, le copia la desviacion a su hermano que tenga al menos 2.
namespace metasolver {

    class Compare{
    public:
        bool operator() (const State* a , const State* b){
            if(a->get_promise() > b->get_promise() ) return false;
            else if(a->get_promise() == b->get_promise()){
                if(a->get_promise()  > b->get_promise() ) return false;
                else return true;
            }
            return true;
        }
    };

    class MCTS : public SearchStrategy {
        public:

    	MCTS(ActionEvaluator* evl, SearchStrategy& greedy, double eps) : SearchStrategy(evl), greedy(greedy), eps(eps) {}

	    virtual ~MCTS() {}


	    void dfsPrintChild(const State* node, ofstream& file){
	    	file << "{ "<<endl;
	    	file<<"\t \"name\":\""<<node->get_id()<<"\",";
	    	file<<"\t \"parent\":\""<<node->get_parent()->get_id() <<"\",";
	    	file<<"\t \"mean\":\""<<node->get_mean() <<"("<< node->get_value() <<")\",";
	    	file<<"\t \"sd\":\""<<node->get_var() <<"\",";
	    	file<<"\t \"mcts_value\":\""<<node->get_promise() <<"\",";
	    	file<<"\t \"children_size\":\""<<node->get_children_size() <<"\",";
	    	file<<"\t \"ponderated_sd\":\"\",";
	    	file<<"\t \"depth\":\"\",";
	    	file<<"\t \"num_visit\":\"\"";
	    	file<< "\t,\"simulations\":[]"<<endl;


	    	if(!node->get_children().empty()){
	    		file<< "\t,\"children\":["<<endl;
	    		for(auto c:node->get_children()){
	    			dfsPrintChild(c,file);
	    			if(c!=node->get_children().back())
	    			    file<<","<<endl;
	    		}
	    		file<<"]";
	    	}
	    	file<<"}"<<endl;
	    }

	    void pointsToTxt(State* root, int it) {
	    	ofstream myfile("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
	    	dfsPrintChild(root,myfile);
	    }

	/**
	 * Run the strategy and return the best found value
	 */
	 double run(State& s, double tl=99999.9, clock_t bt=clock()){
		timelimit=tl;
		std::priority_queue<const State* , vector<const State*>, Compare> q;
		State* s0 = s.clone();
		q.push(s0);

		int i=0;


		while(q.size() > 0 &&  get_time()<timelimit){
			bool change_best=false;

			const State* s = q.top(); q.pop();
			//cout << s->get_promise() << "," << s->get_children_size() << "," <<q.size() << endl;

			const State* s2 = simulate(s, change_best);
			if(s2 == NULL){
				if(s->get_promise() == 0 && q.size()>1){
					//delete s;
					continue;
				}
			}

			if(s->get_children_size() == 3){
				for(auto ch : s->get_children()){
					simulate(ch, change_best);
					simulate(ch, change_best);
					if(ch->get_children_size() >= 2){
						ch->calculate_promise(get_best_value()+eps);
						q.push(ch);
					}
				}
			}else if(s2 && s->get_children_size() > 3){
				simulate(s2, change_best);
				simulate(s2, change_best);
				if(s2->get_children_size() >= 2){
					s2->calculate_promise(get_best_value()+eps);
					q.push(s2);
				}

			}

			if(s->get_children_size() >= 2)
				s->calculate_promise(get_best_value()+eps);

			q.push(s);

			if(change_best/* || i%500==10*/) update_queue(q);

			i++;
		}

		//pointsToTxt(s0, 0);
		//system("firefox problems/clp/tree_plot/index.html");

		return best_state->get_value();
	}

	// update to promise list
	void update_queue(std::priority_queue<const State* , vector<const State*>, Compare>& q)
	{
		std::priority_queue<const State* , vector<const State*>, Compare> aux;
		while(!q.empty()){
			if(q.top()->get_children_size() >= 2)
				q.top()->calculate_promise(get_best_value()+eps);

			//discard some nodes of the tree
			//if(q.top()->get_promise() > 0.001)
				aux.push(q.top());
			/*else{
				for(auto ch:q.top()->get_children())
					if(ch->get_children_size()==0) delete ch;

				delete q.top();
			}*/

			q.pop();
		}
		q=aux;
	}

    // performs a simulation and returns the corresponding child
    State* simulate(const State* s, bool change_best){

    	double value;
    	State* s2, *s3;

        while(true){
			int size = s->get_children_size();
			//TODO: optimizar!

			list< Action* > best_actions;
			get_best_actions(*s, best_actions, size+1);
			if(best_actions.size()<size+1){
				s->set_promise(0.0);
				return NULL;
			}

			s2=s->clone();
			s2->transition(*best_actions.back());
			s3=s2->clone();
			value=greedy.run(*s3);


			if(evals.find( make_pair(value, s3->get_value2()))==evals.end()){
				evals.insert( make_pair(value,s3->get_value2()) );
				break;
			}else{
				delete s2;
				delete s3;
				s->update_values(value);
				return NULL;
			}
        }



        //best_state update
        if(value > get_best_value()){
          if(best_state) delete best_state;
          best_state = s3->clone();
          cout << "[MCTS] new best_solution_found ("<< get_time() <<"): " << value << " "
                      			 << best_state->get_path().size() << " nodes" << endl;
          change_best=true;
        }
        delete s3;

        s2->set_mean(value);
        s->add_children(s2);
        s->update_values(value);

        return s2;
    }



    private:

	SearchStrategy& greedy;

	set< pair<double,double> > evals;

	double eps;

};



} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
