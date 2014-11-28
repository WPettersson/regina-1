
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

#include <algorithm>
#include <cmath>
#include <complex>
#include "regina-config.h"
#include "enumerate/normaliz/cone.h"
#include "maths/approx.h"
#include "maths/ncyclotomic.h"
#include "maths/numbertheory.h"
#include "treewidth/ntreedecomposition.h"
#include "triangulation/ntriangulation.h"
#include "utilities/sequence.h"
#include <gmpxx.h>
#include <map>

// #define TV_BACKTRACK_DUMP_COLOURINGS
// #define TV_IGNORE_CACHE

#define TV_UNCOLOURED -1
#define TV_AGGREGATED -2

namespace regina {

namespace {
    template <bool exact>
    struct TuraevViroDetails;

    template <>
    struct TuraevViroDetails<true> {
        typedef NCyclotomic TVType;
        typedef NCyclotomic TVResult;
    };

    template <>
    struct TuraevViroDetails<false> {
        typedef std::complex<double> TVType;
        typedef double TVResult;
    };

    /**
     * Allows calculation of [n]! for arbitrary n.
     * Values are cached as they are calculated.
     */
    template <bool exact>
    class BracketFactorial {
        public:
            typedef typename TuraevViroDetails<exact>::TVType TVType;
            typedef typename TuraevViroDetails<exact>::TVResult TVResult;

        private:
            TVResult* bracket_;
                /**< The cached brackets [0], [1], ..., [r-1] . */
            TVResult* fact_;
                /**< The cached values [0]!, [1]!, ..., [r-1]! . */
            TVResult* inv_;
                /**< The cached inverses of the values stored in fact[]. */

        public:
            /**
             * Precalculate all values [0]!, ..., [r-1]!.
             *
             * Requires r >= 3.
             */
            BracketFactorial(unsigned long r, unsigned long whichRoot);

            /**
             * Clean up memory.
             */
            ~BracketFactorial() {
                delete[] bracket_;
                delete[] fact_;
                delete[] inv_;
            }

            /**
             * Returns the single value [index] (with no factorial symbol).
             * Requires index < r.
             */
            const TVResult& bracket(unsigned long index) const {
                return bracket_[index];
            }

            /**
             * Returns the value [index]!.
             * Requires index < r.
             */
            const TVResult& operator [] (unsigned long index) const {
                return fact_[index];
            }

            /**
             * Returns the value [index]! ^ -1.
             * Requires index < r.
             */
            const TVResult& inverse(unsigned long index) const {
                return inv_[index];
            }
    };

    template <>
    BracketFactorial<true>::BracketFactorial(
            unsigned long r, unsigned long whichRoot) :
            bracket_(new TVResult[r]),
            fact_(new TVResult[r]),
            inv_(new TVResult[r]) {
        bool halfField = (r % 2 != 0 && whichRoot % 2 == 0);
        bracket_[0].init(halfField ? r : 2 * r);
        bracket_[0][0] = 1;
        fact_[0] = fact_[1] = inv_[0] = inv_[1] =
            bracket_[1] = bracket_[0];

        TVResult q(halfField ? r : 2 * r);
        q[1] = 1;
        TVResult qInv(q);
        qInv.invert();

        TVResult base(q);
        base -= qInv;
        base.invert();

        TVResult qPow(q);
        TVResult qPowInv(qInv);

        TVResult tmp;
        for (unsigned long i = 2; i < r; i++) {
            qPow *= q;
            qPowInv *= qInv;

            bracket_[i] = qPow;
            bracket_[i] -= qPowInv;
            bracket_[i] *= base;
            fact_[i] = fact_[i - 1];
            fact_[i] *= bracket_[i];
            inv_[i] = inv_[i - 1];
            inv_[i] /= bracket_[i];
        }
    }

