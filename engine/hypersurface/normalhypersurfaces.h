
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

/*! \file hypersurface/normalhypersurfaces.h
 *  \brief Contains a packet representing a collection of normal
 *  hypersurfaces in a 4-manifold triangulation.
 */

#ifndef __REGINA_NORMALHYPERSURFACES_H
#ifndef __DOXYGEN
#define __REGINA_NORMALHYPERSURFACES_H
#endif

#include <algorithm>
#include <iterator>
#include <optional>
#include <vector>
#include "regina-core.h"
#include "enumerate/enumconstraints.h"
#include "hypersurface/hypercoords.h"
#include "hypersurface/hyperflags.h"
#include "hypersurface/normalhypersurface.h"
#include "maths/matrix.h"
#include "packet/packet.h"
#include "utilities/listview.h"

namespace regina {

class NormalHypersurface;
class NormalHypersurfaces;
class ProgressTracker;
class XMLPacketReader;

/**
 * \weakgroup hypersurface
 * @{
 */

#ifndef __DOXYGEN // Doxygen complains about undocumented specialisations.
template <>
struct PacketInfo<PACKET_NORMALHYPERSURFACES> {
    typedef NormalHypersurfaces Class;
    static constexpr const char* name = "Normal Hypersurface List";
};
#endif

/**
 * A packet representing a collection of normal hypersurfaces in a 4-manifold
 * triangulation.  Such a packet must always be a child packet of the
 * triangulation from which the surfaces were obtained.  If this triangulation
 * changes, the information contained in this packet will become invalid.
 *
 * See the NormalHypersurfaceVector class notes for details of what to do
 * when introducing a new coordinate system.
 *
 * Normal hypersurface lists should be created using the routine enumerate().
 */
class NormalHypersurfaces : public Packet {
    REGINA_PACKET(NormalHypersurfaces, PACKET_NORMALHYPERSURFACES)

    public:
        class VectorIterator;

    protected:
        std::vector<NormalHypersurface> surfaces_;
            /**< Contains the normal hypersurfaces stored in this packet. */
        HyperCoords coords_;
            /**< Stores which coordinate system is being
                 used by the normal hypersurfaces in this packet. */
        HyperList which_;
            /**< Indicates which normal hypersurfaces these represent
                 within the underlying triangulation. */
        HyperAlg algorithm_;
            /**< Stores the details of the enumeration algorithm that
                 was used to generate this list.  This might not be the
                 same as the \a algorithmHints flag passed to the
                 corresponding enumeration routine (e.g., if invalid or
                 inappropriate flags were passed). */

