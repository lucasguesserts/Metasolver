/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include "clpState.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSGMOP.h"
#include "args.hxx"

bool global::TRACE = false;

using namespace std;


int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-MOP CLP *********.", "BSG-MOP Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<string> _format(parser, "string", "Format: (BR, BRw, 1C)", {'f'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::ValueFlag<double> _maxtheta(parser, "double", "ponderation of the weight of a box for maximizing the total weight", {"maxtheta"});


	args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
	args::Flag trace(parser, "trace", "Trace", {"trace"});
	args::Positional<std::string> _file(parser, "instance-set", "The name of the instance set");

		cout.precision(8);
		try
		{
			parser.ParseCLI(argc, argv);

		}
		catch (args::Help&)
		{
			std::cout << parser;
			return 0;
		}
		catch (args::ParseError& e)
		{
			std::cerr << e.what() << std::endl;
			std::cerr << parser;
			return 1;
		}
		catch (args::ValidationError& e)
		{
			std::cerr << e.what() << std::endl;
			std::cerr << parser;
			return 1;
		}

		string file=_file.Get();
		int inst=(_inst)? _inst.Get():0;
		double min_fr=(_min_fr)? _min_fr.Get():0.98;
		int maxtime=(_maxtime)? _maxtime.Get():100;

		double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04, maxtheta=0.0;
		if(_maxtime) maxtime=_maxtime.Get();
		if(_alpha) alpha=_alpha.Get();
		if(_beta) beta=_beta.Get();
		if(_gamma) gamma=_gamma.Get();
		if(_delta) delta=_delta.Get();
		if(_p) p=_p.Get();
		if(_maxtheta) maxtheta=_maxtheta.Get();

		string format="BR";
		if(_format) format=_format.Get();

		clpState::Format f;
		if(format=="BR")
			f=clpState::BR;
		else if(format=="BRw")
			f=clpState::BRw;
		else if(format=="1C")
			f=clpState::_1C;

		int seed=(_seed)? _seed.Get():1;
		srand(seed);

		global::TRACE = trace;

   // cout << "cargando la instancia..." << endl;

    //a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "File("<< format <<"): " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;



    clpState* s0 = new_state(file,inst, min_fr, 10000, f);
	cout << "Dimensions: " << s0->cont->getL() << " x " << s0->cont->getW() << " x " << s0->cont->getH() << endl;
    cout << "Number of generated blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();


    cout << endl << "***** Creating the heuristic function VCS *****" << endl;
    cout << "alpha: " << alpha ;
    cout << ", beta: " << beta;
    cout << ", gamma: " << gamma;
    cout << ", p: " << p;
    cout << ", delta: " << delta << endl;
    cout << "maxtheta: " << maxtheta << endl;

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, 0.0, 0.0, maxtheta);

    SearchStrategy *gr = new Greedy (vcs);

    BSG_MOP *bsg= new BSG_MOP(vcs,*gr, 4, 0.0, 0, (maxtheta>0.0) );

    SearchStrategy *de= new DoubleEffort(*bsg);

	State& s_copy= *s0->clone();

	cout << "***** Running the solver BSGMOP solver *****" << endl;
    double eval = 1-de->run(s_copy, maxtime, begin_time) ;

    cout << "pareto_front" << endl;
    map< pair<double, double>, State*> pareto = bsg->get_pareto_front();
    double x_old=0.0;
    double hv = 0.0;
    int i=0; double best_volume, best_volume_weight, best_weight;
    for(auto point : pareto){
    	if(i==1) best_weight=point.first.second;

    	if(i==pareto.size()-2) {
    		best_volume=point.first.first;
    		best_volume_weight=point.first.second;
    	}

    	hv += (point.first.first - x_old) * point.first.second;
    	x_old = point.first.first;
    	cout << point.first.first << "," << point.first.second << endl;
    	i++;
    }
    cout << "best_volume best_weight hypervolume" << endl;
    cout << best_volume << " " << best_volume_weight << " " << best_weight<< " " << hv <<  " "<<  pareto.size()-2 <<endl;
/*
 *
	const AABB* b = &dynamic_cast<const clpState*>(gr->get_best_state())->cont.blocks->top();
	while(dynamic_cast<const clpState*>(gr->get_best_state())->cont.blocks->has_next()){
		cout << *b << ":" << b->getVolume() << "(" << b->getOccupiedVolume() << ")" << endl;
		b = &dynamic_cast<const clpState*>(gr->get_best_state())->cont.blocks->next();
	}
*/

	//dynamic_cast<const clpState*>(de->get_best_state())->cont.MatLab_print();


}