    template <>
    BracketFactorial<false>::BracketFactorial(
            unsigned long r, unsigned long whichRoot) :
            bracket_(new TVResult[r]),
            fact_(new TVResult[r]),
            inv_(new TVResult[r]) {
        TVResult angle = (M_PI * whichRoot) / r;
        bracket_[0] = bracket_[1] = fact_[0] = fact_[1] =
            inv_[0] = inv_[1] = 1.0;
        for (unsigned long i = 2; i < r; i++) {
            bracket_[i] = sin(angle * i) / sin(angle);
            fact_[i] = fact_[i - 1] * bracket_[i];
            inv_[i] = inv_[i - 1] / bracket_[i];
        }
    }

    /**
     * Represents the initial data as described in Section 7 of Turaev
     * and Viro's paper.
     */
    template <bool exact>
    struct InitialData {
        typedef typename TuraevViroDetails<exact>::TVType TVType;
        typedef typename TuraevViroDetails<exact>::TVResult TVResult;

        unsigned long r, whichRoot;
            /**< The Turaev-Viro parameters. */
        bool halfField;
        BracketFactorial<exact> fact;
            /**< The cached values [n]!. */
        TVType vertexContrib;
            /**< The vertex-based contribution to the Turaev-Viro invariant;
                 this is the inverse square of the distinguished value w. */

        InitialData(unsigned long newR, unsigned long newWhichRoot);

        static void negate(TVType& x);

        void initZero(TVType& x) const;
        void initOne(TVType& x) const;

        /**
         * Determines whether (i/2, j/2, k/2) is an admissible triple.
         */
        bool isAdmissible(unsigned long i, unsigned long j,
                unsigned long k) const {
            return ((i + j + k) % 2 == 0) &&
                (i <= j + k) && (j <= i + k) && (k <= i + j) &&
                (i + j + k <= 2 * (r - 2));
        }

        /**
         * Multiplies ans by the triangle-based contribution to the Turaev-Viro
         * invariant.  This corresponds to +/- Delta(i/2, j/2, k/2)^2.
         */
        void triContrib(unsigned long i, unsigned long j, unsigned long k,
                TVType& ans) const {
            // By admissibility, (i + j + k) is guaranteed to be even.
            ans *= fact[(i + j - k) / 2];
            ans *= fact[(j + k - i) / 2];
            ans *= fact[(k + i - j) / 2];
            ans *= fact.inverse((i + j + k + 2) / 2);
            if ((i + j + k) % 4 != 0)
                negate(ans);
        }

        /**
         * Multiplies ans by the edge-based contribution to the Turaev-Viro
         * invariant.  This corresponds to w(i/2)^2.
         */
        void edgeContrib(unsigned long i, TVType& ans) const {
            ans *= fact.bracket(i + 1);
            if (i % 2 != 0)
                negate(ans);
        }

        /**
         * Sets ansToOverwrite to the tetrahedron-based contribution to the
         * Turaev-Viro invariant.  This combines with the square roots of the
         * triangle-based contributions for the four tetrahedron faces to
         * give the symbol
         *
         *     | i/2 j/2 k/2 |
         *     | l/2 m/2 n/2 | .
         */
        void tetContrib(unsigned long i, unsigned long j,
                unsigned long k, unsigned long l, unsigned long m,
                unsigned long n, TVType& ansToOverwrite) const {
            ansToOverwrite = 0;

            unsigned long minZ = i + j + k;
            if (minZ < i + m + n)
                minZ = i + m + n;
            if (minZ < j + l + n)
                minZ = j + l + n;
            if (minZ < k + l + m)
                minZ = k + l + m;

            unsigned long maxZ = i + j + l + m;
            if (maxZ > i + k + l + n)
                maxZ = i + k + l + n;
            if (maxZ > j + k + m + n)
                maxZ = j + k + m + n;

            TVType term;
            for (unsigned long z = minZ; z <= maxZ; z++) {
                if (z % 2 != 0)
                    continue;

                // We are guaranteed that z / 2 is an integer.
                if (((z + 2) / 2) < r) {
                    term = fact[(z + 2) / 2];
                    term *= fact.inverse((z - i - j - k) / 2);
                    term *= fact.inverse((z - i - m - n) / 2);
                    term *= fact.inverse((z - j - l - n) / 2);
                    term *= fact.inverse((z - k - l - m) / 2);
                    term *= fact.inverse((i + j + l + m - z) / 2);
                    term *= fact.inverse((i + k + l + n - z) / 2);
                    term *= fact.inverse((j + k + m + n - z) / 2);

                    if (z % 4 == 0)
                        ansToOverwrite += term;
                    else
                        ansToOverwrite -= term;
                }
            }
        }

