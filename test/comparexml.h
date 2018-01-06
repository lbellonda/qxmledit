/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/

#ifndef COMPAREXML_H
#define COMPAREXML_H
#include <QDomDocument>
#include "testbase.h"

class Regola;

class CompareXML : public TestBase
{
    bool _showMessages;
    QList<int> _path;
    QString _doc1String ;
    QString _doc2String ;


    bool comparaFigli(QDomNode &node1, QDomNode &node2);
    bool compareAttributes(QDomElement element1, QDomElement element2);
    void debug(const QString &msg);
    QString getStringData(const QString &str);
    void compareDifferentStrings(const QString &str1, const QString &str2);
    void dumpTwoNodes(QDomNodeList &childNodes1, QDomNodeList &childNodes2);
    void dumpNodes(QDomNodeList &childNodes);
    bool errorFlag(QDomNode errorNode, const QString &msg);
    void reset();

public:
    CompareXML();
    bool comparePlainDocuments();
    bool compareMixedDocuments();
    bool compareMixedDocumentsFalse();
    bool comparePlainDocumentsFalse();

    bool compareDomDocuments(QDomDocument &doc1, QDomDocument &doc2);
    bool loadFileIntoDocument(QIODevice *ioDevice, QDomDocument &document);
    bool loadFileIntoDocument(const QString &filePath, QDomDocument &document);
    bool loadStringIntoDocument(const QString &string, QDomDocument &document);
    bool compareFiles(const QString &file1, const QString &file2);
    bool compareFiles(const QString &file1, QIODevice *file2);
    bool compareAFileWithRegola(const QString &file, Regola *regola);
    bool compareFileWithRegola(Regola *regola, const QString &file);
    bool compareFileWithRegola(Regola *regola, QIODevice *file);
    bool compareBufferWithFile(QIODevice *regola, const QString &file);
    bool compareStringWithFile(const QString &text, const QString &file);
    bool compareRegolaWithRegola(Regola *regola1, Regola *regola2);
    bool compareXMLAsStringAsUtf8(const QString &s1, const QString & s2);

    QString doc1String() ;
    QString doc2String() ;
    void dumpErrorCause();

};

#endif // COMPAREXML_H

