
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2004, Ben Burton                                   *
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

/*! \file nfacepairing.h
 *  \brief Deals with pairing off tetrahedron faces in a triangulation
 *  census.
 */

#ifndef __NFACEPAIRING_H
#ifndef __DOXYGEN
#define __NFACEPAIRING_H
#endif

#include <list>
#include "utilities/nbooleans.h"
#include "utilities/nthread.h"
#include "triangulation/nisomorphism.h"

namespace regina {

class NFacePair;
class NFacePairing;

/**
 * \weakgroup census
 * @{
 */

/**
 * A list of isomorphisms on pairwise matchings of tetrahedron faces.
 *
 * Specifically, such an isomorphism can be used to convert one
 * pairwise matching of tetrahedron faces (as described by class
 * NFacePairing) into another.
 */
typedef std::list<NIsomorphismDirect*> NFacePairingIsoList;

/**
 * A routine used to do arbitrary processing upon a pairwise matching of
 * tetrahedron faces and its automorphisms.  Such routines are used to
 * process pairings found when running NFacePairing::findAllPairings().
 *
 * The first parameter passed should be a pairwise matching of
 * tetrahedron faces (this should not be deallocated by this routine).
 * The second parameter should be a list of all automorphisms of this pairing
 * (this should not be deallocated either).
 * The third parameter may contain arbitrary data as passed to
 * NFacePairing::findAllPairings().
 *
 * Note that the first two parameters passed might be \c null to signal that
 * face pairing generation has finished.
 */
typedef void (*UseFacePairing)(const NFacePairing*, const NFacePairingIsoList*,
    void*);

/**
 * Represents a specific pairwise matching of tetrahedron faces.
 * Given a fixed number of tetrahedra, each tetrahedron face is either
 * paired with some other tetrahedron face (which is in turn paired with
 * it) or remains unmatched.  A tetrahedron face cannot be paired with
 * itself.
 *
 * Such a matching models part of the structure of a triangulation, in
 * which each tetrahedron face is either glued to some other tetrahedron
 * face (which is in turn glued to it) or is an unglued boundary face.
 *
 * Note that if this pairing is used to construct an actual
 * triangulation, the individual gluing permutations will still need to
 * be specified; they are not a part of this structure.
 *
 * \testpart
 */
class NFacePairing : public NThread {
    private:
        unsigned nTetrahedra;
            /**< The number of tetrahedra under consideration. */

        NTetFace* pairs;
            /**< The other face to which each tetrahedron face is paired.
                 If a tetrahedron face is left unmatched, the corresponding
                 element of this array will be boundary (as returned by
                 NTetFace::isBoundary()).
                 If the destination for a particular face has not yet been
                 decided, the face will be paired to itself. */

    public:
        /**
         * Creates a new face pairing that is a clone of the given face
         * pairing.
         *
         * @param cloneMe the face pairing to clone.
         */
        NFacePairing(const NFacePairing& cloneMe);

        /**
         * Deallocates any memory used by this structure.
         */
        virtual ~NFacePairing();

        /**
         * Returns the number of tetrahedra whose faces are (potentially)
         * paired in this particular matching.
         *
         * @return the number of tetrahedra under consideration.
         */
        unsigned getNumberOfTetrahedra() const;

        /**
         * Returns the other face to which the given tetrahedron face is
         * paired.  If the given face is left deliberately unmatched, the value
         * returned will be boundary (as returned by
         * NTetFace::isBoundary()).
         *
         * \pre The given face is a real tetrahedron face (not boundary,
         * before-the-start or past-the-end).
         *
         * @param source the face under investigation.
         * @return the other face to which the given face is paired.
         */
        const NTetFace& dest(const NTetFace& source) const;

        /**
         * Returns the other face to which the given tetrahedron face is
         * paired.  If the given face is left deliberately unmatched, the value
         * returned will be boundary (as returned by
         * NTetFace::isBoundary()).
         *
         * @param tet the tetrahedron under investigation (this must be
         * strictly less than the total number of tetrahedra under
         * consideration).
         * @param face the face of the given tetrahedron under
         * investigation (between 0 and 3 inclusive).
         * @return the other face to which the given face is paired.
         */
        const NTetFace& dest(unsigned tet, unsigned face) const;