    public:
        /**
         * A unified routine for enumerating various classes of normal
         * hypersurfaces within a given triangulation.
         *
         * The HyperCoords argument allows you to specify an underlying
         * coordinate system.
         *
         * The HyperList argument is a combination of flags that
         * allows you to specify exactly which normal hypersurfaces you require.
         * This includes (i) whether you want all vertex hypersurfaces
         * or all fundamental hypersurfaces, which defaults to HS_VERTEX
         * if you specify neither or both; and (ii) whether you want only
         * properly embedded surfaces or you also wish to include
         * immersed and/or singular surfaces, which defaults to
         * HS_EMBEDDED_ONLY if you specify neither or both.
         *
         * The HyperAlg argument is a combination of flags that allows
         * you to control the underlying enumeration algorithm.  These
         * flags are treated as hints only: if your selection of
         * algorithm is invalid, unavailable or unsupported then Regina
         * will choose something more appropriate.  Unless you have
         * some specialised need, the default HS_ALG_DEFAULT (which
         * makes no hints at all) will allow Regina to choose what it
         * thinks will be the most efficient method.
         *
         * The enumerated hypersurfaces will be stored in a new normal
         * hypersurface list, and their representations will be scaled down
         * to use the smallest possible integer coordinates.
         * This normal hypersurface list will be inserted into the packet tree
         * as the last child of the given triangulation.  This triangulation
         * \b must remain the parent of this normal hypersurface list, and must
         * not change while this normal hypersurface list remains in existence.
         *
         * If a progress tracker is passed, the normal hypersurface
         * enumeration will take place in a new thread and this routine
         * will return immediately.  If the user cancels the operation
         * from another thread, then the normal surface list will \e not
         * be inserted into the packet tree (but the caller of this
         * routine will still need to delete it).  Regarding progress tracking,
         * this routine will declare and work through a series of stages
         * whose combined weights sum to 1; typically this means that the
         * given tracker must not have been used before.
         *
         * If no progress tracker is passed, the enumeration will run
         * in the current thread and this routine will return only when
         * the enumeration is complete.  Note that this enumeration can
         * be extremely slow for larger triangulations.
         *
         * If an error occurs, then this routine will return \c null,
         * no normal hypersurface list will be created, and the progress
         * tracker (if passed) will be marked as finished.  Errors can occur
         * in the following scenarios:
         *
         * - Regina could not create the matching equations for the given
         *   triangulation in the given coordinate system.  This is only
         *   possible in certain coordinate systems, and all such coordinate
         *   systems are marked as such in the HyperCoords enum documentation.
         *
         * - A progress tracker is passed but a new thread could not be started.
         *
         * @param owner the triangulation upon which this list of normal
         * hypersurfaces will be based.
         * @param coords the coordinate system to be used.
         * @param which indicates which normal hypersurfaces should be
         * enumerated.
         * @param algHints passes requests to Regina for which specific
         * enumeration algorithm should be used.
         * @param tracker a progress tracker through which progress will
         * be reported, or \c null if no progress reporting is required.
         * @return the newly created normal hypersurface list.  Note that if
         * a progress tracker is passed then this list may not be completely
         * filled when this routine returns.  If an error occurs (as
         * described above) then this routine will return \c null instead.
         */
        static NormalHypersurfaces* enumerate(Triangulation<4>& owner,
            HyperCoords coords,
            HyperList which = HS_LIST_DEFAULT,
            HyperAlg algHints = HS_ALG_DEFAULT,
            ProgressTracker* tracker = nullptr);

        /**
         * Returns the coordinate system being used by the
         * hypersurfaces stored in this set.
         *
         * @return the coordinate system used.
         */
        HyperCoords coords() const;
        /**
         * Returns details of which normal hypersurfaces this list represents
         * within the underlying triangulation.
         *
         * This may not be the same HyperList that was passed to enumerate().
         * In particular, default values will have been explicitly
         * filled in (such as HS_VERTEX and/or HS_EMBEDDED_ONLY), and
         * invalid and/or redundant values will have been removed.
         *
         * @return details of what this list represents.
         */
        HyperList which() const;
        /**
         * Returns details of the algorithm that was used to enumerate
         * this list.
         *
         * These may not be the same HyperAlg flags that were passed to
         * enumerate().  In particular, default values will have been
         * explicitly filled in, invalid and/or redundant values will have
         * been removed, and unavailable and/or unsupported combinations
         * of algorithm flags will be replaced with whatever algorithm
         * was actually used.
         *
         * @return details of the algorithm used to enumerate this list.
         */
        HyperAlg algorithm() const;
        /**
         * Returns whether this set is known to contain only embedded normal
         * hypersurfaces.
         *
         * If this returns \c false, it does not guarantee that immersed
         * and/or singular hypersurfaces are present; it merely indicates
         * that they were not deliberately excluded (for instance, the
         * prism constraints were not enforced).
         *
         * @return \c true if this list was constructed to contain only
         * properly embedded hypersurfaces, or \c false otherwise.
         */
        bool isEmbeddedOnly() const;
        /**
         * Returns the triangulation in which these normal hypersurfaces live.
         *
         * The triangulation is also accessible via the packet tree as
         * parent(); this routine simply adds the convenience of casting
         * down to the correct triangulation class.
         *
         * If you need non-const access to the triangulation (e.g., to
         * rename the packet), use parent(); however, remember that a
         * triangulation that owns normal hypersurfaces must \e not change
         * its tetrahedra or their gluings.
         *
         * @return a reference to the triangulation in which these
         * hypersurfaces live.
         */
        const Triangulation<4>& triangulation() const;