        /**
         * Multiplies ans by a single tetrahedron-based contribution
         * along with all triangle and edge contributions for which that
         * tetrahedron is responsible.  A tetrahedron is "responsible" for
         * a triangle or edge contribution iff it is the tetrahedron
         * referenced by getEmbedding(0) for that triangle or edge.
         *
         * The six arguments colour0, ..., colour5 refer to the colours
         * on tetrahedron edges 0, ..., 5 respectively.
         */
        void tetContrib(const NTetrahedron* tet,
                unsigned long colour0, unsigned long colour1,
                unsigned long colour2, unsigned long colour3,
                unsigned long colour4, unsigned long colour5,
                TVType& ans) const {
            TVType tmp(halfField ? r : 2 * r);
            tetContrib(colour0, colour1, colour3, colour5, colour4, colour2,
                tmp);
            ans *= tmp;

            int i;
            const NTriangle* triangle;
            const NEdge* edge;
            for (i = 0; i < 4; ++i) {
                triangle = tet->getTriangle(i);
                if (triangle->getEmbedding(0).getTetrahedron() == tet &&
                        triangle->getEmbedding(0).getTriangle() == i) {
                    switch (i) {
                        case 0:
                            triContrib(colour3, colour4, colour5, ans);
                            break;
                        case 1:
                            triContrib(colour1, colour2, colour5, ans);
                            break;
                        case 2:
                            triContrib(colour0, colour2, colour4, ans);
                            break;
                        case 3:
                            triContrib(colour0, colour1, colour3, ans);
                            break;
                    }
                }
            }
            for (i = 0; i < 6; ++i) {
                edge = tet->getEdge(i);
                if (edge->getEmbedding(0).getTetrahedron() == tet &&
                        edge->getEmbedding(0).getEdge() == i) {
                    switch (i) {
                        case 0: edgeContrib(colour0, ans); break;
                        case 1: edgeContrib(colour1, ans); break;
                        case 2: edgeContrib(colour2, ans); break;
                        case 3: edgeContrib(colour3, ans); break;
                        case 4: edgeContrib(colour4, ans); break;
                        case 5: edgeContrib(colour5, ans); break;
                    }
                }
            }
        }
    };

    template <>
    InitialData<true>::InitialData(
            unsigned long newR, unsigned long newWhichRoot) :
            r(newR),
            whichRoot(newWhichRoot),
            halfField(r % 2 != 0 && whichRoot % 2 == 0),
            fact(r, whichRoot) {
        // vertexContrib should be |q - q^-1|^2 / 2r.
        vertexContrib.init(halfField ? r : 2 * r);
        vertexContrib[1] = 1;
        TVResult inv(vertexContrib);
        inv.invert();

        vertexContrib -= inv;           // Pure imaginary.
        vertexContrib *= vertexContrib; // Gives -|..|^2
        vertexContrib.negate();         // Gives +|..|^2
        vertexContrib /= (2 * r);
    }

    template <>
    InitialData<false>::InitialData(
            unsigned long newR, unsigned long newWhichRoot) :
            r(newR),
            whichRoot(newWhichRoot),
            halfField(r % 2 != 0 && whichRoot % 2 == 0),
            fact(r, whichRoot) {
        double tmp = sin(M_PI * whichRoot / r);
        vertexContrib = 2.0 * tmp * tmp / r;
    }

