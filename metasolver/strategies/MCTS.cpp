/*
 * MCTS.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: iaraya
 */

#include "MCTS.h"
#include "mctsNode.h"



namespace metasolver {

	void MCTS::dfsPrintChild(const mctsNode* node, ofstream& file){

		file << "{ "<<endl;
		file<<"\t \"name\":\""<<node->get_id()<<"\",";
		file<<"\t \"parent\":\""<< ((node->get_parent())? node->get_parent()->get_id():-1) <<"\",";
		file<<"\t \"mean\":\""<<node->get_mean() << "\",";
		file<<"\t \"sd\":\""<<node->get_var() <<"\",";
		file<<"\t \"mcts_value\":\""<<node->get_promise() <<"\",";
		file<<"\t \"children_size\":\""<<node->get_children().size() <<"\",";
		file<<"\t \"simulations\":\""<<node->nb_simulations <<"\",";
		file<<"\t \"ponderated_sd\":\"\",";
		file<<"\t \"depth\":\"\",";
		file<<"\t \"num_visit\":\"\"";



		if(!node->get_children().empty()){
			file<< "\t,\"children\":["<<endl;

			for(auto c:node->get_children()){
				if(c->selected==true){
				  dfsPrintChild(c,file);
				  if(c!=node->get_children().back())
					  file<<","<<endl;
				}
			}
			file<<"]";
		}
		file<<"}"<<endl;
	}

    void MCTS::pointsToTxt(mctsNode* root, int it) {
    	ofstream myfile("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
    	dfsPrintChild(root,myfile);
    }




		mctsNode* MCTS::next_node(){

			mctsNode* c=NULL;
			do {
				c= *nodes.begin();
				nodes.erase(nodes.begin());
			} while(c->get_pre_children().empty() && !nodes.empty());
			return c;

		}

    double MCTS::run(State& s, double tl, clock_t bt){
    		timelimit=tl;
			cout << "clone state" << endl;
    		s0 = s.clone();


			cout << "root creation" << endl;
    		mctsNode* root=new mctsNode(NULL,NULL);
    		//Se realizan dos simulaciones del nodo raiz
				cout << "Se realizan dos simulaciones del nodo raiz" << endl;
    		simulate(root,s0);
    		simulate(root,s0);
    		nodes.insert(root);

				int i=0;
    		while(nodes.size() > 0 &&  get_time()<timelimit){
					i++;
    			//Seleccion del nodo
    			mctsNode* n = next_node();
    			if(n==NULL) break;

				//se simula n
				simulate(n,s0);

				const list<mctsNode*>& children = n->get_children();


				if(children.size() >= 3){
					//hijos no seleccionados se simulan 1 vez mas.
					for(auto ch : children){
						if(!ch->selected){
							simulate(ch,s0); //incrementa contador del mapa de selected
							nodes.insert(ch);
						}
					}
    			}

    		}

    		//pointsToTxt(root, 0);
    		//system("firefox problems/clp/tree_plot/index.html");
    		return best_state->get_value();
  }

	void MCTS::simulate(mctsNode* n, const State* s0){


		//no hay hijos no simulados
		if(n->selected && n->get_pre_children().empty()) return;

		//se genera el estado a partir de la ra��z
		//cout << "get_state" << endl;
		State* snext=n->get_state(s0);
		//cout << "success" << endl;

		//se crean todos los hijos de n (pre-children)
		if(!n->selected){
			list< Action* > best_actions;
			get_best_actions(*snext, best_actions, 999);
			for(auto a:best_actions){
				if(n->get_children().empty() || (*a!=*n->get_children().front()->get_action())){
					mctsNode* nn=new mctsNode(n,a); //agregar nivel al nodo //incrementa contador del mapa de nodos por nivel
					n->add_pre_children(nn);
				}
				delete a;
			}
			n->selected=true;
		}

    //Se obtiene el siguiente hijo de n y se genera su estado
		mctsNode* next=n->next_child();
		if(!next){
			 delete snext;
			 return;
		 }

		//cout << "current_action:" << *next->get_action() << endl;
		snext->transition(*next->get_action());

        //se lanza un greedy (simulaci��n)
		double value=greedy.run(*snext);
		nb_simulations++;


		//se reconstruye el camino con nodos
		list<const Action*>& actions = snext->get_path();
		bool flag=false;
		for(auto a: actions){

			if(flag){
				//cout << *a << endl;
				next->add_children(new mctsNode(next, a));
				next=next->get_children().front();
			}
			if(*a==*next->get_action()) flag=true;
		}
		//cout << "value:" << value << endl;
		//se actualiza el valor en el nodo y en todos sus ancentros
		next->update_simulations(value);

		//best_state update
		if(value > get_best_value()){
		  if(best_state) delete best_state;
		  best_state = snext->clone();
		  cout << "[MCTS] new best_solution_found ("<< get_time() <<"): " << value << " "
								 << best_state->get_path().size() << " nodes" << endl;
		}

		delete snext;
	}
}
