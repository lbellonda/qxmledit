/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#include "xmlEdit.h"
#include "extractresults.h"
#include "utils.h"

ExtractResults::ExtractResults(QObject *parent) : QObject(parent)
{
    init();
}

ExtractResults::~ExtractResults()
{
}

void ExtractResults::init()
{
    _isError = false ;
    _isAborted = false ;
    _numFragments = 0 ;
    _numDocumentsCreated = 0 ;
    _numFoldersCreated = 0;
    // this is the map that permits us to read a single document from the file using a seek operation
    _startDocumentCharacterOffset.clear();
    _endDocumentCharacterOffset.clear();
    _encoding = "utf-8" ;
    Utils::TODO_THIS_RELEASE("decidere se si o no");
    _optimizeSpeed = false ;
}

uint ExtractResults::numFragments()
{
    return _numFragments ;
}

void ExtractResults::incrementFragment(const quint64 characterOffset)
{
    _numFragments ++ ;
    _startDocumentCharacterOffset.insert(_numFragments, characterOffset);
}

void ExtractResults::endFragment(const qint64 characterOffset)
{
    _endDocumentCharacterOffset.insert(_numFragments, characterOffset);
}

bool ExtractResults::isError()
{
    return _isError ;
}

void ExtractResults::setError(const bool value)
{
    _isError = value ;
}

bool ExtractResults::isAborted()
{
    return _isAborted ;
}

void ExtractResults::setAborted(const bool value)
{
    _isAborted = value ;
}

//-----------------------------------------------------------------------------------------------

bool ExtractResults::readWaste(QTextStream &textStream, const int times, const int charsToSkip)
{
    FORINT(i, times) {
        QString waste = textStream.read(charsToSkip);
        if(waste.length() != charsToSkip) {
            return false;
        }
    }
    return true;
}

void ExtractResults::loadFragment(const int page, StringOperationResult &result)
{
    const qint64 C100_000 = 100000 ;
    const qint64 C10_000 = 10000 ;
    bool isError = true ;
    if(_startDocumentCharacterOffset.keys().contains(page)) {
        isError = false ;
        QFile file(_fileName);
        if(file.open(QIODevice::ReadOnly)) {
            QTextStream textStream(&file);
            textStream.setCodec(_encoding.toLatin1().data());
            // read the start lines of the elements
            qint64 startCharacter = _startDocumentCharacterOffset.value(page);
            qint64 endCharacter ;
            if(_endDocumentCharacterOffset.contains(page)) {
                endCharacter = _endDocumentCharacterOffset.value(page);
            } else {
                endCharacter = startCharacter ;
            }
            qint64 lenToRead = endCharacter - startCharacter + 1;
            QString trData;

            qint64 resto = 0 ;
            qint64 hundredThousands = startCharacter / C100_000;
            resto = startCharacter % C100_000;
            qint64 tenThousands = resto / C10_000;
            resto = resto % C10_000;
            qint64 thousands = resto / 1000;
            resto = resto % 1000;
            qint64 hundreds = resto / 100;
            resto = resto % 100;
            if(!readWaste(textStream, hundredThousands, C100_000)) {
                isError = true;
            }
            if(!readWaste(textStream, tenThousands, C10_000)) {
                isError = true;
            }
            if(!readWaste(textStream, thousands, 1000)) {
                isError = true;
            }
            if(!readWaste(textStream, hundreds, 100)) {
                isError = true;
            }
            if(resto > 0) {
                if(!readWaste(textStream, 1, resto)) {
                    isError = true;
                }
            }
            QString document = textStream.read(lenToRead);
            if(document.length() != lenToRead) {
                isError = false;
            }
            trData = document ;
            if(textStream.status() != QTextStream::Ok) {
                isError = true ;
            }

            if(file.error() != QFile::NoError) {
                isError = true ;
            }
            if(!isError) {
                trData = trData.trimmed();
                result.setResult(trData);
                // printf("xml is:'%s'\n", trData.toAscii().data());
            }
            file.close();
        }
    }
    if(isError) {
        result.setError(true);
    }
}
