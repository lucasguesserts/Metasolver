/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <fstream>
#include <unistd.h>

#include <dirent.h>
#include <malloc.h>
#include <iterator>

#include "args.hxx"
//#include "objects/State.cpp"
#include "mclp-state.h"
#include "BlockSet.h"
#include "BoxShape.h"
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

string findInto(string current_directory, string find_directory){
	DIR* dir;
	struct dirent *ent;
	dir = opendir(current_directory.c_str());
	if(dir != NULL){
		while((ent = readdir(dir)) != NULL){
			if(((string)ent->d_name).compare(find_directory) == 0){
				//break;
				closedir(dir);
				//cout << current_directory + find_directory << endl;
				return current_directory + find_directory;
			}
			if(((string)ent->d_name).find(".") == string::npos){
				findInto(current_directory + "/" + ent->d_name, find_directory);
				//cout << ent->d_name << endl;
				//cout << current_directory << endl;
				//cout << endl;
			}
		}
	}
	closedir(dir);
	//cout << current_directory + find_directory << endl;
	return "";
}

string findOut(string current_directory, string find_directory){
	DIR* dir;
	struct dirent *ent;
	current_directory += "/../";
	dir = opendir(current_directory.c_str());
	//cout << "Dentro de findOut "<< current_directory + find_directory << endl;
	for(int i = 0; i < 5; i++){
		while((ent = readdir(dir)) != NULL){
			if(((string)ent->d_name).compare(find_directory) == 0){
				closedir(dir);
				//cout << current_directory + find_directory << endl;

				return current_directory + find_directory +"/";
			}
		}
		current_directory += "../";
		dir = opendir((current_directory).c_str());
	}
	closedir(dir);
	return "";
}

string findDirectory(string current_directory, string find_directory){
	string find = findInto(current_directory, find_directory);
	//cout << "Dentro de findDirectory-finInto " << find << endl;
	if(!find.empty()) return find;
	else {
		find = findOut(current_directory, find_directory);

		if(!find.empty()) return find;
	}
	return "";
}

/*
//Obtiene la ubicacion actual desde la raiz
char cwd[2048];
if (getcwd(cwd, sizeof(cwd)) != NULL) {
	printf("Current working dir: %s\n", cwd);
} else {
	perror("getcwd() error");
}*/

void exportToTxtSCP(list < set<int> >* bins, long int nb_boxes){

	string path = findDirectory(".", "GRASP-SCP");
	string path2 = findDirectory(".", "gurobi");
	string filename = "bins_scp" + to_string(getpid()) + ".txt";

  path = "";

	/*if(path.empty()){
		cout << "El directorio no existe.\n" << endl;
		exit(0);
	}*/
	cout << "Resultados almacenados en " << path << filename << endl;
	ofstream scp (path + filename);

	if (scp.is_open()){
		long int total_boxes = 0;

		//Bins quantity
		scp << " " << bins->size() << " ";

		//Boxes quantity
		scp << nb_boxes << "\n";

		//Matrix cost by boxes
		/*long int cont = 0;
		for(auto bin: *bins){
			if(cont >= 12){
				scp << "\n";
				cont = 0;
			}
			scp << " 1";
			cont += 1;
		}
		scp << "\n";*/

		//Boxes quantity in a set and then sets boxes
		for(auto bin: *bins){
			scp << " " << bin.size() << "\n";
			for(auto box: bin){
				scp << " " << box + 1;
			}
			scp << "\n";
		}

		scp.close();
	} else cout << "Unable to open file" << endl;
}

