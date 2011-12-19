
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Qt User Interface                                                    *
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

/*! \file reginamanager.h
 *  \brief Provides a window manager for Regina.
 */

#ifndef __REGINAMANAGER_H_
#define __REGINAMANAGER_H_

#include "regina-config.h"
#include "reginamain.h"

#include <QApplication>


class ReginaManager : public QApplication {

    public:
        ReginaManager(int argc, char** argv);
        
        ReginaMain* newWindow();
        ReginaMain* newWindow(const QString url);

        void onClose(ReginaMain *child);

    private:
        QList<ReginaMain *> children;

};

#endif /* __REGINAMANAGER_H_ */

