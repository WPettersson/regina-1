
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Test Suite                                                            *
 *                                                                        *
 *  Copyright (c) 1999-2006, Ben Burton                                   *
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

#include <sstream>
#include <cppunit/extensions/HelperMacros.h>
#include "maths/matrixops.h"
#include "maths/nmatrixint.h"
#include "testsuite/maths/testmaths.h"

using regina::NMatrixInt;

class MatrixOpsTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MatrixOpsTest);

    CPPUNIT_TEST(smithNormalForm);

    CPPUNIT_TEST_SUITE_END();

    private:
        NMatrixInt square3;
            /**< A simple 3-by-3 integer matrix given as an example in the
                 Smith normal form page on Wikipedia (September 2007).
                 The Smith normal form has diagonal (2, 6, 12). */

    public:
        MatrixOpsTest() : square3(3, 3) {
        }

        void setUp() {
            square3.entry(0, 0) = 2;
            square3.entry(0, 1) = 4;
            square3.entry(0, 2) = 4;
            square3.entry(1, 0) = -6;
            square3.entry(1, 1) = 6;
            square3.entry(1, 2) = 12;
            square3.entry(2, 0) = 10;
            square3.entry(2, 1) = -4;
            square3.entry(2, 2) = -16;
        }

        void tearDown() {
        }

        static void checkSNF3(const NMatrixInt& m, const char* name,
                long d0, long d1, long d2) {
            NMatrixInt ans(m);
            regina::smithNormalForm(ans);

            if (ans.rows() != m.rows() || ans.columns() != m.columns()) {
                std::ostringstream msg;
                msg << "Smith normal form for " << name
                    << " has incorrect dimensions.";
                CPPUNIT_FAIL(msg.str());
            }

            unsigned r, c;
            for (r = 0; r < m.rows(); ++r)
                for (c = 0; c < m.columns(); ++c)
                    if (r != c && ans.entry(r, c) != 0) {
                        std::ostringstream msg;
                        msg << "Smith normal form for " << name
                            << " has a non-zero off-diagonal entry at "
                            << "(" << r << ", " << c << ").";
                        CPPUNIT_FAIL(msg.str());
                    } else if (r == c && r > 0) {
                        if (ans.entry(r, c) % ans.entry(r - 1, c - 1) != 0) {
                            std::ostringstream msg;
                            msg << "Smith normal form for " << name
                                << " breaks the divisibility chain: "
                                << ans.entry(r, c) << " does not divide "
                                << ans.entry(r - 1, c - 1) << ".";
                            CPPUNIT_FAIL(msg.str());
                        }
                    }

            if (ans.entry(0, 0) != d0) {
                std::ostringstream msg;
                msg << "Smith normal form for " << name
                    << " has (0, 0) entry " << ans.entry(0, 0)
                    << " instead of " << d0 << ".";
                CPPUNIT_FAIL(msg.str());
            }
            if (ans.entry(1, 1) != d1) {
                std::ostringstream msg;
                msg << "Smith normal form for " << name
                    << " has (1, 1) entry " << ans.entry(1, 1)
                    << " instead of " << d1 << ".";
                CPPUNIT_FAIL(msg.str());
            }
            if (ans.entry(2, 2) != d2) {
                std::ostringstream msg;
                msg << "Smith normal form for " << name
                    << " has (2, 2) entry " << ans.entry(2, 2)
                    << " instead of " << d2 << ".";
                CPPUNIT_FAIL(msg.str());
            }
        }

        void smithNormalForm() {
            checkSNF3(square3, "simple 3x3 example", 2, 6, 12);
        }
};

void addMatrixOps(CppUnit::TextUi::TestRunner& runner) {
    runner.addTest(MatrixOpsTest::suite());
}