        /**
         * Returns the other face to which the given tetrahedron face is
         * paired.  This is a convenience operator whose behaviour is
         * identical to that of dest().
         *
         * If the given face is left deliberately unmatched, the value
         * returned will be boundary (as returned by NTetFace::isBoundary()).
         *
         * \pre The given face is a real tetrahedron face (not boundary,
         * before-the-start or past-the-end).
         *
         * @param source the face under investigation.
         * @return the other face to which the given face is paired.
         */
        const NTetFace& operator [](const NTetFace& source) const;

        /**
         * Determines whether the given tetrahedron face has been left
         * deliberately unmatched.
         *
         * \pre The given face is a real tetrahedron face (not boundary,
         * before-the-start or past-the-end).
         *
         * @param source the face under investigation.
         * @return \c true if the given face has been left unmatched, or
         * \c false if the given face is paired with some other face.
         */
        bool isUnmatched(const NTetFace& source) const;

        /**
         * Determines whether the given tetrahedron face has been left
         * deliberately unmatched.
         *
         * @param tet the tetrahedron under investigation (this must be
         * strictly less than the total number of tetrahedra under
         * consideration).
         * @param face the face of the given tetrahedron under
         * investigation (between 0 and 3 inclusive).
         * @return \c true if the given face has been left unmatched, or
         * \c false if the given face is paired with some other face.
         */
        bool isUnmatched(unsigned tet, unsigned face) const;

        /**
         * Determines whether this face pairing is in canonical form,
         * i.e., is a minimal representative of its isomorphism class.
         *
         * Isomorphisms of face pairings correspond to relabellings of
         * tetrahedra and relabellings of the four faces within each
         * tetrahedron.
         *
         * Face pairings are ordered by lexicographical comparison of
         * <tt>dest(0,0)</tt>, <tt>dest(0,1)</tt>, ...,
         * <tt>dest(n-1,3)</tt>, where <tt>n</tt> is the value of
         * <tt>getNumberOfTetrahedra()</tt>.
         *
         * @return \c true if and only if this face pairing is in
         * canonical form.
         */
        bool isCanonical() const;

        /**
         * Fills the given list with the set of all combinatorial
         * automorphisms of this face pairing.
         *
         * An automorphism is a relabelling of the tetrahedra and/or a
         * renumbering of the four faces of each tetrahedron resulting
         * in precisely the same face pairing.
         *
         * This routine uses optimisations that can cause unpredictable
         * breakages if this face pairing is not in canonical form.
         *
         * The automorphisms placed in the given list will be newly
         * created; it is the responsibility of the caller of this
         * routine to deallocate them.
         *
         * \pre The given list is empty.
         * \pre This face pairing is connected, i.e., it is possible
         * to reach any tetrahedron from any other tetrahedron via a
         * series of matched face pairs.
         * \pre This face pairing is in canonical form as described by
         * isCanonical().
         *
         * \ifacespython Not present.
         *
         * @param list the list into which the newly created automorphisms
         * will be placed.
         */
        void findAutomorphisms(NFacePairingIsoList& list) const;

        /**
         * Returns a human-readable representation of this face pairing.
         *
         * The string returned will contain no newlines.
         *
         * @return a string representation of this pairing.
         */
        std::string toString() const;

        /**
         * Returns a text-based representation of this face pairing that can be
         * used to reconstruct the face pairing.  This reconstruction is
         * done through routine fromTextRep().
         *
         * The text produced is not particularly readable; for a
         * human-readable text representation, see routine toString()
         * instead.
         *
         * The string returned will contain no newlines.
         *
         * @return a text-based representation of this face pairing.
         */
        std::string toTextRep() const;

