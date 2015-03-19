/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef SEARCHXQUERY_H
#define SEARCHXQUERY_H

#include <QObject>
#include <QXmlQuery>
#include <QAbstractMessageHandler>

#include "libQXmlEdit_global.h"
class Regola;
class FindTextParams;
class Element;

class LIBQXMLEDITSHARED_EXPORT SearchXQuery : public QAbstractMessageHandler
{
    Q_OBJECT
    QXmlNamePool _namePool;
    QXmlQuery   _query;

    void searchAndDisplay(Regola *regola, const QString expression);
public:
    explicit SearchXQuery(QObject *parent = 0);
    virtual ~SearchXQuery();

    void search(Regola *regola, Element *element, FindTextParams &search);

    virtual void handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation);

signals:

public slots:

};

#endif // SEARCHXQUERY_H
