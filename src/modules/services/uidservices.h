/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef UIDSERVICES_H
#define UIDSERVICES_H

#include "xmlEdit.h"
#include "libQXmlEdit_global.h"
#include <QWidget>
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

class LIBQXMLEDITSHARED_EXPORT UIDesktopServices
{
    QWidget *_widget;
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    QWinTaskbarButton *_button;
    QWinTaskbarProgress *_progress;
#else
#endif
public:
    void startIconProgressBar();
    void setIconProgressBar(const int percent);
    void endIconProgressBar();

    UIDesktopServices(QWidget *widget);
    ~UIDesktopServices();
    QWidget *widget() const;
    void setWidget(QWidget *widget);
};

#endif // UIDSERVICES_H
