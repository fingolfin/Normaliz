/*
 * Normaliz 2.8
 * Copyright (C) 2007-2012  Winfried Bruns, Bogdan Ichim, Christof Soeger
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

#ifndef NORMALIZ_EXEPTION_H_
#define NORMALIZ_EXEPTION_H_

#include <exception>
//#include "libnormaliz.h"

namespace libnormaliz {

class NormalizException: public std::exception {
	virtual const char* what() const throw() = 0;
};

class ArithmeticException: public NormalizException {
	virtual const char* what() const throw() {
		return "Arithmetic Overflow detected, try a bigger integer type!";
	}
};

class BadInputException: public NormalizException {
	virtual const char* what() const throw() {
		return "Some error in the normaliz input data detected!";
	}
};

class FatalException: public NormalizException {
	virtual const char* what() const throw() {
		return "Fatal error! This should not happen, please contact the developers.";
	}
};


} /* end namespace */

#endif /* LIBNORMALIZ_H_ */