        /**
         * Returns the number of hypersurfaces stored in this list.
         *
         * @return the number of hypersurfaces.
         */
        size_t size() const;
        /**
         * Returns an object that allows iteration through and random access
         * to all normal hypersurfaces in this list.
         *
         * The object that is returned is lightweight, and can be happily
         * copied by value.  The C++ type of the object is subject to change,
         * so C++ users should use \c auto (just like this declaration does).
         *
         * The returned object is guaranteed to be an instance of ListView,
         * which means it offers basic container-like functions and supports
         * C++11 range-based \c for loops.  Note that the elements of the list
         * will be pointers, so your code might look like:
         *
         * \code{.cpp}
         * for (const NormalHypersurface* s : list.hypersurfaces()) { ... }
         * \endcode
         *
         * The object that is returned will remain valid only for as
         * long as this normal hypersurface list exists.
         *
         * \ifacespython This routine returns a Python list.
         *
         * @return access to the list of all normal hypersurfaces.
         */
        auto hypersurfaces() const;
        /**
         * Returns the hypersurface at the requested index in this list.
         *
         * @param index the index of the requested hypersurface in this list;
         * this must be between 0 and size()-1 inclusive.
         *
         * @return the normal hypersurface at the requested index in this list.
         */
        const NormalHypersurface& hypersurface(size_t index) const;

        virtual void writeTextShort(std::ostream& out) const override;
        virtual void writeTextLong(std::ostream& out) const override;
        static XMLPacketReader* xmlReader(Packet* parent,
            XMLTreeResolver& resolver);
        virtual bool dependsOnParent() const override;

        /**
         * Sorts the hypersurfaces in this list according to the given
         * criterion.
         *
         * This sort is stable, i.e., hypersurfaces that are equivalent under
         * the given criterion will remain in the same relative order.
         *
         * The implementation of this routine uses std::stable_sort.
         *
         * \ifacespython Not present.
         *
         * @param comp a binary function (or function object) that
         * accepts two const HyperSurface references, and returns \c true
         * if and only if the first hypersurface should appear before the
         * second in the sorted list.
         */
        template <typename Comparison>
        void sort(Comparison&& comp);

        /**
         * Returns the matching equations that were used to create this
         * normal hypersurface list.
         * This matrix is not cached: multiple calls to this routine will
         * result in the construction of multiple matrices.  This
         * routine in fact merely calls makeMatchingEquations() with the
         * appropriate parameters.
         *
         * The format of the matrix is identical to that returned by
         * makeMatchingEquations().
         *
         * Note that there are situations in which makeMatchingEquations()
         * returns no value (because the triangulation is not supported
         * by the chosen coordinate system).  However, this routine will
         * always return a value, because if makeMatchingEquations() had
         * returned no value then this normal hypersurface list would not have
         * been created in the first place.
         *
         * @return the matching equations used to create this normal
         * hypersurface list.
         */
        MatrixInt recreateMatchingEquations() const;

        /**
         * An iterator that gives access to the raw vectors for hypersurfaces
         * in this list, pointing to the beginning of this hypersurface list.
         *
         * \ifacespython Not present.
         *
         * @return an iterator at the beginning of this hypersurface list.
         */
        VectorIterator beginVectors() const;

        /**
         * An iterator that gives access to the raw vectors for hypersurfaces
         * in this list, pointing past the end of this hypersurface list.
         * This iterator is not dereferenceable.
         *
         * \ifacespython Not present.
         *
         * @return an iterator past the end of this hypersurface list.
         */
        VectorIterator endVectors() const;

