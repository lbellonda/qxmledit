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

#ifndef FAKEUIDELEGATE_H
#define FAKEUIDELEGATE_H

#include "UIDelegate.h"

class FakeUIDelegate : public UIDelegate
{
public:
    QString lastErrorMsg;
    bool isError ;
    int _errors;
    bool _beforeLoad ;
    int _askCountBeforeLoad;
    int _askCountAfterLoad;

    FakeUIDelegate();
    virtual ~FakeUIDelegate();

    void error(const QString& message) ;
    void error(QWidget *parent, const QString& message) ;
    void warning(const QString& message) ;
    void message(const QString& message) ;
    virtual bool askYN(const QString & message) ;
    virtual bool askYN(QWidget *parent, const QString & message);
    virtual void errorNoSel(QWidget *parent) ;
    virtual void errorOutOfMem(QWidget *parent);
    virtual void resetErrorCount() ;
    virtual int errorCount() ;

    void justBeforeLoad();
    void registerAsk();

    QString msgOutOfMem();

    QWidget *getMainWidget();
    QString getAppTitle();
    QString editNodeElementAsXML(const bool isBase64Coded, Element *pElement, const QString &text, const bool isCData, bool &isCDataOut, bool &isOk);

    int askCountBeforeLoad() const;
    void setAskCountBeforeLoad(int askCountBeforeLoad);
    int askCountAfterLoad() const;
    void setAskCountAfterLoad(int askCountAfterLoad);
    int askTotalCount() const ;
};

class FakeUIDelegateYes : public FakeUIDelegate
{
public:

    FakeUIDelegateYes();
    ~FakeUIDelegateYes();

    virtual bool askYN(const QString & message) ;
    virtual bool askYN(QWidget *parent, const QString & message);

};

#endif // FAKEUIDELEGATE_H
