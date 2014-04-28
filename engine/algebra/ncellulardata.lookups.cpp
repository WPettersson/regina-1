/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
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

#include "algebra/ncellulardata.h"
#include <map>

namespace regina {

unsigned long NCellularData::nicIxLookup(const NVertex* vrt) const
{ return lower_bound( nicIx[0].begin(), nicIx[0].end(), 
  tri3->vertexIndex(vrt) ) - nicIx[0].begin(); }
unsigned long NCellularData::nicIxLookup(const NEdge* edg) const
{ return lower_bound( nicIx[1].begin(), nicIx[1].end(), 
  tri3->edgeIndex(edg) ) - nicIx[1].begin(); }
unsigned long NCellularData::nicIxLookup(const NTriangle* fac) const
{ return lower_bound( nicIx[2].begin(), nicIx[2].end(), 
  tri3->faceIndex(fac) ) - nicIx[2].begin(); }
unsigned long NCellularData::nicIxLookup(const NTetrahedron* tet) const
{ return lower_bound( nicIx[3].begin(), nicIx[3].end(), 
  tri3->tetrahedronIndex(tet) ) - nicIx[3].begin(); }

unsigned long NCellularData::nicIxLookup(const Dim4Vertex* vrt) const
{ return lower_bound( nicIx[0].begin(), nicIx[0].end(), 
  tri4->vertexIndex(vrt) ) - nicIx[0].begin(); }
unsigned long NCellularData::nicIxLookup(const Dim4Edge* edg) const
{ return lower_bound( nicIx[1].begin(), nicIx[1].end(), 
  tri4->edgeIndex(edg) ) - nicIx[1].begin(); }
unsigned long NCellularData::nicIxLookup(const Dim4Triangle* fac) const
{ return lower_bound( nicIx[2].begin(), nicIx[2].end(), 
  tri4->triangleIndex(fac) ) - nicIx[2].begin(); }
unsigned long NCellularData::nicIxLookup(const Dim4Tetrahedron* tet) const
{ return lower_bound( nicIx[3].begin(), nicIx[3].end(), 
  tri4->tetrahedronIndex(tet) ) - nicIx[3].begin(); }
unsigned long NCellularData::nicIxLookup(const Dim4Pentachoron* pen) const
{ return lower_bound( nicIx[4].begin(), nicIx[4].end(), 
  tri4->pentachoronIndex(pen) ) - nicIx[4].begin(); }

unsigned long NCellularData::icIxLookup(const NEdge* edg, unsigned long i) 
 const
{ return lower_bound( icIx[0].begin(), icIx[0].end(), 
  2*tri3->edgeIndex(edg)+i ) - icIx[0].begin(); }
unsigned long NCellularData::icIxLookup(const NTriangle* fac, unsigned long i)
 const
{ return lower_bound( icIx[1].begin(), icIx[1].end(), 
  3*tri3->faceIndex(fac)+i ) - icIx[1].begin(); }
unsigned long NCellularData::icIxLookup(const NTetrahedron* tet, 
 unsigned long i) const
{ return lower_bound( icIx[2].begin(), icIx[2].end(), 
  4*tri3->tetrahedronIndex(tet)+i ) - icIx[2].begin(); }

// ideal vertices
unsigned long NCellularData::icIxLookup(const Dim4Edge* edg, unsigned long i) 
 const
{ return lower_bound( icIx[0].begin(), icIx[0].end(), 
  2*tri4->edgeIndex(edg) + i) - icIx[0].begin(); }
// ideal edges
unsigned long NCellularData::icIxLookup(const Dim4Triangle* fac, 
 unsigned long i) const
{ return lower_bound( icIx[1].begin(), icIx[1].end(), 
  3*tri4->triangleIndex(fac) + i) - icIx[1].begin(); }
unsigned long NCellularData::icIxLookup(const Dim4Tetrahedron* tet, 
 unsigned long i) const
{ return lower_bound( icIx[2].begin(), icIx[2].end(), 
  4*tri4->tetrahedronIndex(tet) + i) - icIx[2].begin(); }
unsigned long NCellularData::icIxLookup(const Dim4Pentachoron* pen, 
 unsigned long i) const
{ return lower_bound( icIx[3].begin(), icIx[3].end(), 
  5*tri4->pentachoronIndex(pen) + i ) - icIx[3].begin(); }

unsigned long NCellularData::dcIxLookup(const NTetrahedron* tet) const 
{ return lower_bound( dcIx[0].begin(), dcIx[0].end(), 
 tri3->tetrahedronIndex(tet) ) - dcIx[0].begin(); }
unsigned long NCellularData::dcIxLookup(const NTriangle* fac) const 
{ return lower_bound( dcIx[1].begin(), dcIx[1].end(), 
 tri3->faceIndex(fac) ) - dcIx[1].begin(); }
unsigned long NCellularData::dcIxLookup(const NEdge* edg) const 
{ return lower_bound( dcIx[2].begin(), dcIx[2].end(), 
 tri3->edgeIndex(edg) ) - dcIx[2].begin(); }
unsigned long NCellularData::dcIxLookup(const NVertex* vrt) const 
{ return lower_bound( dcIx[3].begin(), dcIx[3].end(), 
 tri3->vertexIndex(vrt) ) - dcIx[3].begin(); }

unsigned long NCellularData::dcIxLookup(const Dim4Pentachoron* pen) const 
{ return lower_bound( dcIx[0].begin(), dcIx[0].end(), 
  tri4->pentachoronIndex(pen) ) - dcIx[0].begin(); }
unsigned long NCellularData::dcIxLookup(const Dim4Tetrahedron* tet) const 
{ return lower_bound( dcIx[1].begin(), dcIx[1].end(), 
  tri4->tetrahedronIndex(tet) ) - dcIx[1].begin(); }
unsigned long NCellularData::dcIxLookup(const Dim4Triangle* fac) const 
{ return lower_bound( dcIx[2].begin(), dcIx[2].end(), 
  tri4->triangleIndex(fac) ) - dcIx[2].begin(); }
unsigned long NCellularData::dcIxLookup(const Dim4Edge* edg) const 
{ return lower_bound( dcIx[3].begin(), dcIx[3].end(), 
  tri4->edgeIndex(edg) ) - dcIx[3].begin(); }
unsigned long NCellularData::dcIxLookup(const Dim4Vertex* vrt) const 
{ return lower_bound( dcIx[4].begin(), dcIx[4].end(), 
  tri4->vertexIndex(vrt) ) - dcIx[4].begin(); }

unsigned long NCellularData::bcIxLookup(const NVertex* vrt) const 
{ return lower_bound( bcIx[0].begin(), bcIx[0].end(), 
  tri3->vertexIndex(vrt) ) - bcIx[0].begin(); }
unsigned long NCellularData::bcIxLookup(const NEdge* edg) const 
{ return lower_bound( bcIx[1].begin(), bcIx[1].end(), 
  tri3->edgeIndex(edg) ) - bcIx[1].begin(); }
unsigned long NCellularData::bcIxLookup(const NTriangle* fac) const 
{ return lower_bound( bcIx[2].begin(), bcIx[2].end(), 
  tri3->faceIndex(fac) ) - bcIx[2].begin(); }

unsigned long NCellularData::bcIxLookup(const Dim4Vertex* vrt) const
{ return lower_bound( bcIx[0].begin(), bcIx[0].end(), 
  tri4->vertexIndex(vrt) ) - bcIx[0].begin(); }
unsigned long NCellularData::bcIxLookup(const Dim4Edge* edg) const
{ return lower_bound( bcIx[1].begin(), bcIx[1].end(), 
  tri4->edgeIndex(edg) ) - bcIx[1].begin(); }
unsigned long NCellularData::bcIxLookup(const Dim4Triangle* fac) const
{ return lower_bound( bcIx[2].begin(), bcIx[2].end(), 
  tri4->triangleIndex(fac) ) - bcIx[2].begin(); }
unsigned long NCellularData::bcIxLookup(const Dim4Tetrahedron* tet) const
{ return lower_bound( bcIx[3].begin(), bcIx[3].end(), 
  tri4->tetrahedronIndex(tet) ) - bcIx[3].begin(); }

unsigned long NCellularData::rIxLookup(const NVertex* vrt) const
{ return lower_bound( rIx[0].begin(), rIx[0].end(), 
  tri3->vertexIndex(vrt) ) - rIx[0].begin(); }
unsigned long NCellularData::rIxLookup(const NEdge* edg) const
{ return lower_bound( rIx[1].begin(), rIx[1].end(), 
  tri3->edgeIndex(edg) ) - rIx[1].begin(); }
unsigned long NCellularData::rIxLookup(const NTriangle* fac) const
{ return lower_bound( rIx[2].begin(), rIx[2].end(), 
  tri3->faceIndex(fac) ) - rIx[2].begin(); }
unsigned long NCellularData::rIxLookup(const NTetrahedron* tet) const
{ return lower_bound( rIx[3].begin(), rIx[3].end(), 
  tri3->tetrahedronIndex(tet) ) - rIx[3].begin(); }

unsigned long NCellularData::rIxLookup(const Dim4Vertex* vrt) const
{ return lower_bound( rIx[0].begin(), rIx[0].end(), 
  tri4->vertexIndex(vrt) ) - rIx[0].begin(); }
unsigned long NCellularData::rIxLookup(const Dim4Edge* edg) const
{ return lower_bound( rIx[1].begin(), rIx[1].end(), 
  tri4->edgeIndex(edg) ) - rIx[1].begin(); }
unsigned long NCellularData::rIxLookup(const Dim4Triangle* fac) const
{ return lower_bound( rIx[2].begin(), rIx[2].end(), 
  tri4->triangleIndex(fac) ) - rIx[2].begin(); }
unsigned long NCellularData::rIxLookup(const Dim4Tetrahedron* tet) const
{ return lower_bound( rIx[3].begin(), rIx[3].end(), 
  tri4->tetrahedronIndex(tet) ) - rIx[3].begin(); }
unsigned long NCellularData::rIxLookup(const Dim4Pentachoron* pen) const
{ return lower_bound( rIx[4].begin(), rIx[4].end(), 
  tri4->pentachoronIndex(pen) ) - rIx[4].begin(); }

// counts number of elements in thelist less than obj, 
//  defined in ncellulardata.init.pi1.cpp
unsigned long num_less_than(const std::set<unsigned long> &thelist, 
                            const unsigned long &obj); // forward ref

unsigned long NCellularData::pi1Lookup(const Dim4Triangle* fac) const 
// standard boundary
{ unsigned long I = bcIxLookup( fac );
  return I - num_less_than( maxTreeStB, I ); }
unsigned long NCellularData::pi1Lookup(const Dim4Tetrahedron* tet, 
                                       unsigned long num) const 
{ unsigned long I = icIxLookup( tet, num );
  return (numNonIdealBdryCells[2] - maxTreeStB.size()) + 
          I - num_less_than( maxTreeIdB, I ); } 
unsigned long NCellularData::pi1Lookup(const Dim4Tetrahedron* tet) const 
// standard 1-cells in interior
{ unsigned long I = nicIxLookup( tet );
  return (numNonIdealBdryCells[2] - maxTreeStB.size()) + 
    (numIdealCells[2] - maxTreeIdB.size()) +  
    (I - num_less_than( maxTreeStd, I )); }
unsigned long NCellularData::pi1Lookup(const Dim4Pentachoron* pen, 
    unsigned long num) const // ideal boundary connectors
{ unsigned long I = icIxLookup( pen, num );
  return (numNonIdealBdryCells[2] - maxTreeStB.size()) + 
         (numIdealCells[2] - maxTreeIdB.size())  + 
         (numNonIdealCells[3] - maxTreeStd.size()) + 
         (I - num_less_than(maxTreeSttIdB, I )); }

unsigned long NCellularData::pi1Lookup(const NEdge* edg) const
{ unsigned long I = bcIxLookup( edg );
  return I - num_less_than( maxTreeStB, I ); }
unsigned long NCellularData::pi1Lookup(const NTriangle* fac, 
 unsigned long num) const
{ unsigned long I = icIxLookup( fac, num );
  return (numNonIdealBdryCells[1] - maxTreeStB.size()) + I - 
    num_less_than( maxTreeIdB, I ); } 
unsigned long NCellularData::pi1Lookup(const NTriangle* fac) const
{ unsigned long I = nicIxLookup( fac );
  return (numNonIdealBdryCells[1] - maxTreeStB.size()) + 
         (numIdealCells[1] - maxTreeIdB.size())  + 
         (I - num_less_than( maxTreeStd, I )); }
unsigned long NCellularData::pi1Lookup(const NTetrahedron* tet, 
    unsigned long num) const
{ unsigned long I = icIxLookup( tet, num );
  return (numNonIdealBdryCells[1] - maxTreeStB.size()) + 
         (numIdealCells[1] - maxTreeIdB.size())  + 
         (numNonIdealCells[2] - maxTreeStd.size()) + 
         (I - num_less_than(maxTreeSttIdB, I )); }

unsigned long NCellularData::nicIndex( const unsigned long dim, 
                           const unsigned long idx ) const // nicIx
{ return nicIx[dim][idx]; }

std::pair< unsigned long, unsigned long > NCellularData::icIndex( 
    const unsigned long dim, const unsigned long idx ) const // nicIx
{ return std::pair< unsigned long, unsigned long >
    ( icIx[dim][idx]/(dim + 2), icIx[dim][idx] % (dim + 2) ); }

unsigned long NCellularData::dcIndex( const unsigned long dim, 
                       const unsigned long idx ) const // dcIx
{ return dcIx[dim][idx];}

unsigned long NCellularData::bcIndex( const unsigned long dim, 
                          const unsigned long idx ) const // bcIx;
{ return bcIx[dim][idx]; }

unsigned long NCellularData::rIndex( const unsigned long dim, 
                         const unsigned long idx ) const // rIx;
{ return rIx[dim][idx]; }

unsigned long NCellularData::stdCellCount( const unsigned long dim ) const
{ return ( dim < (tri3 ? 4 : 5 ) ? nicIx[dim].size() : 0); }

unsigned long NCellularData::idCellCount( const unsigned long dim ) const
{ return ( dim < (tri3 ? 3 : 4 ) ? icIx[dim].size() : 0); }


} // regina namespace
