/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef MAINWNDCONTROLLER_H
#define MAINWNDCONTROLLER_H

#include <QObject>

class MainWindow;
class AnonContext;
class AnonAlg;
class AnonymizeParameters;

class MainWndController : public QObject
{
    Q_OBJECT
    MainWindow *_w;

    bool anonymizeGetParams(AnonymizeParameters *params);

    void innerAnonymize(AnonAlg *alg);
    void innerAnonymize(AnonContext *context);

public:
    explicit MainWndController(QObject *parent = 0);
    virtual ~MainWndController();

    void setWindow(MainWindow *newWindow);
    //-----
    void onAnonymize();
    void onAnonymizeFile();
    void anonymize(AnonAlg *alg);
    void anonymize(AnonContext *context);
    void onAnonymize(AnonContext *context);
    //---
    QString askNewXSIType(Element *selection);

signals:

public slots:

};

#endif // MAINWNDCONTROLLER_H