        /**
         * A bidirectional iterator that runs through the raw vectors for
         * hypersurfaces in this list.
         *
         * \ifacespython Not present.
         */
        class VectorIterator : public std::iterator<
                std::bidirectional_iterator_tag, Vector<LargeInteger>> {
            private:
                std::vector<NormalHypersurface>::const_iterator it_;
                    /**< An iterator into the underlying list of
                         hypersurfaces. */

            public:
                /**
                 * Creates a new uninitialised iterator.
                 */
                VectorIterator();

                /**
                 * Creates a copy of the given iterator.
                 *
                 * @param cloneMe the iterator to clone.
                 */
                VectorIterator(const VectorIterator& cloneMe) = default;

                /**
                 * Makes this a copy of the given iterator.
                 *
                 * @param cloneMe the iterator to clone.
                 * @return a reference to this iterator.
                 */
                VectorIterator& operator = (const VectorIterator& cloneMe) =
                    default;

                /**
                 * Compares this with the given operator for equality.
                 *
                 * @param other the iterator to compare this with.
                 * @return \c true if the iterators point to the same
                 * element of the same normal surface list, or \c false
                 * if they do not.
                 */
                bool operator == (const VectorIterator& other) const;

                /**
                 * Compares this with the given operator for inequality.
                 *
                 * @param other the iterator to compare this with.
                 * @return \c false if the iterators point to the same
                 * element of the same normal surface list, or \c true
                 * if they do not.
                 */
                bool operator != (const VectorIterator& other) const;

                /**
                 * Returns the raw vector for the normal hypersurface that this
                 * iterator is currently pointing to.
                 *
                 * \pre This iterator is dereferenceable (in particular,
                 * it is not past-the-end).
                 *
                 * @return the corresponding normal hypersurface vector.
                 */
                const Vector<LargeInteger>& operator *() const;

                /**
                 * The preincrement operator.
                 *
                 * @return a reference to this iterator after the increment.
                 */
                VectorIterator& operator ++();

                /**
                 * The postincrement operator.
                 *
                 * @return a copy of this iterator before the
                 * increment took place.
                 */
                VectorIterator operator ++(int);

                /**
                 * The predecrement operator.
                 *
                 * @return a reference to this iterator after the decrement.
                 */
                VectorIterator& operator --();

                /**
                 * The postdecrement operator.
                 *
                 * @return a copy of this iterator before the
                 * decrement took place.
                 */
                VectorIterator operator --(int);

            private:
                /**
                 * Initialise a new vector iterator using an iterator for
                 * the internal list of normal hypersurfaces.
                 */
                VectorIterator(
                    const std::vector<NormalHypersurface>::const_iterator& i);

            friend class NormalHypersurfaces;
        };

    protected:
        /**
         * Creates an empty list of normal hypersurfaces with the given
         * parameters.
         *
         * @param coords the coordinate system to be used for filling
         * this list.
         * @param which indicates which normal hypersurfaces these will
         * represent within the underlying triangulation.
         * @param algorithm details of the enumeration algorithm that
         * will be used to fill this list.
         */
        NormalHypersurfaces(HyperCoords coords, HyperList which,
            HyperAlg algorithm);

        virtual Packet* internalClonePacket(Packet* parent) const override;
        virtual void writeXMLPacketData(std::ostream& out) const override;

    private:
        /**
         * Contains the code responsible for all normal hypersurface
         * enumeration, in a setting where the underlying coordinate system
         * is known at compile time.
         *
         * \tparam Coords an instance of the HyperInfo<> template class.
         */
        template <typename Coords>
        class Enumerator {
            private:
                NormalHypersurfaces* list_;
                    /**< The hypersurface list to be filled. */
                Triangulation<4>* triang_;
                    /**< The triangulation in which these hypersurfaces lie. */
                const MatrixInt& eqns_;
                    /**< The matching equations for the given triangulation in
                         the coordinate system corresponding to \a list_. */
                ProgressTracker* tracker_;
                    /**< The progress tracker through which progress is
                         reported and cancellation requests are accepted,
                         or \c null if no progress tracker is in use. */

            public:
                /**
                 * Creates a new functor with the given parameters.
                 *
                 * @param list the hypersurface list to be filled.
                 * @param triang the triangulation in which these
                 * hypersurfaces lie.
                 * @param eqns the matching equations for the given
                 * triangulation in the coordinate system corresponding to
                 * \a list.
                 * @param tracker the progress tracker to use for
                 * progress reporting and cancellation polling, or \c null if
                 * these capabilities are not required.
                 */
                Enumerator(NormalHypersurfaces* list, Triangulation<4>* triang,
                    const MatrixInt& eqns, ProgressTracker* tracker);

                /**
                 * Default move constructor.
                 */
                Enumerator(Enumerator&&) = default;

                /**
                 * Performs the real enumeration work.
                 *
                 * We assume here that neither list_->which_ nor
                 * list_->algorithm_ have been sanity-checked.
                 *
                 * This routine fills \a list_ with surfaces, and then once
                 * this is finished it inserts \a list_ into the packet
                 * tree as a child of \a triang_.
                 */
                void enumerate();

