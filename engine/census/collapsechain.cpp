
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

#include <boost/next_prior.hpp>
#include <sstream>
#include "census/ngluingpermsearcher.h"
#include "triangulation/nedge.h"
#include "triangulation/nfacepair.h"
#include "triangulation/ntriangulation.h"
#include "utilities/memutils.h"

namespace regina {

const char NCollapsedChainSearcher::dataTag_ = 'h';

NCollapsedChainSearcher::NCollapsedChainSearcher(const NFacePairing* pairing,
        const NFacePairing::IsoList* autos, bool orientableOnly,
        UseGluingPerms use, void* useArgs) :
        NGluingPermSearcher(pairing, autos, orientableOnly, true, // finiteOnly
            PURGE_NON_MINIMAL_PRIME | PURGE_P2_REDUCIBLE,
            use, useArgs), maxOrder(0), nChains(0) {
    // Preconditions:
    //     Only closed prime minimal P2-irreducible triangulations are needed.
    //     The given face pairing is closed with order >= 3.

    unsigned nTets = getNumberOfTetrahedra();

    modified = new NFacePairing(*pairing);

    // Begin by searching for tetrahedra that are joined to themselves.
    // Note that each tetrahedra can be joined to itself at most once,
    // since we are guaranteed that the face pairing is connected with
    // order >= 3.

    NTetFace face;
    for (face.setFirst(); ! face.isPastEnd(nTets, true); face++) {

        NTetFace adj = (*pairing)[face];
        if (adj.simp != face.simp)
            continue;
        collapseChain(NFacePair(face.facet, adj.facet), adj.simp);
    }
    maxOrder = orderElt;
}

NCollapsedChainSearcher::~NCollapsedChainSearcher() {
    delete modified;
}

void NCollapsedChainSearcher::runSearch(long maxDepth) {
    // Preconditions:
    //     Only closed prime minimal P2-irreducible triangulations are needed.
    //     The given face pairing is closed with order >= 3
    //     The given face pairing contains a chain of length >= 1

    // Is it a partial search that has already finished?
    // TODO
//    if (orderElt == static_cast<int>(nTets) * 2) {
//        if (isCanonical())
//            use_(this, useArgs_);
//        use_(0, useArgs_);
//        return;
//    }

    if (modified->size() < 3) {
        NClosedPrimeMinSearcher s(pairing_, autos_, orientableOnly_, use_,
                useArgs_);
        s.runSearch();
    } else {
        //void extendTri(const NGluingPermSearcher *s, void *useArgs);
        UseGluingPerms func = &NCollapsedChainSearcher::extendTriHelper;
        NClosedPrimeMinSearcher s(modified, NULL, orientableOnly_, func,
                this);
        s.runSearch();
    }
    use_(0, useArgs_);
}

void NCollapsedChainSearcher::extendTriHelper(const NGluingPermSearcher *s, void *
        useArgs) {
    NCollapsedChainSearcher *c = static_cast<NCollapsedChainSearcher*>(useArgs);
    c->extendTri(c->triangulate());
}

void NCollapsedChainSearcher::extendTri(NTriangulation *t) {
    orderElt = 0;
    // for each triangulation found.
    // apply reverse isomorphism
    // for each chain, move the gluing on the loop of the collapsed chain to the
    // loop of the full chain. then:
    while (orderElt < maxOrder) {
        NTetFace face = order[orderElt];
        NTetFace adj = (*pairing_)[face];

        // Note that we are only ever identifying faces corresponding to
        // parallel arcs that are part of a one-ended chain. That means we
        // have at most 2 options to test at each pair of arcs.
        if (permIndex(face) < 0)
            permIndex(face) = chainPermIndices[2*orderElt];
        else if (permIndex(face) == chainPermIndices[orderElt])
            permIndex(face) = chainPermIndices[2*orderElt+1];
        else {
            permIndex(face) = -1;
            permIndex(adj) = -1;
            orderElt--;
            continue;
        }
    }
}


void NCollapsedChainSearcher::dumpData(std::ostream& out) const {
    NGluingPermSearcher::dumpData(out);
//
//    int i;
//    for (i = 0; i < orderSize; i++) {
//        if (i)
//            out << ' ';
//        out << orderType[i];
//    }
//    out << std::endl;
//
//    out << nChainEdges << std::endl;
//    if (nChainEdges) {
//        for (i = 0; i < 2 * static_cast<int>(nChainEdges); i++) {
//            if (i)
//                out << ' ';
//            out << chainPermIndices[i];
//        }
//        out << std::endl;
//    }
}

NCollapsedChainSearcher::NCollapsedChainSearcher(std::istream& in,
        UseGluingPerms use, void* useArgs) :
        NGluingPermSearcher(in, use, useArgs)  {
//    if (inputError_)
//        return;
//
//    unsigned nTets = getNumberOfTetrahedra();
//    int i;
//
//    orderType = new unsigned[2 * nTets];
//    for (i = 0; i < orderSize; i++)
//        in >> orderType[i];
//
//    in >> nChainEdges;
//    /* Unnecessary since nChainEdges is unsigned.
//    if (nChainEdges < 0) {
//        inputError_ = true; return;
//    } */
//    if (nChainEdges) {
//        chainPermIndices = new int[nChainEdges * 2];
//        for (i = 0; i < 2 * static_cast<int>(nChainEdges); i++) {
//            in >> chainPermIndices[i];
//            if (chainPermIndices[i] < 0 || chainPermIndices[i] >= 6) {
//                inputError_ = true; return;
//            }
//        }
//    }
//
//    // Did we hit an unexpected EOF?
//    if (in.eof())
//        inputError_ = true;
}

void NCollapsedChainSearcher::collapseChain(NFacePair faces, int tet) {

    modified->unMatch(tet,faces.lower()); // Unmatch loop
    faces = faces.complement();

    NTetFace dest1, dest2;
    dest1 = pairing_->dest(tet, faces.lower());
    //dest1 = modified->dest(tet, faces.lower());
    dest2 = modified->dest(tet, faces.upper());
    // Currently tet and faces refer to the two faces of the base
    // tetrahedron that are pointing outwards.
    while (dest1.simp == dest2.simp && dest1.simp != tet) {
        order[orderElt] = NTetFace(tet, faces.lower());
        order[orderElt+1] = NTetFace(tet, faces.upper());

        NFacePair comp = faces.complement();
        NFacePair facesAdj = NFacePair(dest1.facet, dest2.facet);
        NFacePair compAdj = facesAdj.complement();

        NPerm4 trial1 = NPerm4(faces.lower(), facesAdj.lower(),
                        faces.upper(), compAdj.lower(),
                        comp.lower(), compAdj.upper(),
                        comp.upper(), facesAdj.upper());
        NPerm4 trial2 = NPerm4(faces.lower(), facesAdj.lower(),
                        faces.upper(), compAdj.upper(),
                        comp.lower(), compAdj.lower(),
                        comp.upper(), facesAdj.upper());
        if (trial1.compareWith(trial2) < 0) {
            chainPermIndices[2 * orderElt] = gluingToIndex(order[orderElt], trial1);
            chainPermIndices[2 * orderElt + 2] = gluingToIndex(order[orderElt + 1],
                NPerm4(faces.lower(), compAdj.upper(),
                        faces.upper(), facesAdj.upper(),
                        comp.lower(), facesAdj.lower(),
                        comp.upper(), compAdj.lower()));
        } else {
            chainPermIndices[2 * orderElt] = gluingToIndex(order[orderElt], trial2);
            chainPermIndices[2 * orderElt + 2] = gluingToIndex(order[orderElt + 1],
                NPerm4(faces.lower(), compAdj.lower(),
                        faces.upper(), facesAdj.upper(),
                        comp.lower(), facesAdj.lower(),
                        comp.upper(), compAdj.upper()));
        }

        trial1 = NPerm4(faces.lower(), facesAdj.lower(),
                        faces.upper(), compAdj.lower(),
                        comp.lower(), facesAdj.upper(),
                        comp.upper(), compAdj.upper());
        trial2 = NPerm4(faces.lower(), facesAdj.lower(),
                        faces.upper(), compAdj.upper(),
                        comp.lower(), facesAdj.upper(),
                        comp.upper(), compAdj.lower());
        if (trial1.compareWith(trial2) < 0) {
            chainPermIndices[2 * orderElt + 1] = gluingToIndex(order[orderElt], trial1);
            chainPermIndices[2 * orderElt + 3] = gluingToIndex(order[orderElt + 1],
                NPerm4(faces.lower(), compAdj.upper(),
                        faces.upper(), facesAdj.upper(),
                        comp.lower(), compAdj.lower(),
                        comp.upper(), facesAdj.lower()));
        } else {
            chainPermIndices[2 * orderElt + 1] = gluingToIndex(order[orderElt], trial2);
            chainPermIndices[2 * orderElt + 3] = gluingToIndex(order[orderElt + 1],
                NPerm4(faces.lower(), compAdj.lower(),
                        faces.upper(), facesAdj.upper(),
                        comp.lower(), compAdj.upper(),
                        comp.upper(), facesAdj.lower()));
        }
        modified->unMatch(tet,faces.lower());
        modified->unMatch(tet,faces.upper());
        faces = NFacePair(dest1.facet, dest2.facet);
        faces = faces.complement();
        orderElt += 2;

        // Store this tetrahedron as part of the chain.
        // chainTet[nChains].append(tet);

        tet = dest1.simp;
        dest1 = modified->dest(tet, faces.lower());
        dest2 = modified->dest(tet, faces.upper());

    }
    modified->match(dest1, dest2); // Add back the original loop.
    nChains+=1;
}

} // namespace regina