        /**
         * Reconstructs a face pairing from a text-based representation.
         * This text-based representation must be in the format produced
         * by routine toTextRep().
         *
         * The face pairing returned will be newly constructed; it is the
         * responsibility of the caller of this routine to deallocate it.
         *
         * \pre The face pairing to be reconstructed involves at least
         * one tetrahedron.
         *
         * @param rep a text-based representation of a face pairing, as
         * produced by routine toTextRep().
         * @return the corresponding newly constructed face pairing, or
         * \c null if the given text-based representation was invalid.
         */
        static NFacePairing* fromTextRep(const std::string& rep);

        /**
         * Determines whether this face pairing is closed.
         * A closed face pairing has no unmatched faces.
         */
        bool isClosed() const;

        /**
         * Determines whether this face pairing contains a triple edge.
         * A triple edge is where two different tetrahedra are joined
         * along three of their faces.
         *
         * @return \c true if and only if this face pairing contains a
         * triple edge.
         */
        bool hasTripleEdge() const;

        /**
         * Follows a chain as far as possible from the given point.
         *
         * A chain is the underlying face pairing for a layered chain;
         * specifically it involves one tetrahedron joined to a second
         * along two faces, the remaining two faces of the second
         * tetrahedron joined to a third and so on.  A chain can involve
         * as few as one tetrahedron or as many as we like.  Note that
         * the remaining two faces of the first tetrahedron and the
         * remaining two faces of the final tetrahedron remain
         * unaccounted for by this structure.
         *
         * This routine begins with two faces of a given tetrahedron,
         * described by parameters \a tet and \a face.
         * If these two faces are both joined to some different
         * tetrahedron, parameter \a tet will be changed to this new
         * tetrahedron and parameter \a faces will be changed to the
         * remaining faces of this new tetrahedron (i.e., the two faces
         * that were not joined to the original faces of the original
         * tetrahedron).
         *
         * This procedure is repeated as far as possible until either
         * the two faces in question join to two different tetrahedra,
         * the two faces join to each other, or at least one of the
         * two faces is unmatched.
         *
         * Thus, given one end of a chain, this procedure can be used to
         * follow the face pairings through to the other end of the chain.
         *
         * \warning You must be sure when calling this routine that you
         * are not inside a chain that loops back onto itself!
         * If the face pairing forms a
         * large loop with each tetrahedron joined by two faces to the
         * next, this routine will cycle around the loop forever and
         * never return.
         *
         * @param tet the index in the underlying triangulation of the
         * tetrahedron to begin at.  This parameter will be modified
         * directly by this routine as a way of returning the results.
         * @param faces the pair of face numbers in the given
         * tetrahedron at which we begin.  This parameter will also be
         * modified directly by this routine as a way of returning results.
         */
        void followChain(unsigned& tet, NFacePair& faces) const;

        /**
         * Determines whether this face pairing contains a broken
         * double-ended chain.
         *
         * A chain involves a sequence of tetrahedra, each joined to the
         * next along two faces, and is described in detail in the
         * documentation for followChain().
         *
         * A one-ended chain is a chain in which the first tetrahedron
         * is also joined to itself along one face (i.e., the underlying
         * face pairing for a layered solid torus).  A double-ended
         * chain is a chain in which the first tetrahedron is joined to
         * itself along one face and the final tetrahedron is also
         * joined to itself along one face (i.e., the underlying
         * face pairing for a layered lens space).
         *
         * A broken double-ended chain consists of two one-ended chains
         * (using distinct sets of tetrahedra) joined together along one
         * face.  The remaining two faces (one from each chain)
         * that were unaccounted for by the individual one-ended chains
         * remain unaccounted for by this broken double-ended chain.
         *
         * In this routine we are interested specifically in finding a
         * broken double-ended chain that is not a part of a complete
         * double-ended chain, i.e., the final two unaccounted faces are
         * not joined together.
         *
         * @return \c true if and only if this face pairing contains a
         * broken double-ended chain that is not part of a complete
         * double-ended chain.
         */
        bool hasBrokenDoubleEndedChain() const;

