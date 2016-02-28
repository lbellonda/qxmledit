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


#include "element.h"
#include "modules/delegates/elementviewinfo.h"
#include <regola.h>
#include "utils.h"

/*
const int EPFieldText = 0;
const int EPFieldId = 1;
const int EPFieldChildren = 2;
const int EPFieldSize = 3;
const int EPFieldDetail = 4;
*/

const QString Element::findStyleElementTag(PaintInfo *paintInfo)
{
    VStyle* style = calcStyle(paintInfo);
    // check namespace
    if(NULL != style) {
        StyleEntry *se = chooseStyle(style) ;
        if(NULL != se) {
            return se->id();
        }
    }
    return "";
}

void Element::displayAlt(QTreeWidgetItem * me, PaintInfo *paintInfo)
{
    switch(type) {
    case ET_ELEMENT: {
        QString styleId = findStyleElementTag(paintInfo);
        me->setData(0, StyleIdRole, styleId);
        break;
    }
    case ET_ATTRIBUTE:
    case ET_PROCESSING_INSTRUCTION:
    case ET_COMMENT:
    case ET_TEXT:
    default:
        me->setData(0, StyleIdRole, "");
        break;
    }
    if(wasOpen) {
        ui->setExpanded(true);
    }
}

void Element::getVisInfo(PaintInfo *paintInfo, ElementViewInfo *dataInfo,
                         const bool isSelected, const QColor & selectedTextColor)
{
    dataInfo->reset();
    switch(visibilityState) {
    case EVN_ELLIPSIS:
        dataInfo->_tagInfo = tr("...") ;
        return ;
    case EVN_HIDDEN:
        return ;
    default:
        break;
    }

    if(paintInfo->exploreMode()) {
        QString firstColText = QString("<%1> #%2, data: %L3")
                               .arg(tag())
                               .arg(this->childItems.size())
                               .arg(selfInfo.totalSize + childrenInfo.totalSize);
        dataInfo->_tagInfo = firstColText;
        return;
    }

    bool isShowAttributesLength = paintInfo->showAttributesLength();
    bool isUseFixedFonts = paintInfo->useFixedLengthFont();
    bool isOneAttributePerLine = paintInfo->oneAttrPerLine();
    bool isShowChildIndex = paintInfo->indexPerChild();
    bool isBookmarked = parentRule->isBookmarked(this);
    bool isCompactView = paintInfo->compactView();
    QString firstColText;
    if(isShowChildIndex && (NULL != parentElement)) {
        QTreeWidgetItem *parentItem = parentElement->getUI();
        if(NULL != parentItem) {
            int childIndex = parentItem->indexOfChild(ui);
            firstColText = QString("%1 ").arg(childIndex + 1);
        }
    }

    switch(type) {
    case ET_ELEMENT: {
        firstColText += "<";
        firstColText += tag() ;
        if(paintInfo->isShowElementsIcon()) {
            if(!areChildrenLeavesHidden(getUI())) {
                dataInfo->_icon = isBookmarked ? elementBkmIcon : elementIcon;
            } else {
                dataInfo->_icon = isBookmarked ? childrenHiddenBkmIcon : childrenHiddenIcon;
            }
        } else {
            if(isBookmarked) {
                if(!areChildrenLeavesHidden(getUI())) {
                    dataInfo->_icon = elementBkmIcon ;
                } else {
                    dataInfo->_icon = childrenHiddenBkmIcon ;
                }
            }
        }
        QString qualifiedInfo = getIdUsingStyle(paintInfo);
        if(qualifiedInfo.length() > 0) {
            QString idString = " - ";
            idString.append(qualifiedInfo);
            firstColText.append(idString) ;
        }
        dataInfo->_tagInfo = firstColText ;
        if(paintInfo->showElementSize()) {
            QString children = QString(" Ch. %1 (%2) /")
                               .arg(selfInfo.numElements)
                               .arg(selfInfo.numElements + childrenInfo.numElements);
            dataInfo->_childrenSizeInfo = children ;
            QString size = QString("Sz: %L1   ")
                           .arg(selfInfo.totalSize + childrenInfo.totalSize);
            dataInfo->_childrenSizeInfo.append(size);
        }
        QString textToShow ;
        QString textToShowBase64 ;
        bool collectBase64Text = isShowTextBase64 || paintInfo->showUnBase64();
        QVectorIterator<TextChunk*> tt(textNodes);
        while(tt.hasNext()) {
            TextChunk   *tx = tt.next();
            if(dataInfo->_isAnonPreview) {
                if((NULL != dataInfo->_origDataForAnonPreview) && dataInfo->_origDataForAnonPreview->contains(tx)) {
                    dataInfo->_isTextAnon = true ;
                }
            }
            if(tx->isCDATA) {
                textToShow.append(" [[CDATA:");
                if(isCompactView) {
                    textToShow.append(limitLargeTextWithEllipsis(tx->text.simplified()));
                } else {
                    textToShow.append(limitLargeTextWithEllipsis(tx->text));
                }
                textToShow.append(" ]]");
            } else {
                if(isCompactView) {
                    if(collectBase64Text) {
                        if(!textToShowBase64.isEmpty()) {
                            textToShowBase64.append("\n ---\n");
                        }
                        textToShowBase64.append(limitLargeTextWithEllipsis(Utils::fromBase64(tx->text)));
                    }
                    textToShow.append(limitLargeTextWithEllipsis(tx->text.simplified()));
                } else {
                    if(collectBase64Text) {
                        if(!textToShowBase64.isEmpty()) {
                            textToShowBase64.append("\n ---\n");
                        }
                        textToShowBase64.append(limitLargeTextWithEllipsis(Utils::fromBase64(tx->text)));
                    }
                    textToShow.append(limitLargeTextWithEllipsis(tx->text));
                }
            }
            QString originalText = textToShow ;
            if(isCompactView) {
                textToShow = limitTextWithEllipsis(textToShow);
            } else {
                textToShow = limitLargeTextWithEllipsis(textToShow);
            }
            if(paintInfo->showElementTextLength()) {
                textToShow = QString("(%1)- %2").arg(originalText.length()).arg(textToShow);
            }
        }
        dataInfo->_inlineTextInfo = textToShow ;
        if(!textToShowBase64.isEmpty()) {
            dataInfo->_inlineTextInfo += " Base64:";
            dataInfo->_inlineTextInfo += textToShowBase64;
        }
    }
    break;
    case ET_PROCESSING_INSTRUCTION:
        displayProcessingInstruction(dataInfo, isBookmarked, isSelected);
        break;

    case ET_COMMENT:
        displayComment(dataInfo, isCompactView, isBookmarked, isSelected);
        break;

    case ET_TEXT: {
        firstColText = tr("*text*") ;
        dataInfo->_tagInfo = firstColText;
        if(!isSelected) {
            dataInfo->_mainFgBrush = dataInfo->_textDeclBrush;
            dataInfo->_useFgBrush = true ;
        }
        dataInfo->_icon = isBookmarked ? textBkmIcon : textIcon;
        QString textToShow ;
        if(isCompactView) {
            textToShow = limitTextWithEllipsis(text);
        } else {
            textToShow = limitLargeTextWithEllipsis(text);
        }
        dataInfo->_inlineTextInfo = textToShow;
        if(dataInfo->_isAnonPreview) {
            if((NULL != dataInfo->_origDataForAnonPreview) && dataInfo->_origDataForAnonPreview->contains(this)) {
                dataInfo->_isTextAnon = true ;
            }
        }
    }
    break;

    default:
        dataInfo->_tagInfo = tr("unknown node");
        dataInfo->_icon = isBookmarked ? bookmarkIcon : QIcon();
        break;
    }

    if(!attributes.isEmpty()) {
        bool isFirst = true;
        AttributeFilter *filter = paintInfo->attributeFilter();
        bool skipped = false;
        QString attrList ;

        if(true) {
            dataInfo->_attrTextInfoIsHtml = true ;
            attrList = "<html><body bgcolor=\"transparent\"><FONT color=\"";
            attrList.append(selectedTextColor.name());
            attrList.append("\">");
            if(isUseFixedFonts) {
                attrList += "<tt>";
            }
            int attrIndex = 1 ;
            int size = attributes.size();
            QString fontColorValues = QString("<FONT COLOR=\"%1\">").arg(dataInfo->_attrValuesColor.name());
            QString fontAnonColorValues = QString("<FONT COLOR=\"%1\">").arg(dataInfo->_attrAnonValuesColor.name());
            QString fontColorNames = QString("<FONT COLOR=\"%1\">").arg(dataInfo->_attrNamesColor.name());

            QVector<Attribute*> sortedAttributes ;
            QVector<Attribute*> *theAttributes = &attributes ;
            const int attributesColumnLimit = paintInfo->attributesColumnLimit();
            if(paintInfo->isSortAttributesAlpha()) {
                QMap<QString, Attribute*> attrMap;
                foreach(Attribute * attribute, attributes) {
                    attrMap.insert(attribute->name, attribute);
                }
                //sorteds
                foreach(QString key, attrMap.keys()) {
                    sortedAttributes.append(attrMap.value(key));
                }
                theAttributes = &sortedAttributes ;
            }
            int attrTextLen = 0;
            for(int attr = 0; attr < size ; attr++) {
                Attribute* attribute = theAttributes->at(attr);
                QString key = attribute->name;
                QString value = Utils::escape(attribute->value);
                if(NULL != filter) {
                    bool skip = filter->checkIfSkipped(key);
                    if(skip) {
                        skipped = true ;
                        continue;
                    }
                }
                if(isFirst) {
                    isFirst = false;
                } else {
                    if(isOneAttributePerLine) {
                        if(isCompactView) {
                            bool condOne = (attributesColumnLimit > 0) && (attrTextLen > attributesColumnLimit);
                            bool condTwo = (attributesColumnLimit <= 0) && (attrIndex > ATTR_PER_COLUMN);
                            if(condOne || condTwo) {
                                attrList.append("<br/>\n");
                                attrTextLen = 0 ;
                                attrIndex = 1 ;
                            } else {
                                attrList.append(", ");
                            }
                        } else { // compactView
                            attrList.append("<br/>\n");
                        }
                    } else { //OneAttribute
                        if(isCompactView) {
                            attrList.append(", ");
                        } else {
                            bool condOne = (attributesColumnLimit > 0) && (attrTextLen > attributesColumnLimit);
                            bool condTwo = (attributesColumnLimit <= 0) && (attrIndex > ATTR_PER_COLUMN);
                            if(condOne || condTwo) {
                                attrList.append("<br/>\n");
                                attrTextLen = 0;
                                attrIndex = 1 ;
                            }
                        }
                    }
                }
                attrTextLen += key.length() + attribute->value.length() + 6;
                attrList.append(fontColorNames);
                attrList.append(key);
                if(isShowAttributesLength) {
                    int attrLen = value.length();
                    attrList.append("(");
                    if(isUseFixedFonts) {
                        attrList.append(QString("%1").arg(attrLen, WIDTH_LEN_CHARS));
                    } else {
                        attrList.append(QString("%1").arg(attrLen));
                    }
                    attrList.append(")");
                } else {
                    //
                }
                attrList.append("</FONT>=\"");
                if(dataInfo->_isAnonPreview) {
                    if((NULL != dataInfo->_origDataForAnonPreview) && dataInfo->_origDataForAnonPreview->contains(attribute)) {
                        attrList.append(fontAnonColorValues);
                    } else {
                        attrList.append(fontColorValues);
                    }
                } else {
                    attrList.append(fontColorValues);
                }
                attrList.append(limitLargeTextWithEllipsis(value));
                attrList.append("</FONT>");
                attrList.append("\"");
                attrIndex ++;
            }
            if(isSelected) {
                attrList += "</FONT>";
            }
            if(isUseFixedFonts) {
                attrList += "</tt>";
            }
            attrList.append("</FONT></body><html>");
        }
        dataInfo->_attrTextInfo = attrList;

        if(skipped) {
            dataInfo->_attributesIcon = filteredAttributesIcon ;
        }
        /*
            me->setForeground(paintInfo->columnForAttributes, attrBrush);
            if(isUseFixedFonts) {
                if(firstTimeFixedWidthFont) {
                    firstTimeFixedWidthFont = false ;
                    createFixedWidthFonts();
                }
                me->setFont(paintInfo->columnForAttributes, fixedWidthFont);
            } else {
                if(NULL == attrFont) {
                    QFontInfo info(ui->font(0));
                    QString family = info.family();
                    attrFont = new QFont(family);
                    defaultAttrFonts = attrFont->pointSize();
                }b
                me->setFont(paintInfo->columnForAttributes, *attrFont);
            }

        } else {
            QString txtAttrToShow = convertStringToHtml(attrTxt, paintInfo->attributesHTML());
            setAttributesText(me, paintInfo, txtAttrToShow);
            if(attrTxt.length() > 0) {
                if(NULL == attrFont) {
                    QFontInfo info(ui->font(0));
                    QString family = info.family();
                    attrFont = new QFont(family);
                    defaultAttrFonts = attrFont->pointSize();
                }
                me->setFont(paintInfo->columnForAttributes, *attrFont);
            }
        }*/
    }
}

