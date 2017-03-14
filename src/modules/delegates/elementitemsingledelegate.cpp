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


#include "elementitemsingledelegate.h"
#include "elementviewinfo.h"
#include "element.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <qglobal.h>
#include "utils.h"
#include "modules/services/colormanager.h"



const int ElementItemSingleDelegate::EditStateBandWidth = 4 ;

const QBrush ElementItemSingleDelegate::normalBrush(QColor(0xC0, 0xC0, 0xC0));
const QBrush ElementItemSingleDelegate::editedBrush(QColor(0xFF, 0x80, 0x80));
const QBrush ElementItemSingleDelegate::savedBrush(QColor(0x40, 0xC0, 0x40));

/**
  The ID of the style is in the item (can be onerous)
  The data are evaluated live from the Element

*/

ElementItemSingleDelegate::ElementItemSingleDelegate(PaintInfo * paramPaintInfo, QObject *parent) : QStyledItemDelegate(parent)
{
    _origDataForAnonPreview = NULL ;
    _isAnonPreview = false;
    _colorInfo = QColor(0xC0, 0xC0, 0xC0);
    _paintInfo = paramPaintInfo ;
    _inited = false;
    _hasColorInverted = false ;
}

ElementItemSingleDelegate::~ElementItemSingleDelegate()
{
    if(NULL != _origDataForAnonPreview) {
        delete _origDataForAnonPreview ;
    }
}

bool ElementItemSingleDelegate::diffColorOverThr(QColor &c1, QColor &c2, const int threshold)
{
    int br = c1.red();
    int bg = c1.green();
    int bb = c1.blue();

    int sr = c2.red();
    int sg = c2.green();
    int sb = c2.blue();

    volatile double l1 = (int)(0.299 * br + 0.587 * bg + 0.114 * bb);
    volatile double l2 = (int)(0.299 * sr + 0.587 * sg + 0.114 * sb);
    l1 = l1;
    l2 = l2;

    if((((br - sr) * (br - sr)) >> 1) + (bg - sg) * (bg - sg) + (((bb - sb) * (bb - sb)) >> 3) > threshold) {
        return true;
    }
    return false;
}

bool ElementItemSingleDelegate::diffLightnessThr(QColor &c1, QColor &c2, const int threshold)
{
    int br = c1.red();
    int bg = c1.green();
    int bb = c1.blue();

    int sr = c2.red();
    int sg = c2.green();
    int sb = c2.blue();

    int l1 = br / 3 + bg / 2 + bb / 10;
    int l2 = sr / 3 + sg / 2 + sb / 10;

    if(abs(l1 - l2) > threshold) {
        return true;
    }
    return false;
}

void ElementItemSingleDelegate::reset()
{
    _inited = false ;
}


bool ElementItemSingleDelegate::isAnonPreview() const
{
    return _isAnonPreview;
}

void ElementItemSingleDelegate::setIsAnonPreview(bool isAnonPreview)
{
    _isAnonPreview = isAnonPreview;
}


QHash<void *, QString> *ElementItemSingleDelegate::origDataForAnonPreview() const
{
    return _origDataForAnonPreview;
}

void ElementItemSingleDelegate::setOrigDataForAnonPreview(QHash<void *, QString> *origDataForAnonPreview)
{
    if(NULL != _origDataForAnonPreview) {
        delete _origDataForAnonPreview ;
    }
    _origDataForAnonPreview = origDataForAnonPreview;
}