        /**
         * Determines whether this face pairing contains a one-ended chain
         * with a double handle.
         *
         * A chain involves a sequence of tetrahedra, each joined to the
         * next along two faces, and is described in detail in the
         * documentation for followChain().
         *
         * A one-ended chain is a chain in which the first tetrahedron
         * is also joined to itself along one face (i.e., the underlying
         * face pairing for a layered solid torus).
         *
         * A one-ended chain with a double handle begins with a one-ended
         * chain.  The two faces that are unaccounted for by this
         * one-ended chain must be joined
         * to two different tetrahedra, and these two tetrahedra must be
         * joined to each other along two faces.  The remaining two
         * faces of these two tetrahedra remain unaccounted for by this
         * structure.
         *
         * @return \c true if and only if this face pairing contains a
         * one-ended chain with a double handle.
         */
        bool hasOneEndedChainWithDoubleHandle() const;

        /**
         * Internal to findAllPairings().  This routine should never be
         * called directly.
         *
         * Performs the actual generation of face pairings, possibly as a
         * separate thread.  At most one copy of this routine should be
         * running at any given time for a particular NFacePairing instance.
         *
         * \ifacespython Not present.
         *
         * @param param a structure containing the parameters that were
         * passed to findAllPairings().
         * @return the value 0.
         */
        void* run(void* param);

        /**
         * Generates all possible face pairings satisfying the given
         * constraints.  Only connected face pairings (pairings in which
         * each tetrahedron can be reached from each other via a series of
         * individual matched faces) will be produced.
         *
         * Each face pairing will be produced precisely once up to
         * isomorphism.  Face pairings are considered isomorphic if they
         * are related by a relabelling of the tetrahedra and/or a
         * renumbering of the four faces of each tetrahedron.  Each face
         * pairing that is generated will be a minimal representative of
         * its isomorphism class, i.e., will be in canonical form as
         * described by isCanonical().
         *
         * For each face pairing that is generated, routine \a use (as
         * passed to this function) will be called with that pairing and
         * its automorphisms as arguments.
         *
         * Once the generation of face pairings has finished, routine
         * \a use will be called once more, this time with \c null as its
         * first two arguments (the face pairing and its automorphisms).
         *
         * The face pairing generation may be run in the current thread
         * or as a separate thread.
         *
         * \todo \optlong When generating face pairings, do some checking to
         * eliminate cases in which tetrahedron (<i>k</i> > 0) can be swapped
         * with tetrahedron 0 to produce a smaller representation of the same
         * pairing.
         * \todo \feature Allow cancellation of face pairing generation.
         *
         * \ifacespython Not present.
         *
         * @param nTetrahedra the number of tetrahedra whose faces should
         * be (potentially) matched.
         * @param boundary determines whether any faces may be left
         * unmatched.  This set should contain \c true if pairings with at
         * least one unmatched face are to be generated, and should contain
         * \c false if pairings with no unmatched faces are to be generated.
         * @param nBdryFaces specifies the precise number of faces that
         * should be left unmatched.  If this parameter is negative, it
         * is ignored and no additional restriction is imposed.  If
         * parameter \a boundary does not contain \c true, this parameter
         * is likewise ignored.  If parameter \a boundary does contain
         * true and this parameter is non-negative, only pairings with
         * precisely this many unmatched faces will be generated.
         * In particular, if this parameter is positive then pairings
         * with no unmatched faces will not be produced irrespective of
         * whether \c false is contained in parameter \a boundary.
         * Note that, in order to produce any pairings at all, this
         * parameter must be even and at most 2<i>T</i>+2, where <i>T</i>
         * is the number of tetrahedra requested.
         * @param use the function to call upon each face pairing that is
         * found.  The first parameter passed to this function will be a
         * face pairing.  The second parameter will be a list of all its
         * automorphisms (relabellings of tetrahedra and individual
         * tetrahedron faces that produce the exact same pairing).
         * The third parameter will be parameter \a useArgs as was passed
         * to this routine.
         * @param useArgs the pointer to pass as the final parameter for
         * the function \a use which will be called upon each pairing found.
         * @param newThread \c true if face pairing generation should be
         * performed in a separate thread, or \c false if generation
         * should take place in the current thread.  If this parameter is
         * \c true, this routine will exit immediately (after spawning
         * the new thread).
         * @return \c true if the new thread was successfully started (or
         * if face pairing generation has taken place in the current thread),
         * or \c false if the new thread could not be started.
         */
        static bool findAllPairings(unsigned nTetrahedra,
            NBoolSet boundary, int nBdryFaces, UseFacePairing use,
            void* useArgs = 0, bool newThread = false);

