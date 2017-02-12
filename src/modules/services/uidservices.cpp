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

#include "xmlEdit.h"
#include "uidservices.h"

UIDesktopServices::UIDesktopServices(QWidget *widget)
{
    _widget = widget;
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    _button = NULL ;
    _progress = NULL ;
#else
#endif
}

UIDesktopServices::~UIDesktopServices()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    if(NULL != _progress) {
        if(_progress->isVisible()) {
            _progress->hide();
        }
        delete _progress;
    }
    if(NULL != _button) {
        _button->deleteLater();
    }

    _progress = NULL ;
#else
#endif
}

QWidget *UIDesktopServices::widget() const
{
    return _widget;
}

void UIDesktopServices::setWidget(QWidget *widget)
{
    _widget = widget;
}

void UIDesktopServices::startIconProgressBar()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    if(NULL == _button) {
        _button = new QWinTaskbarButton(widget);
        _button->setWindow(widget->windowHandle());
    }
#else
    _trayIcon.setVisible(true);
    _trayIcon.showMessage("QXmlEdit", "started", QSystemTrayIcon::Warning);
#endif
}

void UIDesktopServices::setIconProgressBar(const int percent)
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    if(NULL == _button) {
        startIconProgressBar();
    }
    if(NULL == _progress) {
        _progress = button->progress();
    }
    _progress->setVisible(true);
    _progress->setValue(percent);
#else
    _trayIcon.showMessage("QXmlEdit", QString("Progress %1").arg(percent), QSystemTrayIcon::Warning);
#endif
}

void UIDesktopServices::endIconProgressBar()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    if(NULLL != _progress) {
        _progress->hide();
    }
#else
    _trayIcon.showMessage("QXmlEdit", "end", QSystemTrayIcon::Warning);
#endif
}