void solve_set_covering(string gurobi_path, list < set<int> >& bins , int nb_box_types){
	exportToTxtSCP(&bins, nb_box_types);
	string filename = "bins_scp" + to_string(getpid()) + ".txt";

	const string MAX_TIME = "10";
	const string SEED = "1";
	string run2 = string("python " + gurobi_path + "/Solver.py " + filename);
	cout << "python " << gurobi_path << "/Solver.py " << filename << endl;
	FILE *p = popen(run2.c_str(), "r");
	list<int> first_bins;


	if(p != NULL) {
		cout << endl;
		//cout << run2 << endl;

		cout << "running Gurobi-Solver" << endl;
		cout << "Time: " << MAX_TIME << endl;
		cout << "Seed: " << SEED << endl;

		char output[150], last_output[150],caracteres[200], containers[200];
		string str;
		vector <string> line;
		string check_container = "";

		/*while(fgets(output, sizeof(output), p) != NULL) {
			strcpy(last_output, output);
		}*/

		while (feof(p) == 0)
 	  {
 			fgets(caracteres,200,p);
			if(caracteres[0] == '[') strcpy(containers, caracteres);
			else strcpy(last_output, caracteres);
		}

	  last_output[strlen(last_output)-1]=0;
		containers[strlen(containers)-1]=0;
		//cout << last_output << " " << endl;
		cout <<containers << endl;
		str = output;

		//Se recorre la lista entregada por gurobi
		for(int i = 1; containers[i] != ']' ;i++){
			if(containers[i] == ',' ){
				const char *pointer = check_container.c_str();
				int buffer = std::atoi(pointer);
				first_bins.push_back(buffer);
				check_container = "";
			}else{
				check_container += containers[i];
			}
		}
		const char *pointer = check_container.c_str();
		int buffer = std::atoi(pointer);
		first_bins.push_back(buffer);

		string delimiter = " ";
		size_t pos = 0;
		string token;
		while ((pos = str.find(delimiter)) != string::npos) {
			token = str.substr(0, pos);
			str.erase(0, pos + delimiter.length());
			line.push_back(token);
		}
		//cout << line.size() << endl;
		for(int i = 0; i < line.size(); i++) cout << line[i] << " " << str;
	} else {
		perror("Unable to open file");
	}

	pclose(p);
	list < set<int> > bins_gurobi;
	//string check_container = "";
	for(auto id_bin : first_bins){
		int index_bins = 0;
		for(auto boxes : bins){
			if(id_bin == index_bins) bins_gurobi.push_back(boxes);
			index_bins++;
		}
	}
	//recorrer lista de bins y copiar los correspondientes a la solucion entregada por gurobi en bins_gurobi
	bins=bins_gurobi;
}

double random(double l, double r) {
	return l + ((double)rand()/RAND_MAX)*(r-l);
}

pair<double,double> computeExclusiveVolumen(list<set<int>>& total_bins, int id_bin){
	int i = 0;
	int j = 0;
	double vol = 0;
	double max_vol = 0;
	set<int> id_boxes;
	for(auto bin: total_bins){
		if(i == id_bin) id_boxes = bin;
		i++;
	}
	for(auto box: id_boxes)
	{
		bool exclusive = true;
		double volu_box = 0;
		int j = 0;
		for(auto bin: total_bins)
		{
			if(j != id_bin)
			{
				if(bin.count(box) > 0) exclusive = false;
			}
			j++;
		}
		if(exclusive ){
			double volu_box = mclpState::id2box[box]->getVolume();
			if(volu_box > max_vol) max_vol = volu_box;
			vol += volu_box;
		}
	}
	return make_pair(vol,max_vol);
}

pair<double,list< set<int> >::iterator > minExclusiveVolume(list<set<int>>& bins){
	//list<set<int>> new_list_bin;
	double minvolume =  std::numeric_limits<double>::max();
	int id_bin = 0;
	int id_actual_min = 0;
	list< set<int> >::iterator min_bin;
	for(auto bin=bins.begin(); bin!= bins.end(); bin++, id_bin++ ){
		double exc_vol = computeExclusiveVolumen(bins, id_bin).first;

		if(exc_vol < minvolume){
			minvolume = exc_vol;
			min_bin = bin;
			id_actual_min = id_bin;
		}
	}
	/*id_bin = 0;
	for(auto bin: bins){
		if(id_bin != id_actual_min){
			new_list_bin.push_back(bin);
		}
	}*/

	pair<double,list< set<int> >::iterator  > minimum;
	minimum.first = minvolume;
	minimum.second = min_bin;
	return minimum;
}