    private:
        /**
         * Creates a new face pairing.  All internal arrays will be
         * allocated but not initialised.
         *
         * \pre \a newNTetrahedra is at least 1.
         *
         * @param newTetrahedra the number of tetrahedra under
         * consideration in this new face pairing.
         */
        NFacePairing(unsigned newNTetrahedra);

        /**
         * Returns the other face to which the given tetrahedron face is
         * paired.  If the given face is left deliberately unmatched, the value
         * returned will be boundary (as returned by
         * NTetFace::isBoundary()).
         *
         * \pre The given face is a real tetrahedron face (not boundary,
         * before-the-start or past-the-end).
         *
         * @param source the face under investigation.
         * @return the other face to which the given face is paired.
         */
        NTetFace& dest(const NTetFace& source);

        /**
         * Returns the other face to which the given tetrahedron face is
         * paired.  If the given face is left deliberately unmatched, the value
         * returned will be boundary (as returned by
         * NTetFace::isBoundary()).
         *
         * @param tet the tetrahedron under investigation (this must be
         * strictly less than the total number of tetrahedra under
         * consideration).
         * @param face the face of the given tetrahedron under
         * investigation (between 0 and 3 inclusive).
         * @return the other face to which the given face is paired.
         */
        NTetFace& dest(unsigned tet, unsigned face);

        /**
         * Returns the other face to which the given tetrahedron face is
         * paired.  This is a convenience operator whose behaviour is
         * identical to that of dest().
         *
         * If the given face is left deliberately unmatched, the value
         * returned will be boundary (as returned by NTetFace::isBoundary()).
         *
         * \pre The given face is a real tetrahedron face (not boundary,
         * before-the-start or past-the-end).
         *
         * @param source the face under investigation.
         * @return the other face to which the given face is paired.
         */
        NTetFace& operator [](const NTetFace& source);

        /**
         * Determines whether the matching for the given tetrahedron face
         * has not yet been determined.  This is signalled by a face
         * matched to itself.
         *
         * \pre The given face is a real tetrahedron face (not boundary,
         * before-the-start or past-the-end).
         *
         * @param source the face under investigation.
         * @return \c true if the matching for the given face has not yet
         * been determined, or \c false otherwise.
         */
        bool noDest(const NTetFace& source) const;

        /**
         * Determines whether the matching for the given tetrahedron face
         * has not yet been determined.  This is signalled by a face
         * matched to itself.
         *
         * @param tet the tetrahedron under investigation (this must be
         * strictly less than the total number of tetrahedra under
         * consideration).
         * @param face the face of the given tetrahedron under
         * investigation (between 0 and 3 inclusive).
         * @return \c true if the matching for the given face has not yet
         * been determined, or \c false otherwise.
         */
        bool noDest(unsigned tet, unsigned face) const;

        /**
         * Determines whether this face pairing is in canonical
         * (smallest lexicographical) form, given a small set of
         * assumptions.
         *
         * If this face pairing is in canonical form, the given list
         * will be filled with the set of all combinatorial automorphisms
         * of this face pairing.  If not, the given list will be left empty.
         *
         * \pre The given list is empty.
         * \pre For each tetrahedron \a t, it is true that
         * the sequence <tt>dest(t,0)</tt>, <tt>dest(t,1)</tt>,
         * <tt>dest(t,2)</tt>, <tt>dest(t,3)</tt> is non-decreasing.
         * \pre For each tetrahedron \a t > 0, it is true that
         * <tt>dest(t,0).tet < t</tt>.
         * \pre The sequence <tt>dest(1,0)</tt>, <tt>dest(2,0)</tt>,
         * ..., <tt>dest(n-1,0)</tt> is strictly increasing, where
         * \a n is the total number of tetrahedra under investigation.
         *
         * @param list the list into which automorphisms will be placed
         * if appropriate.
         * @return \c true if and only if this face pairing is in
         * canonical form.
         */
        bool isCanonicalInternal(NFacePairingIsoList& list) const;