    template <>
    inline void InitialData<true>::negate(InitialData<true>::TVType& x) {
        x.negate();
    }

    template <>
    inline void InitialData<false>::negate(InitialData<false>::TVType& x) {
        x = -x;
    }

    template <>
    inline void InitialData<true>::initZero(InitialData<true>::TVType& x)
            const {
        x.init(halfField ? r : 2 * r);
    }

    template <>
    inline void InitialData<false>::initZero(InitialData<false>::TVType& x)
            const {
        x = 0.0;
    }

    template <>
    inline void InitialData<true>::initOne(InitialData<true>::TVType& x)
            const {
        x.init(halfField ? r : 2 * r);
        x[0] = 1;
    }

    template <>
    inline void InitialData<false>::initOne(InitialData<false>::TVType& x)
            const {
        x = 1.0;
    }

    template <bool exact>
    typename InitialData<exact>::TVType turaevViroBacktrack(
            const NTriangulation& tri,
            const InitialData<exact>& init) {
        typedef typename InitialData<exact>::TVType TVType;

        // Run through all admissible colourings.
        TVType ans;
        init.initZero(ans);

        // Now hunt for colourings.
        unsigned long i;
        unsigned long nEdges = tri.getNumberOfEdges();
        unsigned long nTriangles = tri.getNumberOfTriangles();
        unsigned long* colour = new unsigned long[nEdges];

        std::fill(colour, colour + nEdges, 0);
        long curr = 0;
        TVType valColour(init.halfField ? init.r : 2 * init.r);
        bool admissible;
        std::deque<NEdgeEmbedding>::const_iterator embit;
        long index1, index2;
        const NTetrahedron* tet;
        while (curr >= 0) {
            // Have we found an admissible colouring?
            if (curr >= static_cast<long>(nEdges)) {
#ifdef TV_BACKTRACK_DUMP_COLOURINGS
                for (i = 0; i < nEdges; ++i) {
                    if (i > 0)
                        std::cout << ' ';
                    std::cout << colour[i];
                }
                std::cout << std::endl;
#endif
                // Increment ans appropriately.
                valColour = 1;
                for (i = 0; i < tri.getNumberOfTetrahedra(); i++) {
                    tet = tri.getTetrahedron(i);
                    init.tetContrib(tet,
                        colour[tet->getEdge(0)->index()],
                        colour[tet->getEdge(1)->index()],
                        colour[tet->getEdge(2)->index()],
                        colour[tet->getEdge(3)->index()],
                        colour[tet->getEdge(4)->index()],
                        colour[tet->getEdge(5)->index()],
                        valColour);
                }

                ans += valColour;

                // Step back down one level.
                curr--;
                if (curr >= 0)
                    colour[curr]++;
                continue;
            }

            // Have we run out of values to try at this level?
            if (colour[curr] > init.r - 2) {
                colour[curr] = 0;
                curr--;
                if (curr >= 0)
                    colour[curr]++;
                continue;
            }

            // Does the current value for colour[curr] preserve admissibility?
            admissible = true;
            const std::deque<NEdgeEmbedding>& embs(tri.getEdge(curr)->
                getEmbeddings());
            for (embit = embs.begin(); embit != embs.end(); embit++) {
                index1 = tri.edgeIndex((*embit).getTetrahedron()->getEdge(
                    NEdge::edgeNumber[(*embit).getVertices()[0]]
                    [(*embit).getVertices()[2]]));
                index2 = tri.edgeIndex((*embit).getTetrahedron()->getEdge(
                    NEdge::edgeNumber[(*embit).getVertices()[1]]
                    [(*embit).getVertices()[2]]));
                if (index1 <= curr && index2 <= curr) {
                    // We've decided upon colours for all three edges of
                    // this triangle containing the current edge.
                    if (! init.isAdmissible(colour[index1], colour[index2],
                            colour[curr])) {
                        admissible = false;
                        break;
                    }
                }
            }

            // Use the current value for colour[curr] if appropriate;
            // otherwise step forwards to the next value.
            if (admissible)
                curr++;
            else
                colour[curr]++;
        }

        delete[] colour;

        // Compute the vertex contributions separately, since these are
        // constant.
        for (i = 0; i < tri.getNumberOfVertices(); i++)
            ans *= init.vertexContrib;

        return ans;
    }

