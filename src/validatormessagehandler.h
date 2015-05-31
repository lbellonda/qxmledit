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

#ifndef VALIDATORMESSAGEHANDLER_H
#define VALIDATORMESSAGEHANDLER_H

#include <QString>

#ifndef USE_QT_44

#include <QAbstractMessageHandler>
#include <QSourceLocation>

class ValidatorMessageHandler : public QAbstractMessageHandler
{
private:
    QtMsgType _type;
    QString _description;
    QUrl _identifier;
    QSourceLocation _sourceLocation;

public:
    ValidatorMessageHandler();
    virtual ~ValidatorMessageHandler();

    void handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation);

    QtMsgType type() {
        return _type;
    }
    const QString & description() {
        return _description ;
    }
    const QUrl & identifier() {
        return _identifier;
    }
    const QSourceLocation &sourceLocation() {
        return _sourceLocation ;
    }
    int line() {
        return _sourceLocation.line() ;
    }
    int column() {
        return _sourceLocation.column() ;
    }

    QString descriptionInPlainText();

};

#endif

#endif // VALIDATORMESSAGEHANDLER_H