                // Make this class non-copyable.
                Enumerator(const Enumerator&) = delete;
                Enumerator& operator = (const Enumerator&) = delete;

            private:
                /**
                 * The enumeration code for enumerating vertex hypersurfaces.
                 * This is internal to enumerate().
                 *
                 * We assume that the flag set which_ is set correctly,
                 * and we do not alter it here.
                 * We make no assumptions about the state of algorithm_,
                 * and we set this during the course of this routine.
                 *
                 * This routine only fills \a list_ with hypersurfaces.
                 * It does not make any adjustments to the structure of
                 * the packet tree.
                 *
                 * If \a tracker_ is non-null, this routine will declare and
                 * work through a series of tracker stages whose
                 * combined weights sum to 1.  It will not, however,
                 * call ProgressTracker::setFinished().
                 */
                void fillVertex();

                /**
                 * The enumeration code for enumerating fundamental
                 * hypersurfaces.  This is internal to enumerate().
                 *
                 * We assume that the flag set which_ is set correctly,
                 * and we do not alter it here.
                 * We make no assumptions about the state of algorithm_,
                 * and we set this during the course of this routine.
                 *
                 * This routine only fills \a list_ with surfaces.
                 * It does not make any adjustments to the structure of
                 * the packet tree.
                 *
                 * If \a tracker_ is non-null, this routine declare and
                 * work through a series of tracker stages whose
                 * combined weights sum to 1.  It will not, however,
                 * call ProgressTracker::setFinished().
                 */
                void fillFundamental();

                /**
                 * The enumeration code for enumerating vertex surfaces
                 * using the double description method.
                 * This is internal to fillVertex().
                 *
                 * This routine assumes that \a algorithm_ has been set
                 * correctly, and does not alter it.
                 *
                 * If \a tracker_ is non-null, this routine assumes that
                 * an appropriate tracker stage has already been
                 * declared, and works through that stage only.
                 *
                 * \pre The underlying triangulation is non-empty.
                 */
                void fillVertexDD();

                /**
                 * The enumeration code for enumerating fundamental surfaces
                 * using the primal method.
                 * This is internal to fillFundamental().
                 *
                 * This routine assumes nothing about the state of the
                 * \a algorithm_ flag set, and sets it appropriately.
                 *
                 * If \a tracker_ is non-null, this routine will declare and
                 * work through a series of tracker stages whose
                 * combined weights sum to 1.  It will not, however,
                 * call ProgressTracker::setFinished().
                 *
                 * \pre The underlying triangulation is non-empty.
                 */
                void fillFundamentalPrimal();

                /**
                 * The enumeration code for enumerating fundamental surfaces
                 * using the dual method.
                 * This is internal to fillFundamental().
                 *
                 * This routine assumes nothing about the state of the
                 * \a algorithm_ flag set, and sets it appropriately.
                 *
                 * If \a tracker_ is non-null, this routine will declare and
                 * work through a series of tracker stages whose
                 * combined weights sum to 1.  It will not, however,
                 * call ProgressTracker::setFinished().
                 *
                 * \pre The underlying triangulation is non-empty.
                 */
                void fillFundamentalDual();
        };

