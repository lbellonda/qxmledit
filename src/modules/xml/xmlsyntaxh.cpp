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

#include "xmlsyntaxh.h"
#include "utils.h"
#include "modules/services/colorentry.h"

XMLSyntaxH::XMLSyntaxH(QTextDocument *parent, PaintInfo *paintInfo) : QSyntaxHighlighter(parent)
{
    QColor tagColor = ColorManager::tagColorDefault();
    QColor attributeNameColor = ColorManager::attributeNameColorDefault();
    QColor attributeValueColor = ColorManager::attributeValueColorDefault();
    if(NULL != paintInfo) {
        ColorManager *colorManager = paintInfo->colorManager();
        if(NULL != colorManager) {
            tagColor = colorManager->tags()->color();
            attributeNameColor = colorManager->attributeNames()->color();
            attributeValueColor = colorManager->attributeValues()->color();
        }
    }
    _attributeNameTextFormat.setForeground(attributeNameColor);
    _attributeValueTextFormat.setForeground(attributeValueColor);
    _tagTextFormat.setForeground(tagColor);
    _tagTextFormat.setFontWeight(QFont::Bold);
}

XMLSyntaxH::~XMLSyntaxH()
{
}

void XMLSyntaxH::highlightBlock(const QString &text)
{
    switch(previousBlockState()) {
    default:
    case StateBefore:
        highlightBlockBase(text, true, true);
        break;
    case StateHalfTag:
        highlightBlockBase(text, false, false);
        break;
    case StateTag:
        highlightBlockBase(text, false, false);
        break;
    case StateInStringApic:
        highlightEndString(text, '\'');
        break;
    case StateInStringQuote:
        highlightEndString(text, '"');
        break;
    }
}

void XMLSyntaxH::setStateStringOpen(const QChar &separator)
{
    if(separator == '\'') {
        setCurrentBlockState(StateInStringApic);
    } else {
        setCurrentBlockState(StateInStringQuote);
    }
}

void XMLSyntaxH::scanAttributes(const QString &text, const int startIndex, const int maxIndex)
{
    int index = startIndex;
    // match pairs of attributes, this can be tricky and give wrong results using regex
    const int WatchDog = 1000;
    int loopIndex = 0 ;
    while((index < maxIndex) && (loopIndex < WatchDog)) {
        loopIndex++;
        int lastIndex = index ;
        index = matchUpToEqual(text, index, maxIndex);
        setFormat(lastIndex, index - lastIndex, _attributeNameTextFormat);
        lastIndex = index ;
        QChar separator ;
        bool isInString = false;
        index = matchUpToString(text, index, maxIndex, isInString, separator);
        setFormat(lastIndex, index - lastIndex, _attributeValueTextFormat);
        if(index == maxIndex) {
            if(!isInString) {
                setCurrentBlockState(StateTag);
            } else {
                setStateStringOpen(separator);
            }
        }
    }
}

void XMLSyntaxH::highlightBlockBase(const QString &text, const bool lookForTag, const bool lookForHalfTag)
{
    int index = 0;
    int maxIndex = text.length();
    if(lookForTag) {
        if(lookForHalfTag) {
            // match tag, if any, probably better use a regular expression
            index = skipSpaces(text, index, maxIndex);
            if(index == maxIndex) {
                // no state change
                setCurrentBlockState(StateBefore);
                return ;
            }
            setCurrentBlockState(StateHalfTag);
        }
        index = findNextSpace(text, index, maxIndex);
        setFormat(0, index, _tagTextFormat);
        setCurrentBlockState(StateTag);
        if(index == maxIndex) {
            return ;
        }
    }
    // match pairs of attributes, this can be tricky and give wrong results using regex
    scanAttributes(text, index, maxIndex);
    Utils::TODO_THIS_RELEASE("remove comments");
    /*while(index < maxIndex) {
        int lastIndex = index ;
        index = matchUpToEqual(text, index, maxIndex);
        setFormat(lastIndex, index - lastIndex, _attributeNameTextFormat);
        lastIndex = index ;
        QChar separator ;
        bool isInString = false;
        index = matchUpToString(text, index, maxIndex, isInString, separator);
        setFormat(lastIndex, index - lastIndex, _attributeValueTextFormat);
        if( index == maxIndex ) {
            if(!isInString) {
                setCurrentBlockState(StateTag);
            } else {
                setStateStringOpen(separator);
            }
        }
    }*/
}

void XMLSyntaxH::highlightEndString(const QString &text, const QChar &separator)
{
    int index = 0;
    int maxIndex = text.length();
    // match pairs of attributes, this can be tricky and give wrong results using regex
    bool stringIsOpen = true ;
    index = scanToEndOfToString(text, index, maxIndex, stringIsOpen, separator);
    Utils::TODO_THIS_RELEASE("+-1");
    setFormat(0, index, _attributeValueTextFormat);
    if(stringIsOpen) {
        setStateStringOpen(separator);
        return ;
    }

    // match pairs of attributes, this can be tricky and give wrong results using regex
    scanAttributes(text, index, maxIndex);
    Utils::TODO_THIS_RELEASE("remove comments");
    /*while(index < maxIndex) {
        Utils::TODO_THIS_RELEASE("refactor");
        int lastIndex = index ;
        index = matchUpToEqual(text, index, maxIndex);
        setFormat(lastIndex, index - lastIndex, _attributeNameTextFormat);
        lastIndex = index ;
        QChar separator ;
        bool isInString = false;
        index = matchUpToString(text, index, maxIndex, isInString, separator);
        setFormat(lastIndex, index - lastIndex, _attributeValueTextFormat);
        if( index == maxIndex ) {
            if(!isInString) {
                setCurrentBlockState(StateTag);
            } else {
                setStateStringOpen(separator);
            }
        }
    }*/
}

int XMLSyntaxH::skipSpaces(const QString &text, const int index, const int maxIndex)
{
    for(int i = index ; i < maxIndex ; i ++) {
        QChar ch = text.at(i);
        if(ch.isSpace()) {
            continue;
        }
        return i ;
    }
    return maxIndex ;
}

int XMLSyntaxH::findNextSpace(const QString &text, const int index, const int maxIndex)
{
    for(int i = index ; i < maxIndex ; i ++) {
        QChar ch = text.at(i);
        if(!ch.isSpace()) {
            continue;
        }
        return i ;
    }
    return maxIndex ;
}

int XMLSyntaxH::matchUpToEqual(const QString &text, const int index, const int maxIndex)
{
    for(int i = index ; i < maxIndex ; i ++) {
        QChar ch = text.at(i);
        if(ch != '=') {
            continue;
        }
        return i + 1;
    }
    return maxIndex ;
}

int XMLSyntaxH::matchUpToString(const QString &text, const int index, const int maxIndex, bool &stringIsOpen, QChar &separator)
{
    stringIsOpen = false ;
    separator = '"';
    for(int i = index ; i < maxIndex ; i ++) {
        QChar ch = text.at(i);
        if(!stringIsOpen) {
            if((ch == '\'') || (ch == '"')) {
                stringIsOpen = true ;
                separator = ch ;
            }
        } else {
            if(ch == separator) {
                return i ;
            }
        }
    }
    return maxIndex ;
}

int XMLSyntaxH::scanToEndOfToString(const QString &text, const int index, const int maxIndex, bool &stringIsOpen, const QChar &separator)
{
    stringIsOpen = true ;
    for(int i = index ; i < maxIndex ; i ++) {
        QChar ch = text.at(i);
        if(ch == separator) {
            stringIsOpen = false;
            return i ;
        }
    }
    return maxIndex ;
}
