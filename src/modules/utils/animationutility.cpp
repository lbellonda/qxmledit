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

#include "animationutility.h"
#include "utils.h"

AnimationUtility::AnimationUtility()
{
    //
}

AnimationUtility::~AnimationUtility()
{
    //
}

//------------------------------------------------------------------------------------

void AnimationUtility::setupSizeForLabelIcon(QLabel *label, QPropertyAnimation *animation)
{
    Utils::TODO_THIS_RELEASE("vedi commenti");
    if(!label->hasScaledContents()) {
        label->setScaledContents(true);
    }
    const QSizeF size(label->pixmap()->width(), label->pixmap()->height());
    animation->setTargetObject(label);
    animation->setPropertyName("size");
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setDuration(500);
    animation->setLoopCount(-1);
    animation->setStartValue(size);
    animation->setEndValue(size);
    animation->setKeyValueAt(0.5, QSize(scaleValue(size.width()), scaleValue(size.height())));
}

qreal AnimationUtility::scaleValue(const qreal inValue)
{
    return inValue - 1;
}
