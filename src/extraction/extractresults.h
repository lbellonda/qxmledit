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


#ifndef EXTRACTRESULTS_H
#define EXTRACTRESULTS_H

#include <QHash>
#include "operationresult.h"
#include <QFile>
#include "libQXmlEdit_global.h"


class LIBQXMLEDITSHARED_EXPORT ExtractResults : public QObject
{
    Q_OBJECT
public:
    volatile bool _isError ;
    volatile bool _isAborted ;
    QString _fileName;
    unsigned int _numFragments;
    unsigned int _numDocumentsCreated;
    unsigned int _numFoldersCreated;
    QString _currentSubFolder;
    // this is the map that permits us to read a single document from the file using a seek operation
    QHash<int, qint64> _startDocumentLine;
    QHash<int, qint64> _endDocumentLine;
    //------------------------------------
    QHash<int, qint64> _linePos;
    QHash<int, qint64> _startDocumentColumn;
    QHash<int, qint64> _endDocumentColumn;
    //------------------------------------
    bool _optimizeSpeed;


    void init();
    QString trackForTag(QFile &file, const int startLine, const bool isLookingForOpenTag, bool &isError);

public:
    explicit ExtractResults(QObject *parent = NULL);
    ~ExtractResults();

    void incrementFragment(const qint64 line, const qint64 column);
    void endFragment(const qint64 line, const qint64 column);
    // inline
    uint currentFragment() {
        return _numFragments;
    }
    uint currentFolderCount() {
        return _numFoldersCreated;
    }
    int numFragments();

    bool isError();
    bool isAborted();
    void setError(const bool value);
    void setAborted(const bool value);

    void loadFragment(const int page, StringOperationResult &result);

};

#endif // EXTRACTRESULTS_H
