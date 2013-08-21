
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
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

/*! \file nsurfacefiltercreator.h
 *  \brief Allows the creation of normal surface filters.
 */

#ifndef __NSURFACEFILTERCREATOR_H
#define __NSURFACEFILTERCREATOR_H

#include "../packetcreator.h"

class QButtonGroup;

/**
 * An interface for creating normal surface filters.
 */
class NSurfaceFilterCreator : public PacketCreator {
    private:
        /**
         * Internal components.
         */
        QButtonGroup* group;
        QWidget* ui;

    public:
        /**
         * Constructor and destructor.
         */
        NSurfaceFilterCreator();
        ~NSurfaceFilterCreator();

        /**
         * PacketCreator overrides.
         */
        QWidget* getInterface();
        regina::NPacket* createPacket(regina::NPacket* parentPacket,
            QWidget* parentWidget);
};

#endif
