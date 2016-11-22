/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "definition.h"
#include "definition_p.h"
#include "definitionref_p.h"

#include "context_p.h"
#include "format.h"
#include "format_p.h"
#include "repository_p.h"
#include "rule_p.h"
#include "ksyntaxhighlighting_version.h"
#include "xml_p.h"

#include <QFile>
#include <QXmlStreamReader>

#include <algorithm>
#include <cassert>
#include <iostream>

#include "utilities/stringutils.h"
#include "utilities/xmlutils.h"

using namespace KSyntaxHighlighting;

KeywordList nullList;

DefinitionData::DefinitionData() :
    repo(nullptr),
    delimiters(QStringLiteral("\t !%&()*+,-./:;<=>?[\\]^{|}~")), // must be sorted!
    caseSensitive(Qt::CaseSensitive),
    version(0.0f)
{
}

DefinitionData::~DefinitionData()
{
    qDeleteAll(contexts);
}

DefinitionData* DefinitionData::get(const Definition &def)
{
    return def.d.get();
}

Definition::Definition() :
    d(new DefinitionData)
{
}

Definition::Definition(const Definition &other) :
    d(other.d)
{
    d->q = *this;
}

Definition::Definition(const std::shared_ptr<DefinitionData> &dd) :
    d(dd)
{
}

Definition::~Definition()
{
}

Definition& Definition::operator=(const Definition &rhs)
{
    d = rhs.d;
    return *this;
}

bool Definition::operator==(const Definition &other) const
{
    return d->fileName == other.d->fileName;
}

bool Definition::operator!=(const Definition& other) const
{
    return d->fileName != other.d->fileName;
}

bool Definition::isValid() const
{
    return d->repo && !d->fileName.empty() && !d->name.empty();
}

const std::string& Definition::filePath() const
{
    return d->fileName;
}

const std::string& Definition::name() const
{
    return d->name;
}

const std::string& Definition::section() const
{
    return d->section;
}

int Definition::version() const
{
    return d->version;
}

const std::string& Definition::style() const
{
    return d->style;
}

const std::string& Definition::indenter() const
{
    return d->indenter;
}

const std::string& Definition::author() const
{
    return d->author;
}

const std::string& Definition::license() const
{
    return d->license;
}

Context* DefinitionData::initialContext() const
{
    assert(!contexts.empty());
    return contexts.front();
}

Context* DefinitionData::contextByName(const std::string& name) const
{
    for (auto context : contexts) {
        if (context->name() == name)
            return context;
    }
    return nullptr;
}

const KeywordList& DefinitionData::keywordList(const std::string& name) const
{
    const auto it = keywordLists.find(name);
    if (it != keywordLists.end())
        return *(it->second);

    return nullList;
}

bool DefinitionData::isDelimiter(QChar c) const
{
    return std::binary_search(delimiters.constBegin(), delimiters.constEnd(), c);
}

Format DefinitionData::formatByName(const std::string& name) const
{
    const auto it = formats.find(name);
    if (it != formats.end())
        return it->second;

    return Format();
}

bool DefinitionData::isLoaded() const
{
    return !contexts.empty();
}

bool DefinitionData::load()
{
    if (isLoaded())
        return true;

    assert(!fileName.empty());
    QFile file(QFile::decodeName(fileName.c_str()));
    if (!file.open(QFile::ReadOnly))
        return false;

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        const auto token = reader.readNext();
        if (token != QXmlStreamReader::StartElement)
            continue;

        if (reader.name() == QLatin1String("highlighting"))
            loadHighlighting(reader);

        else if (reader.name() == QLatin1String("general"))
            loadGeneral(reader);
    }

    for (auto it = keywordLists.begin(); it != keywordLists.end(); ++it)
        it->second->setCaseSensitivity(caseSensitive);

    for (auto context : contexts) {
        context->resolveContexts();
        context->resolveIncludes();
    }

    assert(std::is_sorted(delimiters.constBegin(), delimiters.constEnd()));
    return true;
}

void DefinitionData::clear()
{
    // keep only name and repo, so we can re-lookup to make references persist over repo reloads
    keywordLists.clear();
    qDeleteAll(contexts);
    contexts.clear();
    formats.clear();

    fileName.clear();
    section.clear();
    style.clear();
    indenter.clear();
    author.clear();
    license.clear();
    delimiters = QStringLiteral("\t !%&()*+,-./:;<=>?[\\]^{|}~"); // must be sorted!
    caseSensitive = Qt::CaseSensitive,
    version = 0.0f;
}

