/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <fstream>
#include "args.hxx"
//#include "objects/State.cpp"
#include "mclp-state.h"
#include "BlockSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"

bool global::TRACE = false;

using namespace std;


void dfsPrintChild(const State* node, ofstream& file){
	file << "{ "<<endl;
	file<<"\t \"name\":\""<<node->get_id()<<"\",";
	file<<"\t \"parent\":\""<<node->get_parent()->get_id() <<"\",";
	file<<"\t \"value\":\""<<node->get_value() <<"\",";
	file<<"\t \"sd\":\"\",";
	file<<"\t \"mcts_value\":\"\",";
	file<<"\t \"stimated_sd\":\"\",";
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
	ofstream scp("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
	dfsPrintChild(root,scp);
}

void exportToTxtSCP(list < pair <double, map<const BoxShape*, int>> >* bins,
		map<const BoxShape*, list<int> >* used_boxes, long int nb_boxes){

	ofstream scp ("bins_scp.txt");

	if (scp.is_open()){
		long int total_boxes = 0;

		//Bins quantity
		scp << " " << nb_boxes << " ";

		//Boxes quantity
		/*for(auto bin: *bins){
			total_boxes += bin.second.size();
		}
		scp << total_boxes << "\n";*/

		scp << bins->size() << "\n";

		//Matrix cost by boxes
		long int cont = 0;
		for(auto bin: *bins){
			//for(auto box: bin.second){
				if(cont >= 12){
					scp << "\n";
					cont = 0;
				}
				scp << " 1";
				cont += 1;
			//}
		}
		scp << "\n";

		//Boxes quantity in a set and then sets boxes
		for(auto box_el: *used_boxes){
			scp << " " << box_el.second.size() << "\n";
			for(auto bin_id: box_el.second){
				scp << " " << bin_id + 1;
			}
			scp << "\n";
		}
/*
		for(auto bin: *bins){
			scp << " " << bin.second.size() << "\n";
			for(auto box: bin.second){
				scp << " " << box.first->get_id() + 1;
			}
			scp << "\n";
		}*/
		scp.close();
	} else cout << "Unable to open file";
}

/*Clonar estado inicial
Aplicar Greedy y obtener contenedor
Verificar si el contenedor ya existe, si no agregarlo a lista de contenedores (bins)
Reducir peso de las cajas utilizadas en el contenedor
Volver a 1*/

int solve(Greedy* gr, BSG *bsg, mclpState* s0, int nbins, double pdec){
	mclpState::initalize_priorities();
	//multimap<double, map<const BoxShape*, int> > bins;
	list < pair <double, map<const BoxShape*, int>> > bins;
	map<const BoxShape*, list<int> > used_boxes;
	int box_quantity = 0;

	for(int i=0; i<nbins; i++){
		//copia el estado base
		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());

		//usa greedy para llenar contenedor
		double eval=gr->run(s_copy);

		//se actualizan las prioridades
		dynamic_cast<const mclpState*>(gr->get_best_state())->update_priorities(pdec,
				&dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes);

		//se almacena el bin en el mapa
		bins.push_back(make_pair(eval, dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes));


		for(auto box:dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes){
			//cout << box.first->get_id() << "(" << box.first->get_priority() << "),";
			cout << box.first->get_id() << " ";
			used_boxes[box.first].push_back(i);
		}
		cout << endl;


	}

	cout << "used_boxes" << endl;
	for(auto box: used_boxes)
		cout << box.first->get_id() << "(" << box.second.size() << ")," ;
	cout << endl;

	exportToTxtSCP(&bins, &used_boxes, s0->nb_left_boxes.size());

	return bins.size();

}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::Flag _rotate(parser, "rotate", "rotations are allowed?", {"rot"});
	args::ValueFlag<int> _nboxes(parser, "int", "Number of boxes for each type", {"nboxes"});
	args::ValueFlag<int> _nbins(parser, "int", "Number of bins to be generated", {"nbins"});
	args::ValueFlag<double> _pdec(parser, "int", "Priority decreasing ratio", {"pdec"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::Flag _plot(parser, "double", "plot tree", {"plot"});
	//args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
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

	double alpha=3.0, beta=2.0, gamma=0.5, delta=1.0, p=0.04, pdec=0.8;
	int nbins=1000;
	int nboxes=1;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();
	if(_nboxes) nboxes=_nboxes.Get();
	if(_pdec) pdec=_pdec.Get();
	if(_nbins) nbins=_nbins.Get();

	int seed=(_seed)? _seed.Get():1;
	srand(seed);

	global::TRACE = trace;

 cout << "cargando la instancia..." << endl;

//a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;
	cout << "Rotations allowed:" << _rotate << endl;
	cout << "N_bins:" << nbins << endl;
	cout << "Decreasing ratio (priority):" << pdec << endl;

    mclpState* s0 = new_mstate(file,inst, min_fr, 10000, _rotate, nboxes);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();


    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
        alpha, beta, gamma, p, delta, 1.0, 0.0);

	cout << "greedy" << endl;
	Greedy *gr = new Greedy (vcs);

	/*
	cout << "bsg" << endl;
	BSG *bsg= new BSG(vcs,*gr, 12, 0.0, 0, _plot);
	bsg->trace=false;
	*/

   	int bins=solve(gr, NULL, s0, nbins, pdec);

	//if(_plot){
	//   pointsToTxt(&s_copy, 0);
	//   system("firefox problems/clp/tree_plot/index.html");
	//}

}
