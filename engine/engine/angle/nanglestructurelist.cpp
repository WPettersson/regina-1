
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2003, Ben Burton                                   *
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
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,        *
 *  MA 02111-1307, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include "angle/nanglestructurelist.h"
#include "enumerate/ndoubledescriptor.h"
#include "triangulation/ntriangulation.h"
#include "maths/nmatrixint.h"
#include "maths/nvectorunit.h"
#include "surfaces/nnormalsurface.h"
#include "file/nfile.h"
#include "utilities/xmlutils.h"

// Property IDs:
#define PROPID_ALLOWSTRICT 1
#define PROPID_ALLOWTAUT 2

namespace regina {

typedef std::vector<NAngleStructure*>::const_iterator StructureIteratorConst;

NAngleStructureList::NAngleStructureList() {
}

NAngleStructureList::NAngleStructureList(NTriangulation* owner) {
    owner->insertChildLast(this);

    // Form the matching equations (one per non-boundary edge plus
    // one per tetrahedron).
    unsigned long nTetrahedra = owner->getNumberOfTetrahedra();
    unsigned long nCoords = 3 * nTetrahedra + 1;

    long nEquations = long(owner->getNumberOfEdges()) +
        long(owner->getNumberOfTetrahedra());
    for (NTriangulation::BoundaryComponentIterator bit =
            owner->getBoundaryComponents().begin();
            bit != owner->getBoundaryComponents().end(); bit++)
        nEquations -= (*bit)->getNumberOfEdges();

    NMatrixInt eqns(nEquations, nCoords);
    unsigned long row = 0;

    std::deque<NEdgeEmbedding>::const_iterator embit;
    NPerm perm;
    unsigned long index;
    for (NTriangulation::EdgeIterator eit = owner->getEdges().begin();
            eit != owner->getEdges().end(); eit++) {
        if ((*eit)->isBoundary())
            continue;
        for (embit = (*eit)->getEmbeddings().begin();
                embit != (*eit)->getEmbeddings().end(); embit++) {
            index = owner->getTetrahedronIndex((*embit).getTetrahedron());
            perm = (*embit).getVertices();
            eqns.entry(row, 3 * index + vertexSplit[perm[0]][perm[1]]) += 1;
        }
        eqns.entry(row, nCoords - 1) = -2;
        row++;
    }
    for (index = 0; index < nTetrahedra; index++) {
        eqns.entry(row, 3 * index) = 1;
        eqns.entry(row, 3 * index + 1) = 1;
        eqns.entry(row, 3 * index + 2) = 1;
        eqns.entry(row, nCoords - 1) = -1;
        row++;
    }

    // Form the starting cone.
    std::list<NAngleStructureVector*> originalCone;
    NAngleStructureVector* vector;
    NLargeInteger startValue(nCoords - 1);
    for (index = 0; index < nCoords - 1; index++) {
        vector = new NAngleStructureVector(nCoords);
        vector->setElement(index, startValue);
        vector->setElement(nCoords - 1, NLargeInteger::one);
        originalCone.push_back(vector);
    }
    vector = new NAngleStructureVector(nCoords);
    vector->setElement(nCoords - 1, NLargeInteger::one);
    originalCone.push_back(vector);

    // Form the face list.
    std::list<NVector<NLargeInteger>*> faces;
    for (index = 0; index < nCoords - 1; index++)
        faces.push_back(new NVectorUnit<NLargeInteger>(nCoords, index));
    NVectorDense<NLargeInteger>* finalFace =
        new NVectorDense<NLargeInteger>(nCoords, NLargeInteger::one);
    finalFace->setElement(nCoords - 1, -startValue);
    faces.push_back(finalFace);

    // Find the angle structures.
    NDoubleDescriptor().enumerateVertices(StructureInserter(*this, owner),
        originalCone.begin(), originalCone.end(), faces.begin(), faces.end(),
        eqns, 0);

    // Tidy up.
    for_each(originalCone.begin(), originalCone.end(),
        FuncDelete<NAngleStructureVector>());
    for_each(faces.begin(), faces.end(),
        FuncDelete<NVector<NLargeInteger> >());
}

NTriangulation* NAngleStructureList::getTriangulation() const {
    return dynamic_cast<NTriangulation*>(getTreeParent());
}

void NAngleStructureList::writeTextShort(std::ostream& o) const {
    o << structures.size() << " vertex angle structure";
    if (structures.size() != 1)
        o << 's';
}

void NAngleStructureList::writeTextLong(std::ostream& o) const {
    writeTextShort(o);
    o << ":\n";

    for (StructureIteratorConst it = structures.begin();
            it != structures.end(); it++) {
        (*it)->writeTextShort(o);
        o << '\n';
    }
}

void NAngleStructureList::writePacket(NFile& out) const {
    out.writeULong(structures.size());
    for (StructureIteratorConst it = structures.begin();
            it != structures.end(); it++)
        (*it)->writeToFile(out);

    // Write the properties.
    std::streampos bookmark(0);

    if (doesAllowStrict.known()) {
        bookmark = out.writePropertyHeader(PROPID_ALLOWSTRICT);
        out.writeBool(doesAllowStrict.value());
        out.writePropertyFooter(bookmark);
    }
    if (doesAllowTaut.known()) {
        bookmark = out.writePropertyHeader(PROPID_ALLOWTAUT);
        out.writeBool(doesAllowTaut.value());
        out.writePropertyFooter(bookmark);
    }

    out.writeAllPropertiesFooter();
}

NAngleStructureList* NAngleStructureList::readPacket(NFile& in,
        NPacket* parent) {
    NAngleStructureList* ans = new NAngleStructureList();

    unsigned long nStructures = in.readULong();
    for (unsigned long i=0; i<nStructures; i++)
        ans->structures.push_back(NAngleStructure::readFromFile(in,
            dynamic_cast<NTriangulation*>(parent)));

    // Read the properties.
    in.readProperties(ans);

    return ans;
}

void NAngleStructureList::writeXMLPacketData(std::ostream& out) const {
    using regina::xml::xmlValueTag;

    // Write the individual structures.
    for (StructureIteratorConst it = structures.begin();
            it != structures.end(); it++)
        (*it)->writeXMLData(out);

    // Write the properties.
    if (doesAllowStrict.known())
        out << "  " << xmlValueTag("allowstrict", doesAllowStrict.value())
            << '\n';
    if (doesAllowTaut.known())
        out << "  " << xmlValueTag("allowtaut", doesAllowTaut.value())
            << '\n';
}

NPacket* NAngleStructureList::internalClonePacket(NPacket* /* parent */)
        const {
    NAngleStructureList* ans = new NAngleStructureList();
    transform(structures.begin(), structures.end(),
        back_inserter(ans->structures), FuncNewClonePtr<NAngleStructure>());

    if (doesAllowStrict.known())
        ans->doesAllowStrict = doesAllowStrict;
    if (doesAllowTaut.known())
        ans->doesAllowTaut = doesAllowTaut;

    return ans;
}

void NAngleStructureList::readIndividualProperty(NFile& infile,
        unsigned propType) {
    if (propType == PROPID_ALLOWSTRICT)
        doesAllowStrict = infile.readBool();
    else if (propType == PROPID_ALLOWTAUT)
        doesAllowTaut = infile.readBool();
}

void NAngleStructureList::calculateAllowStrict() const {
    if (structures.empty()) {
        doesAllowStrict = false;
        return;
    }

    unsigned long nTets = getTriangulation()->getNumberOfTetrahedra();
    if (nTets == 0) {
        doesAllowStrict = true;
        return;
    }

    // We run into trouble if there's a 0 or pi angle that never changes.
    NRational* fixedAngles = new NRational[nTets * 3];
    unsigned long nFixed = 0;

    // Get the list of bad unchanging angles from the first structure.
    StructureIteratorConst it = structures.begin();
    const NAngleStructure* s = *it;

    NRational angle;
    unsigned long tet;
    int edges;
    for (tet = 0; tet < nTets; tet++)
        for (edges = 0; edges < 3; edges++) {
            angle = s->getAngle(tet, edges);
            if (angle == NRational::zero || angle == NRational::one) {
                fixedAngles[3 * tet + edges] = angle;
                nFixed++;
            } else
                fixedAngles[3 * tet + edges] = NRational::undefined;
        }

    if (nFixed == 0) {
        doesAllowStrict = true;
        delete[] fixedAngles;
        return;
    }

    // Run through the rest of the structures to see if these bad angles
    // do ever change.
    for (it++; it != structures.end(); it++) {
        s = *it;
        for (tet = 0; tet < nTets; tet++)
            for (edges = 0; edges < 3; edges++) {
                if (fixedAngles[3 * tet + edges] == NRational::undefined)
                    continue;
                if (s->getAngle(tet, edges) != fixedAngles[3 * tet + edges]) {
                    // Here's a bad angle that finally changed.
                    fixedAngles[3 * tet + edges] = NRational::undefined;
                    nFixed--;
                    if (nFixed == 0) {
                        doesAllowStrict = true;
                        delete[] fixedAngles;
                        return;
                    }
                }
            }
    }

    // Some of the bad angles never changed.
    doesAllowStrict = false;
    delete[] fixedAngles;
}

void NAngleStructureList::calculateAllowTaut() const {
    doesAllowTaut = (find_if(structures.begin(), structures.end(),
        std::mem_fun(&NAngleStructure::isTaut)) != structures.end());
}

} // namespace regina