    template <bool exact>
    typename InitialData<exact>::TVType turaevViroTreewidth(
            const NTriangulation& tri,
            InitialData<exact>& init) {
        typedef typename InitialData<exact>::TVType TVType;

        NTreeDecomposition d(tri);
        d.compress();
        d.makeNice();

        int nEdges = tri.getNumberOfEdges();
        int nBags = d.size();
        const NTreeBag *bag, *child, *sibling;
        int i, j;
        int index;
        const NTetrahedron* tet;
        const NEdge* edge;

        // In the seenDegree[] array, an edge that has been seen in all
        // of its tetrahedra will be marked as seenDegree[i] = -1 (as
        // opposed to seenDegree[i] = tri.getEdge(i)->getDegree()).
        // This is simply to make such a condition easier to test.
        LightweightSequence<int>* seenDegree =
            new LightweightSequence<int>[nBags];

        for (bag = d.first(); bag; bag = bag->next()) {
            index = bag->index();
            seenDegree[index].init(nEdges);

            if (bag->isLeaf()) {
                std::fill(seenDegree[index].begin(),
                    seenDegree[index].end(), 0);
            } else if (bag->type() == NICE_INTRODUCE) {
                // Introduce bag.
                child = bag->children();
                std::copy(seenDegree[child->index()].begin(),
                    seenDegree[child->index()].end(),
                    seenDegree[index].begin());
            } else if (bag->type() == NICE_FORGET) {
                // Forget bag.
                child = bag->children();
                tet = tri.getTetrahedron(child->element(bag->subtype()));
                std::copy(seenDegree[child->index()].begin(),
                    seenDegree[child->index()].end(),
                    seenDegree[index].begin());
                for (i = 0; i < 6; ++i) {
                    edge = tet->getEdge(i);
                    ++seenDegree[index][edge->index()];
                    if (seenDegree[index][edge->index()] == edge->getDegree())
                        seenDegree[index][edge->index()] = -1;
                }
            } else {
                // Join bag.
                child = bag->children();
                sibling = child->sibling();
                for (i = 0; i < nEdges; ++i) {
                    seenDegree[index][i] = seenDegree[child->index()][i] +
                        seenDegree[sibling->index()][i];
                    if (seenDegree[index][i] == tri.getEdge(i)->getDegree())
                        seenDegree[index][i] = -1;
                }
            }
        }

        typedef std::map<LightweightSequence<int>*, TVType,
            LightweightSequence<int>::Less> SolnSet;

        SolnSet** partial = new SolnSet*[nBags];
        LightweightSequence<int>* seq;
        typename SolnSet::iterator it, it2;
        std::pair<typename SolnSet::iterator, bool> existingSoln;
        int e1, e2;
        int tetEdge[6];
        int colour[6];
        int level;
        bool ok;
        TVType val;

        // For each new tetrahedron that appears in a forget bag, we
        // colour its edges in the order 5,4,3,2,1,0.
        // This is so that we get triangles appearing as soon as possible
        // (edges 5-4-3 form a triangle, but edges 0-1-2 do not).
        //
        // To help in the decision making, choiceType[i] stores for
        // tetrahedron edge i:
        // * 0 if we must colour the edge here;
        // * -1 if the edge was already coloured in the child bag;
        // * x ∈ {5,...,1} if the edge was already coloured in this bag
        //   when it appeared as edge x of this same new tetrahedron.
        int choiceType[6];

        for (bag = d.first(); bag; bag = bag->next()) {
            index = bag->index();

            if (bag->isLeaf()) {
                // A single empty colouring.
                seq = new LightweightSequence<int>(nEdges);
                std::fill(seq->begin(), seq->end(), TV_UNCOLOURED);

                partial[index] = new SolnSet;
                init.initOne(val);
                partial[index]->insert(std::make_pair(seq, val));
            } else if (bag->type() == NICE_INTRODUCE) {
                // Introduce bag.
                child = bag->children();
                partial[index] = partial[child->index()];
                partial[child->index()] = 0;
            } else if (bag->type() == NICE_FORGET) {
                // Forget bag.
                child = bag->children();
                tet = tri.getTetrahedron(child->element(bag->subtype()));

                for (i = 5; i >= 0; --i) {
                    tetEdge[i] = tet->getEdge(i)->index();
                    if (seenDegree[child->index()][tetEdge[i]] > 0) {
                        // The child will have already coloured this edge.
                        choiceType[i] = -1;
                    } else {
                        choiceType[i] = 0;
                        for (j = 5; j > i; --j)
                            if (tetEdge[j] == tetEdge[i]) {
                                // We will have already coloured this edge
                                // because it reappears as a higher-numbered
                                // edge of this same tetrahedron.
                                choiceType[i] = j;
                                break;
                            }
                    }
                }

                partial[index] = new SolnSet;

                for (it = partial[child->index()]->begin();
                        it != partial[child->index()]->end(); ++it) {
                    for (i = 0; i < 6; ++i)
                        colour[i] = (choiceType[i] < 0 ?
                            (*(it->first))[tetEdge[i]] : -1);

                    level = 5;
                    while (level < 6) {
                        if (level < 0) {
                            // We have an admissible partial colouring.

                            // First, compute its (partial) weight:
                            val = it->second;
                            init.tetContrib(tet,
                                colour[0], colour[1], colour[2],
                                colour[3], colour[4], colour[5], val);

                            // Next, compute the sequence of colours
                            // that we will use as a lookup key.
                            // For any edges that never appear beyond
                            // this bag, we mark them for aggregation.
                            seq = new LightweightSequence<int>(nEdges);
                            for (i = 0; i < nEdges; ++i)
                                if (seenDegree[index][i] < 0)
                                    (*seq)[i] = TV_AGGREGATED;
                                else
                                    (*seq)[i] = (*it->first)[i];
                            for (i = 0; i < 6; ++i)
                                if (choiceType[i] == 0 &&
                                        (*seq)[tetEdge[i]] != TV_AGGREGATED)
                                    (*seq)[tetEdge[i]] = colour[i];

                            // Finally, insert the solution into the
                            // lookup table, aggregating with existing
                            // solutions if need be.
                            existingSoln = partial[index]->insert(
                                std::make_pair(seq, val));
                            if (! existingSoln.second) {
                                existingSoln.first->second += val;
                                delete seq;
                            }

                            ++level;
                            while (level < 6 && choiceType[level] != 0)
                                ++level;
                            continue;
                        }

                        if (choiceType[level] > 0)
                            colour[level] = colour[choiceType[level]];
                        else if (choiceType[level] == 0) {
                            if (colour[level] <
                                    static_cast<long>(init.r) - 2)
                                ++colour[level];
                            else {
                                // Out of choices at this level.
                                colour[level] = -1;
                                ++level;
                                while (level < 6 && choiceType[level] != 0)
                                    ++level;
                                continue;
                            }
                        }

                        ok = true;
                        if (level == 3 && ! init.isAdmissible(
                                colour[3], colour[4], colour[5]))
                            ok = false;
                        if (level == 1 && ! init.isAdmissible(
                                colour[1], colour[2], colour[5]))
                            ok = false;
                        if (level == 0 && ! init.isAdmissible(
                                colour[0], colour[2], colour[4]))
                            ok = false;
                        if (level == 0 && ! init.isAdmissible(
                                colour[0], colour[1], colour[3]))
                            ok = false;
                        if (! ok) {
                            // This colouring is inadmissible.
                            // If we have a choice for this edge then
                            // move on to the next colour.
                            // If the colour of this edge is forced then
                            // backtrack.
                            while (level < 6 && choiceType[level] != 0)
                                ++level;
                            continue;
                        }

                        --level;
                    }
                }

                for (it2 = partial[child->index()]->begin();
                        it2 != partial[child->index()]->end(); ++it2)
                    delete it2->first;
                delete partial[child->index()];
                partial[child->index()] = 0;
            } else {
                // Join bag.
                partial[index] = new SolnSet;

                child = bag->children();
                sibling = child->sibling();

                for (it = partial[child->index()]->begin();
                        it != partial[child->index()]->end(); ++it)
                    for (it2 = partial[sibling->index()]->begin();
                            it2 != partial[sibling->index()]->end(); ++it2) {
                        // Are the two solutions compatible?
                        ok = true;
                        for (i = 0; ok && i < nEdges; ++i)
                            if ((*it->first)[i] != TV_UNCOLOURED &&
                                    (*it2->first)[i] != TV_UNCOLOURED &&
                                    (*it->first)[i] != (*it2->first)[i])
                                ok = false;
                        if (! ok)
                            continue;

                        // Combine them and store the corresponding
                        // value, again aggregating if necessary.
                        val = it->second;
                        val *= it2->second;

                        seq = new LightweightSequence<int>(nEdges);
                        for (i = 0; i < nEdges; ++i)
                            if (seenDegree[index][i] < 0)
                                (*seq)[i] = TV_AGGREGATED;
                            else if (seenDegree[child->index()][i] > 0)
                                (*seq)[i] = (*it->first)[i];
                            else
                                (*seq)[i] = (*it2->first)[i];

                        existingSoln = partial[index]->insert(
                            std::make_pair(seq, val));
                        if (! existingSoln.second) {
                            existingSoln.first->second += val;
                            delete seq;
                        }
                    }

                for (it2 = partial[child->index()]->begin();
                        it2 != partial[child->index()]->end(); ++it2)
                    delete it2->first;
                delete partial[child->index()];
                partial[child->index()] = 0;

                for (it2 = partial[sibling->index()]->begin();
                        it2 != partial[sibling->index()]->end(); ++it2)
                    delete it2->first;
                delete partial[sibling->index()];
                partial[sibling->index()] = 0;
            }

#ifdef TV_BACKTRACK_DUMP_COLOURINGS
            std::cout << "Bag " << bag->index() << ":" << std::endl;
            for (it = partial[index]->begin(); it != partial[index]->end();
                    ++it)
                std::cout << *(it->first) << " -> "
                    << it->second << std::endl;
#endif
        }

        // The final bag contains no tetrahedra, and so there should be
        // only one colouring stored (in which all edge colours are aggregated).
        TVType ans = partial[nBags - 1]->begin()->second;
        for (i = 0; i < tri.getNumberOfVertices(); i++)
            ans *= init.vertexContrib;

        for (it = partial[nBags - 1]->begin(); it != partial[nBags - 1]->end();
                ++it)
            delete it->first;
        delete partial[nBags - 1];
        partial[nBags - 1] = 0;

        delete[] seenDegree;
        delete[] partial;

        return ans;
    }

