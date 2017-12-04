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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    _button = NULL ;
    _progress = NULL ;
#endif
#else
#endif
}

UIDesktopServices::~UIDesktopServices()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
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
#endif
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    if(NULL == _button) {
        if(NULL != _widget) {
            _button = new QWinTaskbarButton(_widget);
            _button->setWindow(_widget->windowHandle());
        }
    }
#endif
#else
#endif
}

void UIDesktopServices::setIconProgressBar(
    const int
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
    percent
#endif
)
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    if(NULL == _button) {
        startIconProgressBar();
    }
    if(NULL == _progress) {
        _progress = _button->progress();
    }
    if(NULL != _progress) {
        _progress->setVisible(true);
        _progress->setValue(percent);
    }
#endif
#else
#endif
}

void UIDesktopServices::endIconProgressBar()
{
#if defined(Q_WS_WIN) || defined(Q_OS_WIN)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    if(NULL != _progress) {
        _progress->hide();
    }
#endif
#else
#endif
}
