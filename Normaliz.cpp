/*
 * Normaliz 2.5
 * Copyright (C) 2007-2010  Winfried Bruns, Bogdan Ichim, Christof Soeger
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

#include <stdlib.h>
#include <vector>
#include <list>

#include <sstream>
#include <algorithm>
using namespace std;

#include "Normaliz.h"

#include "output.cpp"

void printHelp(char* command) {
	cout << "usage: "<<command<<" [-acdefhimnpsv?] [-x=<T>] [PROJECT]"<<endl;
	cout << "  runs normaliz on PROJECT.in"<<endl;
	cout << "options:"<<endl;
	cout << "  -?\tprint this help text and exit"<<endl;
	cout << "  -s\tcomputation mode: support_hyperplanes"<<endl;
	cout << "  -S\tcomputation mode: support_hyperplanesvia pyramids"<<endl;
	cout << "  -v\tcomputation mode: triangulation"<<endl;
	cout << "  -V\tcomputation mode: triangulation via pyramids"<<endl;
	cout << "  -n\tcomputation mode: triangulation_hilbert_basis (old type normal)"<<endl;
	cout << "  -N\tcomputation mode: hilbert_basis (using a partial triangulation)"<<endl;
	cout << "  -1\tcomputation mode: ht1_elements"<<endl;
	cout << "  -p\tcomputation mode: hilbert_polynomial"<<endl;
	cout << "  -h\tcomputation mode: hilbert_basis_polynomial"<<endl;
	cout << "  -d\tcomputation mode: dual"<<endl;
	cout << "  -f\tthe files .out .gen .inv .typ .cst are written"<<endl;
	cout << "  -a\tall output files are written"<<endl;
	cout << "  -e\tperform tests for arithmetic errors"<<endl;
	cout << "  -B\tuse indefinite precision arithmetic"<<endl;
	cout << "  -c\tverbose (prints control data)"<<endl;
	cout << "  -m\tsave memory (currently has no effect)"<<endl;
	cout << "  -i\tobsolete option"<<endl;
	cout << "  -x=<T>\tlimit the number of threads to <T>"<<endl;
}

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int i;       //used for iterations
	char c;
	string computation_type="triangulation_hilbert_basis";
	//4  types available, "support_hyperplanes", "triangulation", "normal" and "hilbert_polynomial"
	//it is set by the setup file or by the options "s", "v", "n", "p", "h" and "d" in the command line
	//the type given in the command line overrides the type set by the setup file
	string output_name;         //name of the output file(s) saved here




	// read command line options
	bool filename_set=false;
	string option;            //all options concatenated (including -)
	for (i = 1; i <argc; i++) {
		if (argv[i][0]=='-') {
			if (argv[i][1]!='\0') {
				if (argv[i][1]!='x') {
					option = option + argv[i];
				} else if (argv[i][2]=='=') {
					#ifndef NO_OPENMP
					string Threads = argv[i];
					Threads.erase(0,3);
					int nr_threads;
					if ( (istringstream(Threads) >> nr_threads) && nr_threads > 0) {
						omp_set_num_threads(nr_threads);
					} else {
						cerr<<"Warning: Invalid option string "<<argv[i]<<endl;
					}
					#else
					cerr << "Warning: Compiled without OpenMP support, option "<<argv[i]<<" ignored."<<endl;
					#endif
				} else {
					cerr<<"Warning: Invalid option string "<<argv[i]<<endl;
				}
			}
		} else if (!filename_set) {
			string s(argv[i]);
			output_name=s;
			filename_set=true;
		}
	}



	//Analyzing the command line options
	bool write_extra_files = false, write_all_files = false;
	bool use_Big_Integer = false;

	for (i = 1; i <option.size(); i++) {
		switch (option[i]) {
			case '-':
			case 'i':
				break;
			case 'c':
				verbose=true;
				break;
			case 'f':
				write_extra_files = true;
				break;
			case 'a':
				write_all_files = true;
				break;
			case 's':
				computation_type="support_hyperplanes";
				break;
			case 'S':
				computation_type="support_hyperplanes_pyramid";
				break;
			case 'v':
				computation_type="triangulation";
				break;
			case 'V':
				computation_type="triangulation_pyramid";
				break;
			case 'n':
				computation_type="triangulation_hilbert_basis";
				break;
			case 'N':
				computation_type="hilbert_basis";
				break;
			case '1':
				computation_type="ht1_elements";
				break;
			case 'p':
				computation_type="hilbert_polynomial";
				break;
			case 'h':
				computation_type="hilbert_basis_polynomial";
				break;
			case 'd':
				computation_type="dual";
				break;
			case 'e':  //check for arithmetic overflow
				test_arithmetic_overflow=true;
				break;
			case 'B':  //use Big Interger
				use_Big_Integer=true;
				break;
			case 'm':  //save memory / don't optimize for speed
			//	optimize_speed=false;
				break;
			case '?':  //print help text and exit
				printHelp(argv[0]);
				exit(1);
				break;
			case 'x': //should be separated from other options
				cerr<<"Warning: Option -x=<T> has to be separated from other options"<<endl;
				break;
			default:
				cerr<<"Warning: Unknown option -"<<option[i]<<endl;
				break;
		}
	}

	if (!filename_set) {
		cout<<"Normaliz 2.5"<<endl
			<<"Copyright (C) 2007-2010  Winfried Bruns, Bogdan Ichim, Christof Soeger"<<endl
			<<"This program comes with ABSOLUTELY NO WARRANTY; This is free software,"<<endl
			<<"and you are welcome to redistribute it under certain conditions;"<<endl
			<<"See COPYING for details."
			<<endl<<endl;
		cout<<"Enter the input file name or -? for help: ";
		cin >>output_name;
		if (output_name == "-?") {
			printHelp(argv[0]);
			return 1;
		}
	}

	int returnvalue;

	if(use_Big_Integer) {
		//if the program works with the indefinite precision arithmetic, no arithmetic tests are performed
		test_arithmetic_overflow=false;
		//Read and process Input
		returnvalue = process_data<mpz_class>(output_name, computation_type, write_extra_files, write_all_files);
	} else {
		//Read and process Input
		returnvalue = process_data<long long>(output_name, computation_type, write_extra_files, write_all_files);
	}

	//exit
	if (!filename_set) {
		cout<< "\nType something and press enter to exit.\n";
		cin >> c;
	}
	return returnvalue;
}

//---------------------------------------------------------------------------

template<typename Integer> int process_data(string& output_name, string& computation_type, bool write_extra_files, bool write_all_files ) {
	int i,j;

	Output<Integer> Out;    //all the information relevant for output is collected in this object

	if(write_all_files) {
		Out.set_write_all_files();
	} else if (write_extra_files) {
		Out.set_write_extra_files();
	}



	string name_in=output_name+".in";
	const char* file_in=name_in.c_str();
	ifstream in, in2;
	in2.open(file_in,ifstream::in);
	if (in2.is_open()==false) {
		//check if user added ".in" and ignore it in this case
		string suffix (".in");
		size_t found = output_name.rfind(suffix);
		if (found!=string::npos) {
			output_name.erase(found);
			name_in=output_name+".in";
			file_in=name_in.c_str();
		}
	} else {
		in2.close();
	}
	in.open(file_in,ifstream::in);
	if (in.is_open()==false) {
		cerr<<"error: Failed to open file "<<name_in<<"."<<endl;
		return 1;
	}




	string mode_string;
	int nr_rows,nr_columns, mode;
	Integer number;
	in >> nr_rows;
	in >> nr_columns;
	Matrix<Integer> M(nr_rows,nr_columns);
	for(i=1; i<=nr_rows; i++){
		for(j=1; j<=nr_columns; j++) {
			in >> number;
			M.write(i,j,number);
		}
	}

	in>>mode_string;
	if (mode_string=="0"||mode_string=="integral_closure") {
		mode=0;
	} else
	if (mode_string=="1"||mode_string=="normalization") {
		mode=1;
	} else
	if (mode_string=="2"||mode_string=="polytope") {
		mode=2;
	} else
	if (mode_string=="3"||mode_string=="rees_algebra") {
		mode=3;
	} else
	if (mode_string=="4"||mode_string=="hyperplanes") {
		mode=4;
	} else
	if (mode_string=="5"||mode_string=="equations") {
		mode=5;
	} else
	if (mode_string=="6"||mode_string=="congruences") {
		mode=6;
	} else
	if (mode_string=="10"||mode_string=="lattice_ideal") {
		mode=10;
	} else {
		cerr<<"Warning: Unknown mode "<<mode_string<<" and will be replaced with mode integral_closure."<<endl;
		mode=0;
	}

	if ( in.fail() ) {
		cerr << "error: Failed to read file "<<name_in<<". May be a bad format of the input file."<<endl;
		return 1;
	}

	Out.set_name(output_name);
	
	if (mode >= 4 && mode <= 6) { //equations, inequalities, congruences
		int nc = nr_columns;
		if (mode == 6) {
			nc--;  //the congruence matrix has one extra column
		}
		Matrix<Integer> Inequalities(0,nc), Equations(0,nc), Congruences(0,nc+1);
		switch(mode) {
			case 4: Inequalities = M;
					break;
			case 5: Equations = M;
					break;
			case 6: Congruences = M;
					break;
			default: cerr<<"Reached unreachable code in Normaliz.cpp. Please contact the developers"<<endl;
					 return 1;
		}
		while (in.good()) {
			in >> nr_rows;
			in >> nr_columns;
			if (in.eof())
				break;  //not enough data to read on
			M = Matrix<Integer>(nr_rows,nr_columns);
			for(i=1; i<=nr_rows; i++){
				for(j=1; j<=nr_columns; j++) {
					in >> number;
					M.write(i,j,number);
				}
			}

			in>>mode_string;
			if ( in.fail() ) {
				cerr << "error: Failed to read file "<<name_in<<". May be a bad format of the input file?"<<endl;
				return 1;
			}

			if	 (mode_string=="4"||mode_string=="hyperplanes") {
				mode=4;
				if(nr_columns!=nc) {
					cerr<<"Number of columns not matching. Check input file!";
					return 1;
				}
			} else
			if (mode_string=="5"||mode_string=="equations") {
				mode=5;
				if(nr_columns!=nc) {
					cerr<<"Number of columns not matching. Check input file!";
					return 1;
				}
			} else
			if (mode_string=="6"||mode_string=="congruences") {
				mode=6;
				if(nr_columns!=nc+1) {
					cerr<<"Number of columns not matching. Check input file!";
					return 1;
				}
			} else {
				cerr<<"Illegal input type \""<<mode_string<<"\" at this position."<<endl;
				return 1;
			}
			
			switch(mode) {
				case 4: Inequalities.append(M);
						break;
				case 5: Equations.append(M);
						break;
				case 6: Congruences.append(M);
						break;
				default: cerr<<"Reached unreachable code in Normaliz.cpp. Please contact the developers"<<endl;
						return 10;
			}
		
		}
		
		in.close();
		if (verbose) {
			cout<<"\n************************************************************\n";
			cout<<"Running in computation mode "<<computation_type<<" with input type "<<456<<"."<<endl;
		}
		//run_mode_456(computation_type, Congruences, Equations, Inequalities, Out);
		Cone<Integer> MyCone = Cone<Integer>(Inequalities.to_list(), Equations.to_list(), Congruences.to_list());
		MyCone.compute(computation_type);
		Out.setCone(MyCone);
		Out.cone();
	} 
	else { // all other modes
		in.close();
		//main computations and output
		if (verbose) {
			cout<<"\n************************************************************\n";
			cout<<"Running in computation mode "<<computation_type<<" with input type "<<mode<<"."<<endl;
		}
		//make_main_computation(mode, computation_type, M, Out);
		Cone<Integer> MyCone = Cone<Integer>(M.to_list(), mode);
		MyCone.compute(computation_type);
		Out.setCone(MyCone);
		if (mode == 2) {
			Out.polytop();
		} else if (mode == 3) {
			Out.rees(true); //TODO primary übergeben
		} else {
			Out.cone();
		}
	}
	return 0;
}
