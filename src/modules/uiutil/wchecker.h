/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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

#ifndef WCHECKER_H
#define WCHECKER_H

#include <QWidget>
#include <QTableWidgetItem>

#include "libQXmlEdit_global.h"

class LIBQXMLEDITSHARED_EXPORT UIWidgetHelper : public QObject
{
    Q_OBJECT
protected:
    UIWidgetHelper *_parent;
    QWidget *_source;
    QList<QWidget*> targets;
    bool _enabled;

public:
    UIWidgetHelper(QWidget *source);
    virtual ~UIWidgetHelper();

    virtual void pause() ;
    virtual void fireEvent() = 0 ;
    virtual void activate() = 0 ;

    void setParent(UIWidgetHelper *newParent);
    bool isEnabled();
};


class LIBQXMLEDITSHARED_EXPORT UIHelperManager
{

    QList<UIWidgetHelper*> _children;
    bool _enabled;
    void reset();

public:
    UIHelperManager();
    virtual ~UIHelperManager();

    void addHelper(UIWidgetHelper* helper);
    void fireEvent();
    void pause();
};

class QTableWidget ;


class LIBQXMLEDITSHARED_EXPORT UIDataBaseCheckWidgetHelper : public UIWidgetHelper
{
    Q_OBJECT
protected:
    QList<QWidget*> _allCheckedHandler;
    QList<QWidget*> _noneCheckedHandler;
    QList<QWidget*> _someCheckedHandler;

    void enableList(QList<QWidget*> &list, const bool setOn);

public:
    UIDataBaseCheckWidgetHelper(QWidget *src);
    virtual ~UIDataBaseCheckWidgetHelper();


    void addAllWidget(QWidget* w);
    void addSomeWidget(QWidget* w);
    void addNoneWidget(QWidget* w);

    virtual void fireEvent() = 0;
    virtual void activate() = 0;

};

class LIBQXMLEDITSHARED_EXPORT UITableCheckWidgetHelper : public UIDataBaseCheckWidgetHelper
{

    Q_OBJECT
    QTableWidget *_table;

    void itemClickedEval();

public:
    UITableCheckWidgetHelper(QTableWidget *src);
    virtual ~UITableCheckWidgetHelper();


    virtual void fireEvent() ;
    virtual void activate() ;

private slots:
    void onItemClicked(QTableWidgetItem *);
    void onItemChanged(QTableWidgetItem *);
};


#endif // WCHECKER_H