    friend class XMLNormalHypersurfacesReader;
};

/**
 * Generates the set of normal hypersurface matching equations for the
 * given triangulation using the given coordinate system.
 *
 * Each equation will be represented as a row of the resulting matrix.
 * Each column of the matrix represents a coordinate in the given
 * coordinate system.
 *
 * For some coordinate systems, Regina may not be able to create matching
 * equations for all triangulations (any such coordinate systems will be
 * explicitly mentioned as such in the HyperCoords enum documentation).  If
 * Regina cannot create the matching equations as requested, this routine will
 * return no value instead.
 *
 * @param triangulation the triangulation upon which these matching equations
 * will be based.
 * @param coords the coordinate system to be used;
 * this must be one of the predefined coordinate system
 * constants in NormalHypersurfaces.
 * @return the resulting set of matching equations, or no value if
 * Regina is not able to construct them for the given combination of
 * triangulation and coordinate system.
 */
std::optional<MatrixInt> makeMatchingEquations(
    const Triangulation<4>& triangulation, HyperCoords coords);
/**
 * Generates the validity constraints representing the condition that
 * normal hypersurfaces be embedded.  The validity constraints will be expressed
 * relative to the given coordinate system.
 *
 * \ifacespython Not present.
 *
 * @param triangulation the triangulation upon which these validity constraints
 * will be based.
 * @param coords the coordinate system to be used;
 * this must be one of the predefined coordinate system
 * constants in NormalHypersurfaces.
 * @return the set of validity constraints.
 */
EnumConstraints makeEmbeddedConstraints(const Triangulation<4>& triangulation,
    HyperCoords coords);

/*@}*/

// Inline functions for NormalHypersurfaces

inline HyperCoords NormalHypersurfaces::coords() const {
    return coords_;
}

inline HyperList NormalHypersurfaces::which() const {
    return which_;
}

inline HyperAlg NormalHypersurfaces::algorithm() const {
    return algorithm_;
}

inline bool NormalHypersurfaces::isEmbeddedOnly() const {
    return which_.has(HS_EMBEDDED_ONLY);
}

inline size_t NormalHypersurfaces::size() const {
    return surfaces_.size();
}

inline auto NormalHypersurfaces::hypersurfaces() const {
    return ListView(surfaces_);
}

inline const NormalHypersurface& NormalHypersurfaces::hypersurface(
        size_t index) const {
    return surfaces_[index];
}

inline bool NormalHypersurfaces::dependsOnParent() const {
    return true;
}

template <typename Comparison>
inline void NormalHypersurfaces::sort(Comparison&& comp) {
    ChangeEventSpan span(this);
    std::stable_sort(surfaces_.begin(), surfaces_.end(), comp);
}

inline MatrixInt NormalHypersurfaces::recreateMatchingEquations() const {
    // Although makeMatchingEquations() returns a std::optional, we are
    // guaranteed in our scenario here that this will always contain a value.
    return *makeMatchingEquations(triangulation(), coords_);
}

inline NormalHypersurfaces::VectorIterator::VectorIterator() {
}

inline bool NormalHypersurfaces::VectorIterator::operator ==(
        const NormalHypersurfaces::VectorIterator& other) const {
    return (it_ == other.it_);
}

inline bool NormalHypersurfaces::VectorIterator::operator !=(
        const NormalHypersurfaces::VectorIterator& other) const {
    return (it_ != other.it_);
}

inline const Vector<LargeInteger>&
        NormalHypersurfaces::VectorIterator::operator *() const {
    return it_->vector().coords();
}

inline NormalHypersurfaces::VectorIterator&
        NormalHypersurfaces::VectorIterator::operator ++() {
    ++it_;
    return *this;
}

inline NormalHypersurfaces::VectorIterator
        NormalHypersurfaces::VectorIterator::operator ++(int) {
    return NormalHypersurfaces::VectorIterator(it_++);
}

inline NormalHypersurfaces::VectorIterator&
        NormalHypersurfaces::VectorIterator::operator --() {
    --it_;
    return *this;
}

inline NormalHypersurfaces::VectorIterator
        NormalHypersurfaces::VectorIterator::operator --(int) {
    return NormalHypersurfaces::VectorIterator(it_--);
}

inline NormalHypersurfaces::VectorIterator::VectorIterator(
        const std::vector<NormalHypersurface>::const_iterator& i) : it_(i) {
}

inline NormalHypersurfaces::VectorIterator
        NormalHypersurfaces::beginVectors() const {
    return VectorIterator(surfaces_.begin());
}

inline NormalHypersurfaces::VectorIterator
        NormalHypersurfaces::endVectors() const {
    return VectorIterator(surfaces_.end());
}

inline NormalHypersurfaces::NormalHypersurfaces(HyperCoords coords,
        HyperList which, HyperAlg algorithm) :
        coords_(coords), which_(which), algorithm_(algorithm) {
}

template <typename Coords>
inline NormalHypersurfaces::Enumerator<Coords>::Enumerator(
        NormalHypersurfaces* list, Triangulation<4>* triang,
        const MatrixInt& eqns, ProgressTracker* tracker) :
        list_(list), triang_(triang), eqns_(eqns),
        tracker_(tracker) {
}

} // namespace regina

#endif

