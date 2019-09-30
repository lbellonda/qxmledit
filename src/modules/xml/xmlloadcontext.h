/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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


#ifndef XMLLOADCONTEXT_H
#define XMLLOADCONTEXT_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"

class Element ;

class LIBQXMLEDITSHARED_EXPORT XMLLoadContext
{
    bool    _isError;
    QString _errorMessage;
    QString _encoding ;
    bool _firstElementSeen;
    bool _isAfterDTD;
    QList<Element *> _firstComments;
    qint64 _line;
    qint64 _column;
    qint64 _characterOffset;
public:
    XMLLoadContext();
    ~XMLLoadContext();

    //----
    bool isError() const;
    bool isOk() const;
    void setIsError(bool isError);
    QString errorMessage() const;
    void setErrorMessage(const QString &errorMessage);
    bool setErrorWithMessage(const QString &errorMessage);
    void copyErrorData(QXmlStreamReader *xmlReader);
    /*!
     * \brief setError sets the error with the specified message and return false to chaing this method in a return
     * \param errorMessage the error message
     * \return false()
     */
    bool setError(const QString &errorMessage, QXmlStreamReader *xmlReader);
    bool setErrorFromReader(QXmlStreamReader *xmlReader);
    QString encoding() const;
    void setEncoding(const QString &encoding);
    bool firstElementSeen() const;
    void setFirstElementSeen(bool firstElementSeen);
    bool isAfterDTD() const;
    void setIsAfterDTD(bool isAfterDTD);
    QList<Element *> &firstComments() ;
    void addFirstComment(Element *comment);
    qint64 characterOffset() const;
    void setErrorPosition(const qint64 line, const qint64 column, const qint64 offset);
    bool testAnErrorPos(const QString &dataIn,
                        const qint64 line, const qint64 column, const qint64 offset,
                        const QString &expectedBefore,
                        const QString &expectedIn,
                        const QString &expectedAfter);

};

#endif // XMLLOADCONTEXT_H
