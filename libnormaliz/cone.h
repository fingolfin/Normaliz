/*
 * Normaliz 2.7
 * Copyright (C) 2007-2011  Winfried Bruns, Bogdan Ichim, Christof Soeger
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CONE_H_
#define CONE_H_

#include <vector>
#include <map>
#include "libnormaliz.h"
#include "cone_property.h"
#include "full_cone.h"
#include "sublattice_representation.h"

namespace libnormaliz {
using std::vector;
using std::map;


template<typename Integer>
class Cone {

//---------------------------------------------------------------------------
//                               public methods
//---------------------------------------------------------------------------
public:

//---------------------------------------------------------------------------
//                    Constructors, they preprocess the input 
//---------------------------------------------------------------------------

	/* give a single matrix as input */
	Cone(const vector< vector<Integer> >& input_data,
	     InputType type = Type::integral_closure);
	/* give multiple */ //TODO
	Cone(map< InputType , vector< vector<Integer> > >& multi_input_data);

//---------------------------------------------------------------------------
//                          give additional data
//---------------------------------------------------------------------------

	/* Sets the linear form which is used to grade. 
	 * It has to be an N-grading, i.e. all generators must have a value >=1.
	 * If it is not, a NormalizException will be thrown at the time of 
	 * detection which can be in this method or later!
	 * It will delete all data from the cone that depend on the grading!
	 */
	void setLinearForm (vector<Integer> lf);


//---------------------------------------------------------------------------
//                           make computations
//---------------------------------------------------------------------------

	void compute(ComputationMode mode = Mode::hilbertBasisPolynomial); //default: everything
	void compute(ConeProperties ToCompute);
	void compute(ConeProperty::Enum prop);

//---------------------------------------------------------------------------
//                         check what is computed
//---------------------------------------------------------------------------

	bool isComputed(ConeProperty::Enum prop) const;
	//returns true, when ALL properties in CheckComputed are computed
	bool isComputed(ConeProperties CheckComputed) const;

//---------------------------------------------------------------------------
//          get the results, these methods do not start a computation
//---------------------------------------------------------------------------

	vector< vector<Integer> > getGenerators() const;
	vector< vector<Integer> > getExtremeRays() const;
	vector< vector<Integer> > getSupportHyperplanes() const;
	vector< vector<Integer> > getEquations() const;
	vector< vector<Integer> > getCongruences() const;
	map< InputType , vector< vector<Integer> > > getConstraints() const;
	vector< pair<vector<size_t>, Integer> > getTriangulation() const;
	vector< vector<Integer> > getHilbertBasis() const;
	vector< vector<Integer> > getHt1Elements() const;
	HilbertSeries getHilbertSeries() const; //general purpose object
	vector<long64> getHVector64() const;
	vector<Integer> getHVector() const;
	vector<mpz_class> getHilbertPolynomial();
	vector< vector<mpz_class> > getHilbertQuasiPolynomial();
	vector<Integer> getLinearForm() const;
	Integer getMultiplicity() const;
	bool isPointed() const;
	bool isHt1ExtremeRays() const;
	bool isHt1HilbertBasis() const;
	bool isIntegrallyClosed() const;
	bool isReesPrimary() const;
	Integer getReesPrimaryMultiplicity() const;
	vector< vector<Integer> > getGeneratorsOfToricRing() const;
	Sublattice_Representation<Integer> getBasisChange() const;
	
//---------------------------------------------------------------------------
//                          private part
//---------------------------------------------------------------------------

private:	
	size_t dim;

	Sublattice_Representation<Integer> BasisChange;  //always use compose_basis_change() !
	bool BC_set;
	ConeProperties is_Computed;
	vector< vector<Integer> > GeneratorsOfToricRing;
	vector< vector<Integer> > Generators;
	vector<bool> ExtremeRays;
	vector< vector<Integer> > SupportHyperplanes;
	vector< pair<vector<size_t>, Integer> > Triangulation;
	Integer multiplicity;
	vector< vector<Integer> > HilbertBasis;
	vector< vector<Integer> > Ht1Elements;
	HilbertSeries HSeries;
	vector< vector<Integer> > HilbertQuasiPolynomial;
	vector<Integer> LinearForm;
	bool pointed;
	bool ht1_extreme_rays;
	bool ht1_hilbert_basis;
	bool integrally_closed;
	bool rees_primary;
	Integer ReesPrimaryMultiplicity;

	void compose_basis_change(const Sublattice_Representation<Integer>& SR); // composes SR


	/* Progress input, depending on input_type */
	void prepare_input_type_0(const vector< vector<Integer> >& Input);
	void prepare_input_type_1(const vector< vector<Integer> >& Input);
	void prepare_input_type_2(const vector< vector<Integer> >& Input);
	void prepare_input_type_3(const vector< vector<Integer> >& Input);
	void prepare_input_type_10(const vector< vector<Integer> >& Binomials);
	void prepare_input_type_456(const Matrix<Integer>& Congruences, const Matrix<Integer>& Equations, const Matrix<Integer>& Inequalities);
	void prepare_input_type_45(const Matrix<Integer>& Equations, const Matrix<Integer>& Inequalities);

	/* only used by the constructors */
	void initialize();
	void single_matrix_input(const vector< vector<Integer> >& Input, InputType input_type);

	/* compute method for the dual_mode, used in compute(string) */
	void compute_dual();

	/* extract the data from Full_Cone, this may remove data from Full_Cone!*/
	void extract_data(Full_Cone<Integer>& FC);

};

}  //end namespace libnormaliz

#endif /* CONE_H_ */