void ElementItemSingleDelegate::calcTextColor(const QStyleOptionViewItem & option)
{
    STYLEVIEWITEM_OPTION4(option);
    _inited = true;
    QStyle * style = option4.widget->style();
    if(NULL != style) {
        if(!strcmp(style->metaObject()->className(), "QWindowsVistaStyle")) {
            _hasColorInverted = true ;
        }
    }

    QPalette palette = option.palette;
    QColor backgroundColor = palette.color(QPalette::Normal, QPalette::Background);
    QColor textColor = palette.color(QPalette::Normal, QPalette::Text);
    QColor hiTextColor = palette.color(QPalette::Normal, QPalette::HighlightedText);

    _textAnonColor = _paintInfo->colorManager()->anonTextPreview()->color();

    if(_paintInfo->colorManager()->procInstr()->isSet()) {
        _cyanBrush = QBrush(_paintInfo->colorManager()->procInstr()->color());
    } else {
        QColor cyan = _paintInfo->colorManager()->procInstr()->color();
        if(diffColorOverThr(backgroundColor, cyan, 0x3 * 4)) {
            _cyanBrush = QBrush(cyan);
        } else {
            _cyanBrush = QBrush(QColor(0x00, 0x40, 0x40));
        }
    }
    _attrNamesColor = _paintInfo->colorManager()->attributeNames()->color();
    if(!_paintInfo->colorManager()->attributeNames()->isSet()) {
        if(!diffColorOverThr(textColor, _attrNamesColor, 0x3 * 4)) {
            if(textColor.lightness() > 128) {
                int hue = textColor.hslHue();
                hue = (hue + 128) % 360;
                int saturation = textColor.hslSaturation();
                int lightness = textColor.lightness();
                textColor.setHsl(hue, saturation, lightness);
                _attrNamesColor = textColor;
            } else {
                _attrNamesColor = QColor(0x00, 0x40, 0x20);
            }
        }
    }

    QColor attrCol = _paintInfo->colorManager()->attributeValues()->color();
    _attrValuesColor = attrCol ;
    if(!_paintInfo->colorManager()->attributeValues()->isSet()) {
        if(diffColorOverThr(backgroundColor, attrCol, 3 * 40 * 40) && diffLightnessThr(backgroundColor, attrCol, 0x60))  {
            _attrValuesColor = attrCol;
        } else {
            if(backgroundColor.lightness() > 128) {
                _attrValuesColor = QColor(0x00, 0x00, 0x60);
            } else {
                _attrValuesColor = QColor(0x00, 0xC0, 0xFF);
            }
        }
    }
    _attrAnonValuesColor = _paintInfo->colorManager()->anonAttrPreview()->color();
    _attrSelectedText = hiTextColor;
    _hilitedTextColor = hiTextColor;

    QColor grayColor(0x80, 0x80, 0x80);
    if(_paintInfo->colorManager()->comments()->isSet()) {
        _commentBrush = QBrush(_paintInfo->colorManager()->comments()->color());
    } else {
        if(!diffColorOverThr(backgroundColor, grayColor, 10 * 4)) {
            int lightness = (backgroundColor.lightness() + 64) % 256;
            _commentBrush = QBrush(QColor(lightness, lightness, lightness));
        } else {
            _commentBrush = QBrush(grayColor);
        }
    }

    if(_paintInfo->colorManager()->textDecl()->isSet()) {
        _textDeclBrush = QBrush(_paintInfo->colorManager()->textDecl()->color());
    } else {
        if(!diffColorOverThr(backgroundColor, grayColor, 10 * 4)) {
            int lightness = (backgroundColor.lightness() + 64) % 256;
            _textDeclBrush = QBrush(QColor(lightness, lightness, lightness));
        } else {
            _textDeclBrush = QBrush(grayColor);
        }
    }

    QColor ltGrayColor(0xB0, 0xB0, 0xB0);
    if(_paintInfo->colorManager()->info()->isSet()) {
        ltGrayColor = _paintInfo->colorManager()->info()->color();
        _infoBrush = QBrush(ltGrayColor);
    } else {
        if(!diffColorOverThr(backgroundColor, ltGrayColor, 10 * 4)) {
            int lightness = (backgroundColor.lightness() + 64) % 256;
            _infoBrush = QBrush(QColor(lightness, lightness, lightness));
        } else {
            _infoBrush = QBrush(ltGrayColor);
        }
    }

    if(_paintInfo->colorManager()->tags()->isSet()) {
        _tagColor = _paintInfo->colorManager()->tags()->color();
    } else {
        _tagColor = _paintInfo->colorManager()->tags()->defaultValue();
        if(!diffColorOverThr(backgroundColor, _tagColor, 0x3 * 4)) {
            if(_tagColor.lightness() > 128) {
                int hue = _tagColor.hslHue();
                hue = (hue + 128) % 360;
                int saturation = textColor.hslSaturation();
                int lightness = textColor.lightness();
                _tagColor.setHsl(hue, saturation, lightness);
            } else {
                _tagColor.setRgb(0x60, 0, 0x60);
            }
        }
    }

    _colorInfo = _infoBrush.color();
}


void ElementItemSingleDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    STYLEVIEWITEM_OPTION4(option);
    if(!_inited) {
        const_cast<ElementItemSingleDelegate*>(this)->calcTextColor(option);
    }

    bool isReverse = option4.widget->layoutDirection() == Qt::RightToLeft ? true : false ;
    bool isSelected = option.state & QStyle::State_Selected;
    QPalette::ColorGroup colorGroup = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if((colorGroup == QPalette::Normal) && (0 == (option.state & QStyle::State_Active))) {
        colorGroup = QPalette::Inactive;
    }
    QColor defaultTextColor = option.palette.color(colorGroup, isSelected ? (_hasColorInverted ? QPalette::Text : QPalette::HighlightedText) : QPalette::Text);

    if(!isSelected) {
        QStyledItemDelegate::paint(painter, option, index) ;
    } else {
        QPoint topLeft = option.rect.topLeft();
        QPoint bottomLeft = option.rect.bottomLeft();
        QLinearGradient backgroundGradient(topLeft, bottomLeft);
        QColor colorBg = option.palette.color(QPalette::Normal, QPalette::Highlight);
        backgroundGradient.setColorAt(0.0, colorBg.lighter(50));
        backgroundGradient.setColorAt(1.0, colorBg);
        painter->fillRect(option.rect, QBrush(backgroundGradient));
        // remember Vista
        defaultTextColor = _hilitedTextColor ;
    }

    ElementViewInfo dataInfo ;
    dataInfo._isAnonPreview = _isAnonPreview;
    dataInfo._origDataForAnonPreview = _origDataForAnonPreview ;
    dataInfo._commentBrush = _commentBrush ;
    dataInfo._procInstrBrush = _cyanBrush ;
    dataInfo._attrTextColor = defaultTextColor ;
    dataInfo._textDeclBrush = _textDeclBrush ;
    dataInfo._showAllComments = _paintInfo->showFullComments();
    dataInfo._attrAnonValuesColor = _attrAnonValuesColor ;
    QColor selTextColor = defaultTextColor;
    if(isSelected) {
        dataInfo._attrNamesColor = _attrSelectedText ;
        dataInfo._attrValuesColor = _attrSelectedText;
        selTextColor = _attrSelectedText;
    } else {
        dataInfo._attrNamesColor = _attrNamesColor;
        dataInfo._attrValuesColor = _attrValuesColor;
    }
    QString styleEntryId = index.model()->data(index, Element::StyleIdRole).toString();
    Element *element = Element::fromModelIndex(index);
    if(NULL != element) {
        if(element->getUI()->isHidden()) {
            return ;
        }
        element->getVisInfo(_paintInfo, &dataInfo, isSelected, selTextColor);
    }
    if(!VStyle::globalInitFlag) {
        VStyle::globalActivate(element->createElementFonts(option.font), QColor(0xC0, 0x40, 0xC0));
    }

    StyleEntry *se = _paintInfo->styleEntryById(styleEntryId);
    painter->save();
    int currentPosX = option.rect.x();
    int sign = 1;
    if(isReverse) {
        currentPosX = option.rect.right() - option.decorationSize.width();
        sign = -1 ;
    }
    //------------------------------------------------------------------------------------------------------------------------------------
    if(NULL != element) {
        QRect markRect = option.rect;
        int startPosition = static_cast<const QTreeWidget*>(option4.widget)->columnViewportPosition(0) ;
        if(isReverse) {
            startPosition = -static_cast<const QTreeWidget*>(option4.widget)->columnWidth(0) - option.rect.left() - EditStateBandWidth;
        }
        markRect.setLeft(startPosition);
        markRect.setWidth(EditStateBandWidth);
        if(element->saved()) {
            painter->fillRect(markRect, savedBrush);
        } else if(element->edited()) {
            painter->fillRect(markRect, editedBrush);
        } else {
            painter->fillRect(markRect, normalBrush);
        }
    }

    if(!dataInfo._icon.isNull()) {
        QSize size = option.decorationSize;
        int yOffset = (option.rect.height() - size.height()) >> 1;
        if(yOffset < 0) {
            yOffset = 0 ;
        }
        dataInfo._icon.paint(painter, currentPosX, option.rect.y() + yOffset, size.width(), size.height());
        currentPosX += (size.width() + HGap) * sign ;
    }
    //------------------------------------------------------------------------------------------------------------------------------------
    {
        bool isFont = false ;
        bool isBrush = false ;
        QFont oldFont = painter->font();
        QPen oldPen = painter->pen();
        QBrush backBrush = painter->background();
        QBrush fgBrush = painter->brush();
        const QFontMetrics *metrics = &option.fontMetrics;
        bool isBackBrush = false;
        if(NULL != se) {
            if(se->isColor()) {
                painter->setBrush(se->brush());
                painter->setPen(QPen(se->brush().color()));
                isBrush = true ;
            }
            if(se->useBackColor()) {
                painter->setBackground(se->backColor());
                isBackBrush = true;
            }
            QFont *styleFont = se->font();
            if(NULL != styleFont) {
                painter->setFont(*styleFont);
                if(NULL != se->fontMetrics()) {
                    metrics = se->fontMetrics();
                }
                isFont = true ;
            }
        }
        //usa il default se non assegnato
        if(!isBrush) {
            if(!isSelected) {
                if(dataInfo._useFgBrush) {
                    painter->setBrush(dataInfo._mainFgBrush);
                    painter->setPen(QPen(dataInfo._mainFgBrush.color()));
                } else {
                    QColor color = _paintInfo->colorManager()->tags()->color();
                    painter->setBrush(QBrush(color));
                    painter->setPen(color);
                }
            } else {
                painter->setPen(defaultTextColor);
            }
        }
        if(!isFont) {
            painter->setFont(VStyle::defaultFont());
            metrics = &VStyle::defaultFontMetrics();
        }
        QRect rect(option.rect);
        int textWidth = metrics->width(dataInfo._tagInfo);
        if(isReverse) {
            rect.setLeft(currentPosX - textWidth - textWidth / 10);
            rect.setWidth(textWidth + textWidth / 10);
        } else {
            rect.setLeft(currentPosX);
            rect.setWidth(textWidth + textWidth / 10);
        }
        if(isBackBrush) {
            if(isSelected) {
                QColor color(painter->background().color());
                color.setAlpha(128);
                painter->fillRect(rect, QBrush(color));
            } else {
                painter->fillRect(rect, painter->background());
            }
        }

        if(dataInfo._isAnonPreview) {
            if((NULL != dataInfo._origDataForAnonPreview) && dataInfo._origDataForAnonPreview->contains(element)) {
                QColor color = _paintInfo->colorManager()->anonElementPreview()->color();
                painter->setBrush(QBrush(color));
                painter->setPen(color);
            }
        }
        painter->drawText(rect, dataInfo._tagInfo, option.displayAlignment);
        currentPosX += (textWidth + HGap) * sign ;
        currentPosX += (4 * metrics->width(" ")) * sign;
        painter->setFont(oldFont);
        painter->setPen(oldPen);
        painter->setBackground(backBrush);
        painter->setBrush(fgBrush);
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    // background already painted in the default method.
    painter->setPen(defaultTextColor);
    if(!dataInfo._childrenSizeInfo.isEmpty()) {
        QRect rect(option.rect);
        int textWidth = painter->fontMetrics().width(dataInfo._childrenSizeInfo);
        if(isReverse) {
            rect.setLeft(currentPosX - textWidth - 10);
            rect.setWidth(textWidth + 10);
        } else {
            rect.setLeft(currentPosX);
            rect.setWidth(textWidth + 10);
        }
        painter->setFont(option.font);
        QPen oldPen = painter->pen();
        if(isSelected) {
            painter->setPen(defaultTextColor);
        } else {
            painter->setPen(_colorInfo);
        }

        painter->drawText(rect, dataInfo._childrenSizeInfo, option.displayAlignment);
        painter->setPen(oldPen);
        currentPosX += (textWidth + HGap) * sign ;
    }
    if(!dataInfo._attributesIcon.isNull()) {
        QSize size = option.decorationSize;
        int yOffset = (option.rect.height() - size.height()) >> 1;
        if(yOffset < 0) {
            yOffset = 0 ;
        }
        dataInfo._attributesIcon.paint(painter, currentPosX, option.rect.y() + yOffset, size.width(), size.height());
        currentPosX += (size.width() + HGap) * sign ;
    }
    //------------------------------------------------------------------------------------------------------------------------------------
    if(!dataInfo._attrTextInfo.isEmpty()) {
        if(dataInfo._attrTextInfoIsHtml) {
            _document.setHtml(dataInfo._attrTextInfo);
        } else {
            painter->setPen(defaultTextColor);
            _document.setPlainText(dataInfo._attrTextInfo);
        }
        int offsetX = 0 ;
        if(isReverse) {
            offsetX = - _document.idealWidth();
        }
        painter->translate(currentPosX + offsetX, option.rect.y());
        if(dataInfo._attrTextInfoIsHtml) {
            _document.drawContents(painter);
        } else {
            QAbstractTextDocumentLayout::PaintContext paintContext;
            if(isSelected) {
                paintContext.palette.setColor(QPalette::Text, defaultTextColor);
            } else {
                paintContext.palette.setColor(QPalette::Text, dataInfo._attrTextColor);
            }
            _document.documentLayout()->draw(painter, paintContext);
        }
        painter->translate(-(currentPosX + offsetX), -option.rect.y());
        currentPosX += (_document.size().width() + HGap) * sign;
    }
    //------------------------------------------------------------------------------------------------------------------------------------
    if(!dataInfo._inlineTextInfo.isEmpty()) {
        if(dataInfo._isTextAnon) {
            painter->setPen(_textAnonColor);
        } else {
            painter->setPen(defaultTextColor);
        }
        QString text;

        text.append(dataInfo._inlineTextInfo);

        if(_paintInfo->compactView()) {
            if(dataInfo._isTextAnon) {
                painter->setPen(_textAnonColor);
            } else {
                painter->setPen(defaultTextColor);
            }
            QRect rect(option.rect);
            Utils::TODO_NEXT_RELEASE("reverse text");

            if(isReverse) {
                int textWidth = painter->fontMetrics().width(dataInfo._inlineTextInfo);
                rect.setLeft(currentPosX - textWidth - 10);
                rect.setWidth(textWidth + 10);
            } else {
                rect.setLeft(currentPosX);
                rect.setWidth(option.rect.right() - currentPosX - 1);
            }
            painter->drawText(rect, text, option.displayAlignment);
        } else {
            _document.setPlainText(text);
            int offsetX = 0 ;
            if(isReverse) {
                offsetX = - _document.idealWidth();
            }
            painter->translate(currentPosX + offsetX, option.rect.y());
            QAbstractTextDocumentLayout::PaintContext paintContext;
            if(dataInfo._isTextAnon) {
                paintContext.palette.setColor(QPalette::Text, _textAnonColor);
            } else {
                paintContext.palette.setColor(QPalette::Text, defaultTextColor);
            }
            /*if (rect.isValid())
            {
                p->setClipRect(rect);
                ctx.clip = rect;
            }*/
            _document.documentLayout()->draw(painter, paintContext);
        }
    }
    if(isSelected) {
        option4.widget->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option4, painter, option4.widget);
    }
    painter->restore();
}