bool DefinitionData::loadMetaData(const std::string& definitionFileName)
{
    fileName = definitionFileName;

    xmlTextReaderPtr reader = xmlNewTextReaderFilename(fileName.c_str());
    if (! reader)
        return false; // could not open

    bool success = false;
    while (xmlTextReaderRead(reader) == 1) {
        if (xmlTextReaderNodeType(reader) == 1 /* start element */) {
            if (regina::xml::xmlString(xmlTextReaderName(reader)) == "language") {
                success = loadLanguage(reader);
                break;
            }
        }
    }
    // Either could not parse, or end of file.
    xmlFreeTextReader(reader);
    return success;
}

bool DefinitionData::loadLanguage(xmlTextReaderPtr reader)
{
    // These attributes default to empty strings.
    name = regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"name"));
    section = regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"section"));
    style = regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"style"));
    indenter = regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"indenter"));
    author = regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"author"));
    license = regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"license"));

    // Since valueOf() does not have defaults, we must explicitly test validity.
    double ver;
    if (regina::valueOf(regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"version")), ver))
        version = ver;

    bool cs;
    if (regina::valueOf(regina::xml::xmlString(xmlTextReaderGetAttribute(reader, (const xmlChar*)"casesensitive")), cs))
        caseSensitive = (cs ? Qt::CaseSensitive : Qt::CaseInsensitive);

    return true;
}

void DefinitionData::loadHighlighting(QXmlStreamReader& reader)
{
    assert(reader.name() == QLatin1String("highlighting"));
    assert(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("list")) {
                    KeywordList* keywords = new KeywordList;
                    keywords->load(reader);
                    keywordLists.insert(std::make_pair(keywords->name(), std::shared_ptr<KeywordList>(keywords)));
                } else if (reader.name() == QLatin1String("contexts")) {
                    loadContexts(reader);
                    reader.readNext();
                } else if (reader.name() == QLatin1String("itemDatas")) {
                    loadItemData(reader);
                } else {
                    reader.readNext();
                }
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void DefinitionData::loadContexts(QXmlStreamReader& reader)
{
    assert(reader.name() == QLatin1String("contexts"));
    assert(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("context")) {
                    auto context = new Context;
                    context->setDefinition(q);
                    context->load(reader);
                    contexts.push_back(context);
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void DefinitionData::loadItemData(QXmlStreamReader& reader)
{
    assert(reader.name() == QLatin1String("itemDatas"));
    assert(reader.tokenType() == QXmlStreamReader::StartElement);

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (reader.name() == QLatin1String("itemData")) {
                    Format f;
                    auto formatData = FormatPrivate::get(f);
                    formatData->definition = q;
                    formatData->load(reader);
                    formatData->id = RepositoryPrivate::get(repo)->nextFormatId();
                    formats.insert(std::make_pair(f.name(), f));
                    reader.readNext();
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                return;
            default:
                reader.readNext();
                break;
        }
    }
}

void DefinitionData::loadGeneral(QXmlStreamReader& reader)
{
    assert(reader.name() == QLatin1String("general"));
    assert(reader.tokenType() == QXmlStreamReader::StartElement);
    reader.readNext();

    // reference counter to count XML child elements, to not return too early
    int elementRefCounter = 1;

    while (!reader.atEnd()) {
        switch (reader.tokenType()) {
            case QXmlStreamReader::StartElement:
                ++elementRefCounter;

                if (reader.name() == QLatin1String("keywords")) {
                    if (reader.attributes().hasAttribute(QStringLiteral("casesensitive")))
                        caseSensitive = Xml::attrToBool(reader.attributes().value(QStringLiteral("casesensitive"))) ? Qt::CaseSensitive : Qt::CaseInsensitive;
                    delimiters += reader.attributes().value(QStringLiteral("additionalDeliminator"));
                    std::sort(delimiters.begin(), delimiters.end());
                    auto it = std::unique(delimiters.begin(), delimiters.end());
                    delimiters.truncate(std::distance(delimiters.begin(), it));
                    foreach (const auto c, reader.attributes().value(QLatin1String("weakDeliminator")))
                        delimiters.remove(c);
                } else {
                    reader.skipCurrentElement();
                }
                reader.readNext();
                break;
            case QXmlStreamReader::EndElement:
                --elementRefCounter;
                if (elementRefCounter == 0)
                    return;
            default:
                reader.readNext();
                break;
        }
    }
}

DefinitionRef::DefinitionRef()
{
}

DefinitionRef::DefinitionRef(const Definition &def) :
    d(def.d)
{
}

DefinitionRef::~DefinitionRef()
{
}

DefinitionRef& DefinitionRef::operator=(const Definition &def)
{
    d = def.d;
    return *this;
}

Definition DefinitionRef::definition() const
{
    if (!d.expired())
        return Definition(d.lock());
    return Definition();
}