void Element::displayProcessingInstruction(ElementViewInfo * dataInfo, const bool isBookmarked, const bool isSelected)
{
    QString firstColText = QString("*Processing Instruction* <?").append(getPITarget()) ;
    dataInfo->_tagInfo = firstColText ;
    if(!isSelected) {
        dataInfo->_mainFgBrush = dataInfo->_procInstrBrush ;
        dataInfo->_useFgBrush = true;
    }
    dataInfo->_icon = isBookmarked ? procInstrBkmIcon : procInstrIcon;
    QString pI = getPITarget();
    pI.append("(");
    pI.append(getPIData());
    pI.append(")");
    dataInfo->_attrTextInfo = pI;
}

void Element::displayComment(ElementViewInfo * dataInfo, const bool isCompactView, const bool isBookmarked, const bool isSelected)
{
    QString firstColText = tr("*comment*") ;
    dataInfo->_tagInfo = firstColText ;
    if(!isSelected) {
        dataInfo->_mainFgBrush = dataInfo->_commentBrush ;
        dataInfo->_useFgBrush = true;
    }
    dataInfo->_icon = isBookmarked ? commentBkmIcon : commentIcon;
    QString textToShow ;
    if(isCompactView && !dataInfo->_showAllComments) {
        textToShow = limitTextWithEllipsis(text);
    } else {
        textToShow = limitLargeTextWithEllipsis(text);
    }
    dataInfo->_attrTextInfo = textToShow;
    dataInfo->_attrTextColor = dataInfo->_commentBrush.color();
}

const QString Element::getIdUsingStyle(PaintInfo * paintInfo)
{
    QString qualifiedInfo ;
    VStyle *style = paintInfo->currentStyle() ;
    if(NULL != style) {
        foreach(Attribute * attribute, attributes) {
            IdEntry *id = style->getIdEntry(attribute->name);
            if(NULL != id) {
                if(id->isAlpha()) {
                    qualifiedInfo.append(" '");
                    qualifiedInfo.append(limitLargeTextWithEllipsis(attribute->value));
                    qualifiedInfo.append("'");
                } else {
                    qualifiedInfo.append(" ");
                    qualifiedInfo.append(limitLargeTextWithEllipsis(attribute->value));
                }
            }
        }
    }
    return qualifiedInfo;
}
