
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
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

#include "regina-config.h"

#include "pythonmanager.h"

#include <klocale.h>
#include <kmessagebox.h>

#ifdef HAVE_BOOST_PYTHON

PythonManager::~PythonManager() {
    std::set<PythonConsole*>::iterator it, next;

    it = consoles.begin();
    next = it;
    while (it != consoles.end()) {
        // INV: next == it.

        // Step forwards before we actually destroy (*it), since the
        // destruction will deregister (*it) and invalidate the
        // iterator.
        next++;

        // The destruction process removes (*it) from the set, but other
        // iterators (i.e., next) are not invalidated.
        delete *it;

        it = next;
    }
}

void PythonManager::launchPythonConsole(QWidget* parent, regina::NPacket*,
        regina::NPacket*) {
    KMessageBox::sorry(parent, i18n("<qt>Python scripting has not yet "
        "been reimplemented for the KDE user interface.  This should "
        "be completed for version 4.0.<p>"
        "In the meantime, you can still use Python scripting with Regina "
        "through the command-line <b>regina-python</b> application.</qt>"));
}

void PythonManager::registerConsole(PythonConsole* console) {
    consoles.insert(console);
}

void PythonManager::deregisterConsole(PythonConsole* console) {
    consoles.erase(console);
}

#else

namespace {
    void scriptingDisabled(QWidget* parent) {
        KMessageBox::sorry(parent, i18n("<qt>Python scripting has been "
            "disabled in your particular build of Regina.  This is probably "
            "because no usable boost.python installation could be found.<p>"
            "Watch the output of <b>./configure</b> at compile time "
            "for a more detailed explanation of why this has happened.  "
            "Please write to <tt>%1</tt> if you would like further "
            "assistance.</qt>").arg(PACKAGE_BUGREPORT));
    }
}

PythonManager::~PythonManager() {
}

void PythonManager::launchPythonConsole(QWidget* parent, regina::NPacket*,
        regina::NPacket*) {
    scriptingDisabled(parent);
}

void PythonManager::registerConsole(PythonConsole*) {
}

void PythonManager::deregisterConsole(PythonConsole*) {
}

#endif

