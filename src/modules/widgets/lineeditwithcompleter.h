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


#ifndef LINEEDITWITHCOMPLETER_H
#define LINEEDITWITHCOMPLETER_H

#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QSet>
#include <QStringListModel>
#include <QAction>
#include "ICompleter.h"

class LineEditWithCompleter : public QLineEdit, public ICompleter
{
    Q_OBJECT
    Q_INTERFACES(ICompleter)

    bool _isCompleting;
    QCompleter *_completer;
    QStringListModel _namesModel;
    QKeySequence *_keySequence;

    QString _wordSeparator ;
    int _wordStart;
    int _keyToActivate;

    bool handleKeyEvent(QKeyEvent * event);
    bool handleKeyEventForCompleter(QKeyEvent * event);
    void activateCompleter(const QString & prefix);
    bool isCompleterActive();

public:
    explicit LineEditWithCompleter(QWidget *parent = 0);
    virtual ~LineEditWithCompleter();


    void setup();
    void setActionShortcut(QAction *action);
    void setData(QSet<QString> newData);
    void setData(QSet<QString> *newData);
    QSet<QString> data();
    void autocomplete();


protected:
    void keyPressEvent(QKeyEvent *event);

signals:

public slots:

private slots:
    void onCompleterFired(const QString & text);
    void onAutocompleteFunctionActivated();

};

#endif // LINEEDITWITHCOMPLETER_H
