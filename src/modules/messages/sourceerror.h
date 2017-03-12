/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#ifndef SOURCEERROR_H
#define SOURCEERROR_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT SourceMessage
{
public:
    enum EType {
        Debug,
        Info,
        Warning,
        Error
    };
private:
    qint64 _line;
    qint64 _column;
    QString _description;
    QList<int> _path;
    bool _found;
    qint64 _foundLine;
    qint64 _foundColumn;
    EType _type;
    QString _source;
    int _code;
public:
    SourceMessage();
    ~SourceMessage();
    qint64 line() const;
    void setLine(const qint64 &line);
    qint64 column() const;
    void setColumn(const qint64 &column);
    QString description() const;
    void setDescription(const QString &description);
    QList<int> path() const;
    void setPath(const QList<int> &path);
    bool isFound() const;
    void setFound(bool found);
    qint64 foundLine() const;
    void setFoundLine(const qint64 &foundLine);
    qint64 foundColumn() const;
    void setFoundColumn(const qint64 &foundColumn);
    EType type() const;
    void setType(const EType &type);
    QString source() const;
    void setSource(const QString &source);
    int code() const;
    void setCode(int code);
};

#endif // SOURCEERROR_H
