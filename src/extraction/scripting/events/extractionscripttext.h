/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#ifndef EXTRACTIONSCRIPTTEXT_H
#define EXTRACTIONSCRIPTTEXT_H

#include "xmlEdit.h"

class ExtractionScriptTextEvent : public QObject
{
    Q_OBJECT

    bool _isModified;
    bool _isCDATA ;
    QString _text ;
    bool _isIgnored;
    bool _isWhitespace;
    bool _isError;
    QString _errorMessage;

public:
    explicit ExtractionScriptTextEvent(QObject *parent = NULL);
    virtual ~ExtractionScriptTextEvent();

    Q_PROPERTY(bool ignored READ isIgnored WRITE setIgnored)
    Q_PROPERTY(bool isModified READ isModified)
    Q_PROPERTY(bool isCDATA READ isCDATA WRITE setCDATA)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool isWhitespace READ isWhitespace)
    Q_PROPERTY(QString errorMessage READ errorMessage WRITE setErrorMessage)
    Q_PROPERTY(bool isError READ isError WRITE setError)

    bool compareTo(ExtractionScriptTextEvent &other);

    bool isModified() const;
    bool resetModifed();

    bool isCDATA() const;
    void setCDATA(bool isCDATA);

    QString text() const;
    void setText(const QString &value);

    bool isIgnored() const;
    void setIgnored(bool isIgnored);

    void setWhiteSpaceFlag(bool how);

    bool isWhitespace() const;

    bool isError() const;
    void setError(bool value);

    QString errorMessage() const;
    void setErrorMessage(const QString &value);

signals:

public slots:
};

#endif // EXTRACTIONSCRIPTTEXT_H