        /**
         * Internal to hasBrokenDoubleEndedChain().  This routine assumes
         * that the give face of the given tetrahedron is glued to this
         * same tetrahedron, and attempts to find a broken double-ended
         * chain for which this tetrahedron is at the end of one of the
         * one-ended chains therein.
         *
         * \pre The given face of the given tetrahedron is paired with
         * another face of the same tetrahedron under this face pairing.
         *
         * @param tet the index in the triangulation of the given
         * tetrahedron.
         * @param face the number of the given face in the tetrahedron;
         * this must be between 0 and 3 inclusive.
         *
         * @return \c true if and only if this face pairing contains a
         * broken double-ended chain as described above.
         */
        bool hasBrokenDoubleEndedChain(unsigned tet, unsigned face) const;

        /**
         * Internal to hasOneEndedChainWithDoubleHandle().  This routine
         * assumes that the give face of the given tetrahedron is glued
         * to this same tetrahedron, and attempts to find a one-ended
         * chain with a double handle for which this tetrahedron is at
         * the end of the chain contained therein.
         *
         * \pre The given face of the given tetrahedron is paired with
         * another face of the same tetrahedron under this face pairing.
         *
         * @param tet the index in the triangulation of the given
         * tetrahedron.
         * @param face the number of the given face in the tetrahedron;
         * this must be between 0 and 3 inclusive.
         *
         * @return \c true if and only if this face pairing contains a
         * one-ended chain with a double handle as described above.
         */
        bool hasOneEndedChainWithDoubleHandle(unsigned tet,
            unsigned face) const;
};

/*@}*/

// Inline functions for NFacePairing

inline NFacePairing::NFacePairing(unsigned newNTetrahedra) :
        nTetrahedra(newNTetrahedra), pairs(new NTetFace[newNTetrahedra * 4]) {
}

inline NFacePairing::~NFacePairing() {
    delete[] pairs;
}

inline unsigned NFacePairing::getNumberOfTetrahedra() const {
    return nTetrahedra;
}

inline NTetFace& NFacePairing::dest(const NTetFace& source) {
    return pairs[4 * source.tet + source.face];
}
inline NTetFace& NFacePairing::operator [](const NTetFace& source) {
    return pairs[4 * source.tet + source.face];
}
inline NTetFace& NFacePairing::dest(unsigned tet, unsigned face) {
    return pairs[4 * tet + face];
}
inline const NTetFace& NFacePairing::dest(const NTetFace& source) const {
    return pairs[4 * source.tet + source.face];
}
inline const NTetFace& NFacePairing::operator [](const NTetFace& source)
        const {
    return pairs[4 * source.tet + source.face];
}
inline const NTetFace& NFacePairing::dest(unsigned tet, unsigned face) const {
    return pairs[4 * tet + face];
}
inline bool NFacePairing::isUnmatched(const NTetFace& source) const {
    return pairs[4 * source.tet + source.face].isBoundary(nTetrahedra);
}
inline bool NFacePairing::isUnmatched(unsigned tet, unsigned face) const {
    return pairs[4 * tet + face].isBoundary(nTetrahedra);
}

inline bool NFacePairing::noDest(const NTetFace& source) const {
    return dest(source) == source;
}
inline bool NFacePairing::noDest(unsigned tet, unsigned face) const {
    NTetFace& f = pairs[4 * tet + face];
    return (f.tet == static_cast<int>(tet) &&
        f.face == static_cast<int>(face));
}

inline void NFacePairing::findAutomorphisms(NFacePairingIsoList& list) const {
    isCanonicalInternal(list);
}

} // namespace regina

#endif