    template <bool exact>
    typename InitialData<exact>::TVType turaevViroPolytope(
            const NTriangulation& tri,
            InitialData<exact>& init) {
        typedef typename InitialData<exact>::TVType TVType;

        std::vector<std::vector<mpz_class> > input;
        unsigned long nTri = tri.getNumberOfTriangles();

        NTriangulation::EdgeIterator eit;
        std::deque<NEdgeEmbedding>::const_iterator emb;
        const NTetrahedron* tet;
        NPerm4 p;
        unsigned long i;
        for (eit = tri.getEdges().begin(); eit != tri.getEdges().end(); ++eit) {
            for (emb = (*eit)->getEmbeddings().begin();
                    emb != (*eit)->getEmbeddings().end(); ++emb) {
                input.push_back(std::vector<mpz_class>());
                std::vector<mpz_class>& v(input.back());
                v.reserve(3 * nTri);

                for (i = 0; i < 3 * nTri; ++i)
                    v.push_back(long(0));

                tet = emb->getTetrahedron();
                p = emb->getVertices();

                ++v[3 * tet->getTriangle(p[2])->index() +
                    tet->getTriangleMapping(p[2]).preImageOf(p[0])];
                ++v[3 * tet->getTriangle(p[2])->index() +
                    tet->getTriangleMapping(p[2]).preImageOf(p[1])];
                --v[3 * tet->getTriangle(p[3])->index() +
                    tet->getTriangleMapping(p[3]).preImageOf(p[0])];
                --v[3 * tet->getTriangle(p[3])->index() +
                    tet->getTriangleMapping(p[3]).preImageOf(p[1])];
            }
        }

        libnormaliz::Cone<mpz_class> cone(input, libnormaliz::Type::equations);
        libnormaliz::ConeProperties wanted(
            libnormaliz::ConeProperty::HilbertBasis);
        cone.compute(wanted);

        if (! cone.isComputed(libnormaliz::ConeProperty::HilbertBasis)) {
            std::cerr << "ERROR: Hilbert basis not computed!" << std::endl;
            return TVType(init.halfField ? init.r : 2 * init.r);
        }
        const std::vector<std::vector<mpz_class> > basis =
            cone.getHilbertBasis();

        unsigned long j;
        for (i = 0; i < basis.size(); ++i) {
            for (j = 0; j < basis[i].size(); ++j)
                std::cout << basis[i][j] << ' ';
            std::cout << std::endl;
        }

        return TVType(init.halfField ? init.r : 2 * init.r);
    }
}

double NTriangulation::turaevViro(unsigned long r, unsigned long whichRoot,
        TuraevViroAlg alg) const {
    // Have we already calculated this invariant?
    std::pair<unsigned long, unsigned long> tvParams(r, whichRoot);
#ifndef TV_IGNORE_CACHE
    TuraevViroSet::const_iterator it = turaevViroCache_.find(tvParams);
    if (it != turaevViroCache_.end())
        return (*it).second;
#endif

    // Do some basic parameter checks.
    if (r < 3)
        return 0;
    if (whichRoot >= 2 * r)
        return 0;
    if (gcd(r, whichRoot) > 1)
        return 0;

    // Set up our initial data.
    InitialData<false> init(r, whichRoot);

    InitialData<false>::TVType ans;
    switch (alg) {
        case TV_DEFAULT:
        case TV_BACKTRACK:
            ans = turaevViroBacktrack(*this, init);
            break;
        case TV_TREEWIDTH:
            ans = turaevViroTreewidth(*this, init);
            break;
        case TV_POLYTOPE:
            // TODO: Cache the Hilbert basis.
            ans = turaevViroPolytope(*this, init);
            break;
    }

#if 0
    std::cout << "Result: " << ans << std::endl;
    std::cout << "Evaluation: " << ans.evaluate(
        init.halfField ? whichRoot / 2 : whichRoot) << std::endl;
    return 0.0; // TODO
#else
    if (isNonZero(ans.imag())) {
        // This should never happen, since the Turaev-Viro invariant is the
        // square of the modulus of the Witten invariant for sl_2.
        std::cerr <<
            "WARNING: The Turaev-Viro invariant has an imaginary component.\n"
            "         This should never happen.\n"
            "         Please report this (along with the 3-manifold that"
            "         was used) to " << PACKAGE_BUGREPORT << "." << std::endl;
    }
    turaevViroCache_[tvParams] = ans.real();
    return ans.real();
#endif
}

} // namespace regina