pair<double,list< set<int> >::iterator > minLargestExlusiveBox(list<set<int>>& bins){
	//list<set<int>> new_list_bin;
	double minvolume =  std::numeric_limits<double>::max();
	int id_bin = 0;
	int id_actual_min = 0;
	list< set<int> >::iterator min_bin;
	for(auto bin=bins.begin(); bin!= bins.end(); bin++, id_bin++ ){
		double exc_vol = computeExclusiveVolumen(bins, id_bin).second;

		if(exc_vol < minvolume){
			minvolume = exc_vol;
			min_bin = bin;
			id_actual_min = id_bin;
		}
	}
	/*id_bin = 0;
	for(auto bin: bins){
		if(id_bin != id_actual_min){
			new_list_bin.push_back(bin);
		}
	}*/

	pair<double,list< set<int> >::iterator > minimum;
	minimum.first = minvolume;
	minimum.second = min_bin;
	return minimum;
}


list < set<int> > generate_bins(SearchStrategy* clp_solver, mclpState* s0, set<int>& id_boxes, int nbins=1000){
	list < set<int> > bins;

	map <int, int> used_boxes;

	for(auto box: id_boxes)
		used_boxes[box] = 100;

	for(auto b : s0->nb_left_boxes){
		b.first->set_profit(b.first->getVolume()*pow(random(0.8, 1.0),used_boxes[b.first->get_id()]));
	}

	set<int> new_bin;

	for(int i=0; i < nbins || (s0->nb_left_boxes.size() > used_boxes.size()); i++){
		//copia el estado base
		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());

		//usa clp_solver para llenar contenedor
		double eval=clp_solver->run(s_copy);

		const mclpState* best_state=dynamic_cast<const mclpState*>(clp_solver->get_best_state());
		best_state->update_profits(&best_state->cont->nb_boxes, used_boxes);

		//se almacena el bin en el conjunto
		for(auto box: dynamic_cast<const mclpState*>(clp_solver->get_best_state())->cont->nb_boxes){
			new_bin.insert(box.first->get_id());
			if(used_boxes.find(box.first->get_id()) == used_boxes.end())
				used_boxes.insert(make_pair(box.first->get_id(), box.second));
			else
				used_boxes[box.first->get_id()]++;

			//cout << box.first->get_id() + 1 << ", ";
		}

		//se busca el nuevo bin en el conjunto de bins creados
		bool insert_bin = true;
		for(auto bin: bins){
			if(new_bin == bin){
				insert_bin = false;
				break;
			}
		}

		if(insert_bin && !new_bin.empty()){
			bins.push_back(new_bin);
			for(auto box: new_bin)
				cout << box << " ";
			cout << endl;
		}
		new_bin.clear();
	}

	return bins;
}


list < set<int> >  get_break_bins(list < set<int> > best_bins, set <int>& break_boxes){

	//para obtener una caja usando el id
	//BoxShape* boxt = mclpState::id2box[1];
	//cout << boxt->getVolume() << endl;

	list < set <int> > break_bins;
	int n = best_bins.size()/2;
	for(int i=0; i<n; i++){
		list < set<int> >::iterator it;
		if(i%2==0)
		   it = minExclusiveVolume(best_bins).second;
		else
		   it = minLargestExlusiveBox(best_bins).second;

		break_bins.push_back(*it);

		for (auto box: *it)
			break_boxes.insert(box);

		best_bins.erase(it);
	}

	for (set<int> bin: best_bins){
		for(int box: bin){
			if(break_boxes.find(box)!=break_boxes.end()){
				break_boxes.erase(box);
			}
		}
	}

	return break_bins;
}