QSize ElementItemSingleDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    if(!_inited) {
        const_cast<ElementItemSingleDelegate*>(this)->calcTextColor(option);
    }

    ElementViewInfo dataInfo ;
    dataInfo._showAllComments = _paintInfo->showFullComments();
    QString styleEntryId = index.model()->data(index, Element::StyleIdRole).toString();
    bool isSelected = option.state & QStyle::State_Selected;
    Element *element = Element::fromModelIndex(index);
    if(NULL != element) {
        if(NULL == element->getParentRule()) {
            return QSize(0, 0);
        }
        if(element->getUI()->isHidden() || element->isHidden()) {
            return QSize(0, 0);
        }
        element->getVisInfo(_paintInfo, &dataInfo, isSelected, QColor());
    }
    int currentPosX = 0 ;
    int minHeight = option.fontMetrics.lineSpacing();

    if(!dataInfo._icon.isNull()) {
        QSize size = option.decorationSize;
        currentPosX += size.width() + HGap ;
    }

    if(!dataInfo._tagInfo.isEmpty()) {
        const QFontMetrics *fontMetrics = &option.fontMetrics;
        if(NULL != _paintInfo->currentStyle()) {
            StyleEntry *se = _paintInfo->styleEntryById(styleEntryId);
            if((NULL != se) && (NULL != se->font())) {
                fontMetrics = se->fontMetrics();
            } else {
                fontMetrics = &VStyle::defaultFontMetrics();
            }
        }
        int textWidth = fontMetrics->width(dataInfo._tagInfo);
        currentPosX += textWidth + HGap ;
    }

    int attrTextHeight = 0 ;
    if(!dataInfo._attrTextInfo.isEmpty()) {
        _document.setDefaultFont(option.font);
        if(dataInfo._attrTextInfoIsHtml) {
            _document.setHtml(dataInfo._attrTextInfo);
        } else {
            _document.setPlainText(dataInfo._attrTextInfo);
        }
        QSizeF sizeF = _document.size();
        attrTextHeight = sizeF.height();
        currentPosX += sizeF.width() + HGap ;
    }
    int textHeight = 0 ;
    if(!dataInfo._inlineTextInfo.isEmpty()) {
        _document.setDefaultFont(option.font);
        _document.setPlainText(dataInfo._inlineTextInfo);
        QSizeF sizeF = _document.size();
        textHeight = sizeF.height();
        currentPosX += sizeF.width();
    }
    return QSize(currentPosX + 10, qMax(qMax(minHeight, attrTextHeight), textHeight));
}
