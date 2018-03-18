/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
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

#ifndef DEFAULTUIDELEGATE_H
#define DEFAULTUIDELEGATE_H

#include "xmlEdit.h"
#include "UIDelegate.h"

class QWidget;

class DefaultUIDelegate : public UIDelegate
{
    QWidget * _window;
    QString _lastErrorMsg;
    int _errorCount;
    int _errors;
    bool _beforeLoad ;
    int _askCountBeforeLoad;
    int _askCountAfterLoad;

public:
    DefaultUIDelegate();
    virtual ~DefaultUIDelegate();

    virtual void registerError();
    virtual void error(const QString& message);
    virtual void error(QWidget *widget, const QString& message) ;
    virtual void warning(const QString& message) ;
    virtual void message(const QString& message) ;
    virtual bool askYN(const QString & message) ;
    virtual bool askYN(QWidget *parent, const QString & message) ;
    //--
    virtual void errorNoSel(QWidget *parent) ;
    virtual void errorOutOfMem(QWidget *parent);

    virtual QString msgOutOfMem();

    virtual QWidget *getMainWidget();
    virtual QString getAppTitle();
    virtual QString editNodeElementAsXML(const bool isBase64Coded, Element *pElement, const QString &text, const bool isCData, bool &isCDataOut, bool &isOk);

    virtual void resetErrorCount();
    virtual int errorCount();

    QWidget *getWindow() const;
    void setWindow(QWidget *window);
};

#endif // DEFAULTUIDELEGATE_H
