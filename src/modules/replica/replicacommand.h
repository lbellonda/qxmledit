/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef REPLICACOMMAND_H
#define REPLICACOMMAND_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT ReplicaCommand
{
public:
    enum EFormat {
        NumberUnpadded,
        NumberPadded,
        AlphaUnpadded,
        AlphaPadded,
    };

    enum EErrorCode {
        NoError,
        AttributeNameError,
        StartNumberError,
        IllegalFormatError
    };

private:
    bool _recursive;
    QString _attributeName;
    bool _overwrite;
    int _startNumber;
    bool _atEnd;
    QString _separator;
    EFormat _format;
    bool _replace;
public:

    ReplicaCommand();
    ~ReplicaCommand();
    bool recursive() const;
    void setRecursive(bool recursive);
    QString attributeName() const;
    void setAttributeName(const QString &attributeName);
    bool overwrite() const;
    void setOverwrite(bool overwrite);
    int startNumber() const;
    void setStartNumber(int startNumber);
    bool atEnd() const;
    void setAtEnd(bool atEnd);
    QString separator() const;
    void setSeparator(const QString &separatorAndFormat);
    EFormat format() const;
    void setFormat(const EFormat &format);
    bool replace() const;
    void setReplace(bool replace);
    EErrorCode check();
    QString errorMessage(const EErrorCode err);
    bool isLegalFormat();

    ReplicaCommand *clone();
    bool compare(ReplicaCommand* other);
};

#endif // REPLICACOMMAND_H
