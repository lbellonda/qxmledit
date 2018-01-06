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

#ifndef FRAMEWORK_UIDELEGATE_H
#define FRAMEWORK_UIDELEGATE_H

#include <QString>
#include <QWidget>

// This class is a callback interface.

class Element;

class UIDelegate
{
public:
    virtual void registerError() = 0 ;
    virtual void error(const QString& message) = 0 ;
    virtual void error(QWidget *widget, const QString& message) = 0 ;
    virtual void warning(const QString& message) = 0 ;
    virtual void message(const QString& message) = 0 ;
    virtual bool askYN(const QString & message) = 0 ;
    virtual bool askYN(QWidget *parent, const QString & message) = 0 ;
    //--
    virtual void errorNoSel(QWidget *parent) = 0 ;
    virtual void errorOutOfMem(QWidget *parent) = 0;

    virtual QString msgOutOfMem() = 0 ;

    virtual QWidget *getMainWidget() = 0;
    virtual QString getAppTitle() = 0;
    virtual QString editNodeElementAsXML(const bool isBase64Coded, Element *pElement, const QString &text, const bool isCData, bool &isCDataOut, bool &isOk) = 0;

    virtual void resetErrorCount() = 0 ;
    virtual int errorCount() = 0 ;
};

#endif // FRAMEWORK_UIDELEGATE_H
