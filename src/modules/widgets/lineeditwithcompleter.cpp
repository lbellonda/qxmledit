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

//#define LOG_CONSOLE
#include <xmlEdit.h>
#include "lineeditwithcompleter.h"
#include "utils.h"

#include <QShortcut>
#include <QScrollBar>

LineEditWithCompleter::LineEditWithCompleter(QWidget *parent) :
    QLineEdit(parent)
{
    _completer = NULL ;
    _isCompleting = false;
    _keySequence = NULL ;
    _wordSeparator = "/\\.@![]~{};";
    _wordStart = -1 ;
    _charLimit = DefaultLimit ;
    _keyToActivate = -1 ;
    _fireIfMatch = true ;
    _fireifLimit = false ;
    setup();
}

LineEditWithCompleter::~LineEditWithCompleter()
{
}

void LineEditWithCompleter::setup()
{
    _completer = new QCompleter(this);
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    _completer->setModel(&_namesModel);
    connect(_completer, SIGNAL(activated(const QString &)), this, SLOT(onCompleterFired(const QString &)));
    // do not use setCompleter since non standar behaviour is expected.
    _completer->setWidget(this);
}

void LineEditWithCompleter::setData(QSet<QString> *newData)
{
    QStringList newList;
    if(NULL != newData) {
        newList = newData->toList();
        // do a sort
        qSort(newList.begin(), newList.end(), Utils::caseInsensitiveLessThanCompareFunction);
    }
    _namesModel.setStringList(newList);
}

void LineEditWithCompleter::setData(QSet<QString> newData)
{
    QStringList newList;
    newList = newData.toList();
    // do a sort
    qSort(newList.begin(), newList.end(), Utils::caseInsensitiveLessThanCompareFunction);
    _namesModel.setStringList(newList);
}

QSet<QString> LineEditWithCompleter::data()
{
    QSet<QString>  result;
    QStringList lst = _namesModel.stringList();
    foreach(QString str, lst) {
        result.insert(str);
    }
    return result;
}


void LineEditWithCompleter::autocomplete()
{
    onAutocompleteFunctionActivated();
}

bool LineEditWithCompleter::isCompleterActive()
{
    return _completer->popup()->isVisible();
}

// called by the shortcut
void LineEditWithCompleter::onAutocompleteFunctionActivated()
{
    if(isCompleterActive()) {
        return ;
    }

    // jump to the start of the word
    QString theText = text();
    int currentPosition = cursorPosition();
    _wordStart = currentPosition - 1 ;
    bool first = true;
    if(currentPosition >= theText.length()) {
        currentPosition = theText.length() - 1;
    }
    for(int index = currentPosition ; index >= 0 ; index --) {
        QChar theChar = theText.at(index);
        if(theChar.isSpace() || _wordSeparator.contains(theChar)) {
            break;
        } else {
            if(!first) {
                _wordStart --;
            } else {
                first = false ;
            }
        }
    }
    QString prefix = theText.mid(_wordStart, currentPosition - _wordStart);
    Utils::TODO_NEXT_RELEASE("replace text");
    //if( !prefix.isEmpty() ) {
    activateCompleter(prefix);
    //}
}

// activates the completer
// if one match, insert it, else open the popup
void LineEditWithCompleter::activateCompleter(const QString & prefix)
{
    if(_completer->completionPrefix() != prefix) {
        _completer->setCompletionPrefix(prefix);
        _completer->popup()->setCurrentIndex(_namesModel.index(0, 0));
    }

    QString currentCompletion = _completer->currentCompletion();
    // if a match is given, use it
    int matchedItems = _completer->completionCount();
    if((1 == matchedItems) && _fireIfMatch) {
        onCompleterFired(currentCompletion);
    } else {
        // activates the completer
        QRect bounds = this->cursorRect();
        bounds.setWidth(_completer->popup()->sizeHintForColumn(0) + _completer->popup()->verticalScrollBar()->sizeHint().width());
        _completer->complete(bounds);
        _isCompleting = true ;
    }
}

void LineEditWithCompleter::onCompleterFired(const QString & newText)
{
    // Replace until the end of the current word.
    QString theText = text();
    int len = theText.length();
    int wordEnd = _wordStart ;
    if(_wordStart < 0) {
        _wordStart = 0 ;
    }
    for(int index = _wordStart ; index < len ; index ++) {
        QChar theChar = theText.at(index);
        if(theChar.isSpace() || _wordSeparator.contains(theChar)) {
            break;
        } else {
            wordEnd++;
        }
    }
    QString finalText = theText.left(_wordStart) + newText + theText.mid(wordEnd);
    setText(finalText);
    setCursorPosition(_wordStart + newText.length());
    _isCompleting = false;
}


void LineEditWithCompleter::keyPressEvent(QKeyEvent * event)
{
    TRACEQ(QString("Key event: %1").arg(event->key(), 1, 16));
    if(handleKeyEventForCompleter(event)) {
        TRACEQ(QString("ignored 1"));
        event->ignore();
    } else {
        const bool isCompleterVisible = _completer->popup()->isVisible() ;
        if(!isCompleterVisible && (event->key() ==  Qt::Key_Space) && (event->modifiers() ==  Qt::ControlModifier)) {
            // start handling
            TRACEQ(QString("activated "));
            onAutocompleteFunctionActivated();
            QLineEdit::keyPressEvent(event);
        } else {
            if(!handleKeyEvent(event)) {
                TRACEQ(QString("default keyp"));
                QLineEdit::keyPressEvent(event);
                if(!isCompleterVisible) {
                    if(_fireifLimit) {
                        QString theText = text();
                        int len = theText.length();
                        if(len > _charLimit) {
                            onAutocompleteFunctionActivated();
                        }
                    }
                }
            } else {
                TRACEQ(QString("handled by"));
            }
        }
    }
}


bool LineEditWithCompleter::fireIfMatch() const
{
    return _fireIfMatch;
}

void LineEditWithCompleter::setFireIfMatch(bool fireIfMatch)
{
    _fireIfMatch = fireIfMatch;
}


bool LineEditWithCompleter::fireifLimit() const
{
    return _fireifLimit;
}

void LineEditWithCompleter::setFireifLimit(bool fireifLimit)
{
    _fireifLimit = fireifLimit;
}


int LineEditWithCompleter::charLimit() const
{
    return _charLimit;
}

void LineEditWithCompleter::setCharLimit(int charLimit)
{
    _charLimit = charLimit;
}

bool LineEditWithCompleter::handleKeyEvent(QKeyEvent * event)
{
    if(_completer->popup()->isVisible()) {
        // handled by the completer
        switch(event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            return true;
        case Qt::Key_Escape:
            _completer->popup()->hide();
            return false;
        default:
            //completer->popup()->keyPressEvent(event);
            //event->ignore();
            return false;
        }
    } else {
        return false;
    }
}

bool LineEditWithCompleter::handleKeyEventForCompleter(QKeyEvent * event)
{
    if(_completer->popup()->isVisible()) {
        switch(event->key()) {
        // enter
        case Qt::Key_Return:
        case Qt::Key_Enter:
            return true;
        case Qt::Key_Escape:
            return true;
        default:
            break;
        }
    }
    return false;
}

