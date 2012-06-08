
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
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

#include "census/dim2gluingperms.h"
#include "census/ngenericgluingperms.tcc"
#include "dim2/dim2triangulation.h"

namespace regina {

// Instantiate all templates from the .tcc file.
template NGenericGluingPerms<2>::NGenericGluingPerms(
        const NGenericGluingPerms<2>&);
template NGenericGluingPerms<2>::NGenericGluingPerms(std::istream&);
template Dim2Triangulation* NGenericGluingPerms<2>::triangulate() const;
template int NGenericGluingPerms<2>::gluingToIndex(
        const Dim2TriangleEdge&, const NPerm3&) const;
template int NGenericGluingPerms<2>::gluingToIndex(
        unsigned, unsigned, const NPerm3&) const;
template void NGenericGluingPerms<2>::dumpData(std::ostream&) const;

} // namespace regina