list < set<int> >  get_break_bins_random(list < set<int> > best_bins, set <int>& break_boxes){

	//para obtener una caja usando el id
	//BoxShape* boxt = mclpState::id2box[1];
	//cout << boxt->getVolume() << endl;

	list < set <int> > break_bins;
	int n = best_bins.size()/2;
	for(int i=0; i<n; i++){
		int r=rand()%best_bins.size();
		list < set<int> >::iterator it = best_bins.begin();
		std::advance(it,r);
		break_bins.push_back(*it);

		for (auto box: *it)
			break_boxes.insert(box);

		best_bins.erase(it);
	}

	for (set<int> bin: best_bins){
		for(int box: bin){
			if(break_boxes.find(box)!=break_boxes.end()){
				break_boxes.erase(box);
			}
		}
	}

	return break_bins;
}

/*Clonar estado inicial
Aplicar Greedy y obtener contenedor
Verificar si el contenedor ya existe, si no agregarlo a lista de contenedores (bins)
Reducir peso de las cajas utilizadas en el contenedor
Volver a 1*/

int solve(Greedy* gr, BSG *bsg, mclpState* s0, int nbins, double pdec, string gurobi_path, int solver_iter){
	SearchStrategy * clp_solver = gr;
	set<int> boxes;
	list < set<int> > best_bins;
	list < set<int> > break_bins;
	list < set<int> > bins;
	bool first=true;
	for(int i=0;i<solver_iter;i++){

		bins = generate_bins(clp_solver, s0, boxes);

		for(auto bin: bins){
			for(auto box: bin)
				cout << box << " ";
			cout << endl;
		}

		//resuelve set-covering y deja los bins de la solucion optima
		solve_set_covering(gurobi_path, bins,  s0->nb_left_boxes.size());

		cout << break_bins.size() << endl;
		cout << bins.size() << endl;
		if(best_bins.empty()) best_bins=bins;
		else if(bins.size() < break_bins.size()){
			//best_bins <- best_bins - break_bins + bins
			list < set<int> > v, v2;
			auto it=std::set_difference(best_bins.begin(), best_bins.end(), break_bins.begin(), break_bins.end(), v.begin());
			it=std::set_union (v.begin(), v.end(), bins.begin(), bins.end(), v2.begin());
			best_bins = v2;
		}

		boxes.clear();

		break_bins = get_break_bins(best_bins, boxes);

	}


	//end while


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
	args::ValueFlag<double> _solver_iter(parser, "int", "number of iterations by the solver", {"solver_iter"});
	args::ValueFlag<string> _gurobi_path(parser, "string", "path of gurobi", {"gurobi_path"});

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

	string gurobi_path = "extras/gurobi";
	string file=_file.Get();
	int inst=(_inst)? _inst.Get():0;
	double min_fr=(_min_fr)? _min_fr.Get():0.98;
	int maxtime=(_maxtime)? _maxtime.Get():100;
	int solver_iter = 1;

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
  if(_gurobi_path) gurobi_path = _gurobi_path.Get();
	if(_solver_iter)	solver_iter = _solver_iter.Get();
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


    clock_t begin_time=clock();

	mclpState* s0 = new_mstate(file,inst, min_fr, 10000, _rotate, nboxes);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;




    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
        alpha, beta, gamma, p, delta, 0.0, 0.0);

	cout << "greedy" << endl;
	Greedy *gr = new Greedy (vcs);


	cout << "bsg" << endl;
	BSG *bsg= new BSG(vcs,*gr, 4, 0.0, 0, _plot);
	bsg->trace=false;


  int bins=solve(gr, bsg, s0, nbins, pdec, gurobi_path, solver_iter);

  std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

	//if(_plot){
	//   pointsToTxt(&s_copy, 0);
	//   system("firefox problems/clp/tree_plot/index.html");
	//}

}
