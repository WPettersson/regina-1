
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

/*! \file snappea/xmlsnappeareader.h
 *  \brief Deals with parsing XML data for SnapPea triangulations.
 */

#ifndef __REGINA_XMLSNAPPEAREADER_H
#ifndef __DOXYGEN
#define __REGINA_XMLSNAPPEAREADER_H
#endif

#include "regina-core.h"
#include "file/xml/xmlpacketreader.h"
#include "snappea/snappeatriangulation.h"

namespace regina {

/**
 * An XML packet reader that reads a single SnapPea triangulation.
 *
 * \ifacespython Not present.
 */
class XMLSnapPeaReader : public XMLPacketReader {
    private:
        SnapPeaTriangulation* snappea_;
            /**< The SnapPea triangulation currently being read. */

    public:
        /**
         * Creates a new SnapPea triangulation reader.
         *
         * All parameters are the same as for the parent class XMLPacketReader.
         */
        XMLSnapPeaReader(XMLTreeResolver& resolver, Packet* parent,
            bool anon, std::string label, std::string id);

        virtual Packet* packetToCommit() override;
        virtual void initialChars(const std::string& chars) override;
};

/**
 * An XML packet reader that reads a single SnapPea triangulation using the
 * older second-generation file format.
 *
 * \ifacespython Not present.
 */
class XMLLegacySnapPeaReader : public XMLPacketReader {
    private:
        SnapPeaTriangulation* snappea_;
            /**< The SnapPea triangulation currently being read. */

    public:
        /**
         * Creates a new SnapPea triangulation reader.
         *
         * All parameters are the same as for the parent class XMLPacketReader.
         */
        XMLLegacySnapPeaReader(XMLTreeResolver& resolver, Packet* parent,
            bool anon, std::string label, std::string id);

        virtual Packet* packetToCommit() override;
        virtual XMLElementReader* startContentSubElement(
            const std::string& subTagName,
            const regina::xml::XMLPropertyDict& subTagProps) override;
        virtual void endContentSubElement(const std::string& subTagName,
            XMLElementReader* subReader) override;
};

// Inline functions for XMLSnapPeaReader

inline XMLSnapPeaReader::XMLSnapPeaReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        snappea_(new SnapPeaTriangulation()) {
}

inline Packet* XMLSnapPeaReader::packetToCommit() {
    return snappea_;
}

// Inline functions for XMLLegacySnapPeaReader

inline XMLLegacySnapPeaReader::XMLLegacySnapPeaReader(
        XMLTreeResolver& res, Packet* parent, bool anon,
        std::string label, std::string id) :
        XMLPacketReader(res, parent, anon, std::move(label), std::move(id)),
        snappea_(new SnapPeaTriangulation()) {
}

inline Packet* XMLLegacySnapPeaReader::packetToCommit() {
    return snappea_;
}

inline XMLElementReader* XMLLegacySnapPeaReader::startContentSubElement(
        const std::string& subTagName, const regina::xml::XMLPropertyDict&) {
    if (subTagName == "snappea")
        return new XMLCharsReader();
    else
        return new XMLElementReader();
}

} // namespace regina

#endif

