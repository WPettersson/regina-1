
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
 *                                                                        *
 *  Copyright (c) 1999-2014, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include <boost/python.hpp>
#include "dim2/dim2exampletriangulation.h"
#include "dim2/dim2triangulation.h"
#include "../utilities/equality.h"

using namespace boost::python;
using regina::Dim2ExampleTriangulation;
using regina::Dim2Triangulation;

void addDim2ExampleTriangulation() {
    class_<Dim2ExampleTriangulation>("Dim2ExampleTriangulation", no_init)
        .def("orientable", &Dim2ExampleTriangulation::orientable,
            return_value_policy<manage_new_object>())
        .def("nonOrientable", &Dim2ExampleTriangulation::nonOrientable,
            return_value_policy<manage_new_object>())
        .def("sphere", &Dim2ExampleTriangulation::sphere,
            return_value_policy<manage_new_object>())
        .def("sphereTetrahedron", &Dim2ExampleTriangulation::sphereTetrahedron,
            return_value_policy<manage_new_object>())
        .def("sphereOctahedron", &Dim2ExampleTriangulation::sphereOctahedron,
            return_value_policy<manage_new_object>())
        .def("disc", &Dim2ExampleTriangulation::disc,
            return_value_policy<manage_new_object>())
        .def("annulus", &Dim2ExampleTriangulation::annulus,
            return_value_policy<manage_new_object>())
        .def("mobius", &Dim2ExampleTriangulation::mobius,
            return_value_policy<manage_new_object>())
        .def("torus", &Dim2ExampleTriangulation::torus,
            return_value_policy<manage_new_object>())
        .def("rp2", &Dim2ExampleTriangulation::rp2,
            return_value_policy<manage_new_object>())
        .def("kb", &Dim2ExampleTriangulation::kb,
            return_value_policy<manage_new_object>())
        EQUAL_BY_PTR(Dim2ExampleTriangulation)
        .staticmethod("orientable")
        .staticmethod("nonOrientable")
        .staticmethod("sphere")
        .staticmethod("sphereTetrahedron")
        .staticmethod("sphereOctahedron")
        .staticmethod("disc")
        .staticmethod("annulus")
        .staticmethod("mobius")
        .staticmethod("torus")
        .staticmethod("rp2")
        .staticmethod("kb")
    ;
}

