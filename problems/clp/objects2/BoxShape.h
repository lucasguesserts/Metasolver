/*
 * Box.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#include "Volume.h"

#ifndef OBJECTS_BOXSHAPE_H_
#define OBJECTS_BOXSHAPE_H_

namespace clp {

class BoxShape : public Volume {
public:
	enum Orientation{LWH=0, LHW, WLH, WHL, HLW, HWL, OR};


	BoxShape(int id, long l, long w, long h, bool rot1=true, bool rot2=true, bool rot3=true, double weight=1.0);

	bool is_valid(Orientation o) const {return is_valid_orientation[o];}

	long getL(Orientation o) const;
	long getW(Orientation o) const;
	long getH(Orientation o) const;

	/**
	 * returns the i-th dimension of the box oriented by o
	 */
	long get(int i, Orientation o) const;



    bool operator<(const BoxShape& bt) const{
    	return (id<bt.id);
    }

    inline int get_id() const {return id;}

    double get_weight() const { return weight; }


    double get_priority() const { return priority; }

    void set_volume(double volume) const { this->volume = volume; }

    void set_priority(double priority) const { this->priority = priority; }
    //void set_weight(const double w) const { weight = w; }


private:
	int id;
	bool is_valid_orientation[6];

	double weight;

	mutable double priority;


};

} /* namespace clp */

#endif /* OBJECTS_BOXSHAPE_H_ */
