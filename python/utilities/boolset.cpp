
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
 *                                                                        *
 *  Copyright (c) 1999-2021, Ben Burton                                   *
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

#include "../pybind11/pybind11.h"
#include "../pybind11/operators.h"
#include "utilities/boolset.h"
#include "../helpers.h"

using regina::BoolSet;

void addBoolSet(pybind11::module_& m) {
    auto c = pybind11::class_<BoolSet>(m, "BoolSet")
        .def(pybind11::init<>())
        .def(pybind11::init<bool>())
        .def(pybind11::init<const BoolSet&>())
        .def(pybind11::init<bool, bool>())
        .def("hasTrue", &BoolSet::hasTrue)
        .def("hasFalse", &BoolSet::hasFalse)
        .def("contains", &BoolSet::contains)
        .def("insertTrue", &BoolSet::insertTrue)
        .def("insertFalse", &BoolSet::insertFalse)
        .def("removeTrue", &BoolSet::removeTrue)
        .def("removeFalse", &BoolSet::removeFalse)
        .def("empty", &BoolSet::empty)
        .def("fill", &BoolSet::fill)
        .def(pybind11::self < pybind11::self)
        .def(pybind11::self > pybind11::self)
        .def(pybind11::self <= pybind11::self)
        .def(pybind11::self >= pybind11::self)
        .def(pybind11::self |= pybind11::self)
        .def(pybind11::self &= pybind11::self)
        .def(pybind11::self ^= pybind11::self)
        .def(pybind11::self | pybind11::self)
        .def(pybind11::self & pybind11::self)
        .def(pybind11::self ^ pybind11::self)
        .def(~ pybind11::self)
        .def("byteCode", &BoolSet::byteCode)
        .def("setByteCode", &BoolSet::setByteCode)
        .def_static("fromByteCode", &BoolSet::fromByteCode)
        .def("stringCode", &BoolSet::stringCode)
        .def("setStringCode", &BoolSet::setStringCode)
        // We don't use the C++ sNone, sTrue, sFalse, sBoth constants here
        // because they are deprecated.
        .def_property_readonly_static("sNone", [](pybind11::object) {
            return BoolSet();
        })
        .def_property_readonly_static("sTrue", [](pybind11::object) {
            return BoolSet(true);
        })
        .def_property_readonly_static("sFalse", [](pybind11::object) {
            return BoolSet(false);
        })
        .def_property_readonly_static("sBoth", [](pybind11::object) {
            return BoolSet(true, true);
        })
    ;
    regina::python::add_output_ostream(c);
    regina::python::add_eq_operators(c);

    pybind11::implicitly_convertible<bool, BoolSet>();
}

