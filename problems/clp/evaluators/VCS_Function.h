/*
 * VCS.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef VCS_FUNCTION_H_
#define VCS_FUNCTION_H_

#include <random>
#include "VLossFunction.h"
#include "clpState.h"

using namespace std;

namespace clp {

class VCS_Function : public VLossFunction{
public:
	VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0, double theta=0.0, double r=0.0, double max_theta=1.5, double alphafactor =1.0, double betafactor = 1.0, double gammafactor = 1.0, double deltafactor = 1.0, double pfactor = 1.0, int metodo = 1);

	virtual ~VCS_Function();

	virtual double eval_action(const State& , const Action& a );


	long cs[6];

	static int nn;

	double CS_p(const State& s, const Block& b, const Space& sp);


	virtual void set_alpha(double a){ alpha = a; }

	virtual void set_gamma(double g){ gamma = g; }

	virtual void set_p(double pp){ p = pp; }

	virtual double get_alpha(){ return alpha;}

	virtual double get_betaV() { return get_beta();}

	virtual double get_gamma() { return gamma;}

	virtual double get_deltaV() { return get_delta();}

	virtual double get_p() { return p;}

	virtual void update_level_alpha(double mean, double stdev){
		//cout << "UFF" << endl;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<double> distribution(mean,stdev);
		double sample = distribution(gen);
		if(sample < 2.0) sample = 2.0;
		else if (sample > 8.0) sample = 8.0;
		alpha=sample;
	}

	virtual void update_parameters(const State& s, double mean, double stdev){

		 	std::random_device rd;
		 	std::mt19937 gen(rd());
			std::normal_distribution<double> distribution(mean,stdev);
			double sample = distribution(gen);
			int size = s.get_path().size(); //largo del camino
			const clpState* state=dynamic_cast<const clpState*>(&s);
			int boxn = state->cont->n_boxes; //numero cajas
			int blockn = state->valid_blocks.size();
			double fill = state->get_value();//porcentaje de llenado
			//state->cont->getH();//Altura del contenedor
			//const Space& sp= *dynamic_cast<SpaceSet*>(state->cont->spaces)->data.begin();


			//cout << fill << endl;
			//cout << state->cont->n_boxes << "//" << size << endl;
			//cout << blockn << "//" << size << endl;

			//La distribucion normal esta basada en el parametro alpha
			if(metodo == 1){
				if(sample < 3.0) sample = 3.0;
				else if (sample > 5.0) sample = 5.0;
				alpha = sample;
			}

			//La distribucion normal esta basada en el parametro beta
			if(metodo == 2){
				if(sample < 0.5) sample = 0.5;
				else if (sample > 1.5) sample = 1.5;
				set_beta(sample);
			}

			//La distribucion normal esta basada en el parametro gamma
			if(metodo == 3){
				if(sample < 0.05) sample = 0.05;
				else if (sample > 0.4) sample = 0.4;
				gamma = sample;
			}

			//La distribucion normal esta basada en el parametro delta
			if(metodo == 4){
				if(sample < 0.5) sample = 0.5;
				else if (sample > 1.5) sample = 1.5;
				set_delta(sample);
			}

			//La distribucion normal esta basada en el parametro p
			if(metodo == 5){
				if(sample < 0.01) sample = 0.01;
				else if (sample > 0.08) sample = 0.08;
				p = sample;
			}

			/*
			if(metodo == 5){   //exponencial desde el primer movimiento. tiende a 0 e infinito.
				alpha = alpha0 * pow(alphafactor,size);
				set_beta(beta0);
				set_delta(delta0 * pow(deltafactor,size));
				p = p0 * pow(pfactor,size);
				gamma = gamma0 * pow(gammafactor,size);
			}

			if(metodo == 3){ //segregacion por cantidad de cajas en el contenedor
				if(boxn < 45){ //necesitamos algo que cacule la cant de cajas
					alpha=alpha0;
					set_beta(beta0);
					set_delta(delta0);
					p=p0;
					gamma = gamma0 * pow(gammafactor,size);

				}
				else{
					alpha = alpha0 * pow(alphafactor,size);
					set_beta(beta0);
					set_delta(delta0 * pow(deltafactor,size));
					p = p0 * pow(pfactor,size);
					gamma = gamma0 * pow(gammafactor,size);
				}
			}
*/
			/**
			* Segregacion por size
			*/
			/*
			if(metodo == 1){
				if(size<9)
				{
					alpha=alpha0;
					set_beta(beta0);
					gamma= gamma0;
					set_delta(delta0);
					p=p0;
				}else
				{
					if(size>=9)
					{

						alpha = alpha0 * pow(alphafactor,size-8);
						set_beta(beta0 * pow(betafactor,size-8));
						gamma = gamma0 * pow(gammafactor,size-8);
						set_delta(delta0 * pow(deltafactor,size-8));
						p = p0 * pow(pfactor,size-8);
					}
				}
			}*/
/*
			if(metodo == 4){ //segregacion por size en reversa
				if(size<9)
				{
					alpha = alpha0 * pow(alphafactor,size);
					set_beta(beta0 * pow(betafactor,size));
					gamma = gamma0 * pow(gammafactor,size);
					set_delta(delta0 * pow(deltafactor,size));
					p = p0 * pow(pfactor,size);
				}else
				{
					if(size>=9)
					{
						alpha=alpha0;
						set_beta(beta0);
						gamma= gamma0;
						set_delta(delta0);
						p=p0;
					}
				}
			}
*/
			/*
			if(metodo == 6){ //segregacion por size exponencial al principio y luego constante
							if(size<9)
							{
								alpha = alpha0 * pow(alphafactor,size);
								set_beta(beta0 * pow(betafactor,size));
								gamma = gamma0 * pow(gammafactor,size);
								set_delta(delta0 * pow(deltafactor,size));
								p = p0 * pow(pfactor,size);
							}

			}
	*/
			/**
			* segregacion por % de llenado
			*/
			/*
			if(metodo == 2){
				if(fill < 0.85){
					alpha = alpha0;
					set_beta(beta0);
					gamma= gamma0;
					set_delta(delta0);
					p = p0;
				}else
				{
					if(fill >=0.85)
					{
						alpha = alpha0 * pow(alphafactor,size);
						set_beta(beta0 * pow(betafactor,size));
						gamma = gamma0 * pow(gammafactor,size);
						set_delta(delta0 * pow(deltafactor,size));
						p = p0 * pow(pfactor,size);
					}
				}
			}
			*/
			//cout << alpha << endl;
		}



protected:

	long _surface_in_contact(const AABB& b, const AABB& bi);
	long _surface_in_contact(const AABB& bi, const Block& c);


  int metodo;
	//parameters
  double alpha, gamma, p; //
  double alpha0;
  double gamma0;
  double p0;
  double beta0;
  double delta0;
  double alphafactor, betafactor, gammafactor, deltafactor, pfactor;


};

} /* namespace clp */

#endif /* HVCS_FUNCTION_H_ */
