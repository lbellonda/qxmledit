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
    _startDocumentLine.clear();
    _endDocumentLine.clear();
    _linePos.clear();
    _startDocumentColumn.clear();
    _endDocumentColumn.clear();
    _optimizeSpeed = false ;
}

int ExtractResults::numFragments()
{
    return _numFragments ;
}

void ExtractResults::incrementFragment(const qint64 line, const qint64 column)
{
    _numFragments ++ ;
    _startDocumentLine.insert(_numFragments, line);
    _startDocumentColumn.insert(_numFragments, column);
}

void ExtractResults::endFragment(const qint64 line, const qint64 column)
{
    _endDocumentLine.insert(_numFragments, line);
    _endDocumentColumn.insert(_numFragments, column);
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

void ExtractResults::loadFragment(const int page, StringOperationResult &result)
{
    bool isError = true ;
    if(_startDocumentLine.keys().contains(page)) {
        isError = false ;
        QFile file(_fileName);
        if(file.open(QIODevice::ReadOnly)) {

            // read the start lines of the elements
            qint64 fileLineStart = _startDocumentLine.value(page);
            qint64 fileLineEnd = _endDocumentLine.value(page);
            qint64 fileLineStartPos = _linePos.value(fileLineStart);
            qint64 fileLineEndPos = _linePos.value(fileLineEnd);
            qint64 startCharColumn = _startDocumentColumn.value(page);
            qint64 endCharColumn = _endDocumentColumn.value(page);

            QString trData;
            qint64 nextLinePos = _linePos.value(fileLineEnd + 1);

            bool lookForStart = false;
            bool lookForEnd = false;
            if(fileLineStart == fileLineEnd) {   // same line
                if(!file.seek(fileLineStartPos)) {
                    isError = true ;
                }
                QByteArray data = file.readLine(nextLinePos - fileLineStartPos + 10);
                trData = QString::fromUtf8(data);
                int indexStart = trData.lastIndexOf('<', startCharColumn - 1);
                int indexEnd = trData.indexOf('>', endCharColumn - 1);
                if((indexStart >= 0) && (indexEnd >= 0)) {
                    trData = trData.mid(indexStart, indexEnd - indexStart + 1);
                } else {
                    if(indexStart < 0) {
                        lookForStart = true ;
                        startCharColumn = 0;
                    }
                    if(indexEnd < 0) {
                        lookForEnd = true ;
                        endCharColumn = trData.length() - 1 ;
                    }
                    trData = trData.mid(startCharColumn, endCharColumn - startCharColumn + 1);
                }
            } else {
                if(!file.seek(fileLineStartPos)) {
                    isError = true ;
                }
                // read up to last line excluded
                QByteArray data = file.read(fileLineEndPos - fileLineStartPos);
                trData = QString::fromUtf8(data);
                int indexStart = trData.lastIndexOf('<', startCharColumn - 1);
                if(indexStart < 0) {
                    lookForStart = true ;
                    trData = trData;
                } else {
                    trData = trData.mid(indexStart);
                }

                // read the last line
                file.seek(fileLineEndPos);
                qint64 charactersToRead = nextLinePos - fileLineEndPos ;
                if(charactersToRead < 0) {
                    // This condition happens when the tag to look for is the last one (root).
                    // there is no such next line in this case.
                    // Using a token.
                    charactersToRead = 1024;
                }
                QByteArray dataLast = file.readLine(charactersToRead);
                QString trDataEnd = QString::fromUtf8(dataLast);
                int indexEnd = trDataEnd.indexOf('>', endCharColumn - 1);

                if(indexEnd < 0) {
                    lookForEnd = true ;
                    // trData is the whole string
                } else {
                    QString endData = trDataEnd.mid(0, indexEnd + 1) ;
                    trData.append(endData);
                }
            }

            // look for outliers.
            if(lookForStart) {
                trData.prepend(trackForTag(file, fileLineStart, true, isError));
            }
            if(lookForEnd) {
                trData.append(trackForTag(file, fileLineEnd, false, isError));
            }

            if(file.error() != QFile::NoError) {
                isError = true ;
            } else {
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

QString ExtractResults::trackForTag(QFile &file, const int startLine, const bool isLookingForOpenTag, bool &isError)
{
    QString finalString ;
    int line = isLookingForOpenTag ? startLine - 1 : startLine + 1 ;
    int iteration = 0;
    QChar charToLookFor = isLookingForOpenTag ? '<' : '>' ;

    do {
        if(!(_linePos.contains(line) && _linePos.contains(line + 1))) {
            // no way, let return whatever we have
            break;
        }

        qint64 fileLineStartPos = _linePos.value(line);
        qint64 nextLinePos = _linePos.value(line + 1);
        if(!file.seek(fileLineStartPos)) {
            isError = true ;
        }
        QByteArray data = file.readLine(nextLinePos - fileLineStartPos);
        QString trData = QString::fromUtf8(data);
        int index ;
        if(isLookingForOpenTag) {
            index = trData.lastIndexOf(charToLookFor);
        } else {
            index = trData.indexOf(charToLookFor);
        }
        if(index >= 0) {
            if(isLookingForOpenTag) {
                trData = trData.mid(index);
                finalString.prepend(trData);
            } else {
                trData = trData.left(index + 1);
                finalString.append(trData);
            }
            return finalString;
        } else {
            if(isLookingForOpenTag) {
                finalString.prepend(trData);
                line -- ;
            } else {
                finalString.append(trData);
                line ++;
            }
        }
        iteration++;
    } while(iteration < Utils::ReasonableIterationCount);

    if(file.error() != QFile::NoError) {
        isError = true ;
    }
    return finalString;
}
