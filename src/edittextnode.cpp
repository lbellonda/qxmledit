/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2020 by Luca Bellonda and individual contributors  *
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
#include "edittextnode.h"
#include "qxmleditdata.h"
#include "utils.h"

const int EditTextNode::InputSizeLimit = 1024 * 1024;

EditTextNode::EditTextNode(const bool isBase64Value, const QString &startPath, QWidget * parent) : QDialog(parent)
{
    ui.setupUi(this);
    ui.searchText->installEventFilter(this);
    setSearchVisibility(false);
    Utils::addMaximizeToDialog(this);
    _fileDataPath = startPath;
    target = "";
    connect(ui.wrapText, SIGNAL(stateChanged(int)), this, SLOT(onWrapChanged(int)));
    ui.wrapText->setChecked(true);
    isBase64 = isBase64Value ;
    if(isBase64) {
        ui.fromBase64->setVisible(false);
        ui.fromBase64->setEnabled(false);
        ui.toBase64->setVisible(false);
        ui.toBase64->setEnabled(false);
    }
    _lastSearchFound = false ;
    _lastSearchWrapped = false ;
}

EditTextNode::~EditTextNode()
{
    disconnect(ui.wrapText, SIGNAL(stateChanged(int)), this, SLOT(onWrapChanged(int)));
}

void EditTextNode::setWrapMode(const bool wrap)
{
    ui.editor->setLineWrapMode(wrap ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void EditTextNode::onWrapChanged(int newState)
{
    setWrapMode((Qt::Checked == newState)) ;
}

void EditTextNode::setText(const QString &theText)
{
    target = theText ;
    ui.editor->setPlainText(target);
    if(target.length() >= TextLengthLimitForFullSize) {
        showMaximized();
    }
}

QString EditTextNode::getText() const
{
    return target ;
}

void EditTextNode::accept()
{
    target = ui.editor->toPlainText();
    QDialog::accept();
}

void EditTextNode::error(const QString& message)
{
    Utils::error(this, message);
}


void EditTextNode::on_fromBase64_clicked()
{
    QString text = ui.editor->toPlainText();
    QByteArray array(text.toLatin1());
    QByteArray array2 = QByteArray::fromBase64(array);
    ui.editor->setPlainText(array2.data());
}

void EditTextNode::on_toBase64_clicked()
{
    QByteArray array(ui.editor->toPlainText().toLatin1());
    QByteArray converted = array.toBase64();
    QString strBase64 = converted.data();
    ui.editor->setPlainText(strBase64);
}


void EditTextNode::on_loadFromBinaryFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                       tr("Choose a File to be Transformed in Base 64 Encoded Text"),
                       QXmlEditData::sysFilePathForOperation(_fileDataPath), tr("All files (*);;XML files (*.xml);;XML Schema files (*.xsd)"));
    if(!filePath.isEmpty()) {
        _fileDataPath = filePath ;
        loadFromBinaryFile(filePath);
    }
}

bool EditTextNode::loadFromBinaryFile(const QString &filePath)
{
    bool isError = true ;
    bool isAbort = false ;
    QFile file(filePath);
    QByteArray data ;
    if(file.open(QIODevice::ReadOnly)) {
        qint64 fileSize = file.size();
        if(fileSize > InputSizeLimit) {
            if(!Utils::askYN(this, tr("Warning: the size of the file to import is %1. Do you want to continue?").arg(Utils::getSizeForPresentation(fileSize)))) {
                isAbort = true ;
                isError = false ;
            }
        }
        if(!isAbort) {
            data = file.readAll();
            if(file.error() == QFile::NoError) {
                isError = false ;
            }
        }
        file.close();
        if(isError) {
            Utils::error(tr("Error reading file."));
        } else {
            QByteArray converted = data.toBase64();
            QString strBase64 = converted.data();
            ui.editor->setPlainText(strBase64);
        }
    } else {
        Utils::error(QString(tr("Unable to load file.\nError code is '%1'")).arg(file.error()));
    }
    return !isError ;
}

void EditTextNode::on_saveBase64IntoFile_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Base 64 Coded Data to File"),
                       QXmlEditData::sysFilePathForOperation(_fileDataPath), tr("XML files (*.xml);;XML Schema files (*.xsd);;All files (*)"));
    if(!filePath.isEmpty()) {
        saveToBinaryFile(filePath);
    }
}

void EditTextNode::saveToBinaryFile(const QString &filePath)
{
    bool isError = true ;
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly)) {
        if(saveToBinaryDevice(&file)) {
            isError = false ;
        }
        file.close();
        if(file.error() != QFile::NoError) {
            isError = true ;
        }
        if(isError) {
            Utils::error(tr("Error writing to file."));
        }
    } else {
        Utils::error(QString(tr("Unable to open file.\n Error code is '%1'")).arg(file.error()));
    }
}

bool EditTextNode::saveToBinaryDevice(QIODevice *device)
{
    bool isOk = true ;
    QString text = ui.editor->toPlainText();
    QByteArray array(text.toLatin1());
    QByteArray binaryArray = QByteArray::fromBase64(array);
    if(-1 == device->write(binaryArray)) {
        isOk = false ;
    }
    return isOk;
}

#ifdef QXMLEDIT_TEST
bool EditTextNode::testLoadBinaryFile(const QString &filePath)
{
    return loadFromBinaryFile(filePath);
}
#endif

void EditTextNode::on_searchButton_clicked()
{
    toggleSearchVisibility();
}

void EditTextNode::toggleSearchVisibility()
{
    const bool desiredState = !ui.searchGroup->isVisible();
    setSearchVisibility(desiredState);
}

void EditTextNode::setSearchVisibility(const bool desiredState)
{
    if(desiredState) {
        ui.searchGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    } else {
        ui.searchGroup->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }
    ui.searchGroup->setVisible(desiredState);
    if(desiredState) {
        setSearchStatus("");
        enableSearchUI(false);
        ui.searchText->setFocus();
    }
}

void EditTextNode::on_cmdSearchNext_clicked()
{
    doSearch(true);
}

void EditTextNode::on_cmdSearchPrev_clicked()
{
    doSearch(false);
}

void EditTextNode::doSearch(const bool directionForward)
{
    search(ui.searchText->text(), ui.searchCaseSensitive->isChecked(), ui.searchOptionWholeWords->isChecked(), directionForward);
}

bool EditTextNode::search(const QString &textToSearch, const bool caseSensitive, const bool isWordOnly, const bool directionForward)
{
    //setEnabled(false);
    QTextDocument::FindFlags options = 0;
    if(caseSensitive) {
        options |= QTextDocument::FindCaseSensitively ;
    }
    if(isWordOnly) {
        options |= QTextDocument::FindWholeWords;
    }
    if(!directionForward) {
        options |= QTextDocument::FindBackward;
    }
    bool found = false;
    bool lastSearchWrapped = false ;
    found = ui.editor->find(textToSearch, options);

    if(found) {
        setSearchStatus(tr("found"));
        ui.editor->ensureCursorVisible();
    } else {
        // wrap around
        QTextCursor textCursor = ui.editor->textCursor();
        const int currentPosition = textCursor.position();
        if(directionForward) {
            ui.editor->moveCursor(QTextCursor::Start);
        } else {
            ui.editor->moveCursor(QTextCursor::End);
        }
        found = ui.editor->find(textToSearch, options);
        if(found) {
            lastSearchWrapped = true ;
            setSearchStatus(tr("found - search wrapped"));
        } else {
            textCursor.setPosition(currentPosition);
            setSearchStatus(tr("No match"));
        }
    }
    _lastSearchFound = found ;
    _lastSearchWrapped = lastSearchWrapped ;
    setEnabled(true);
    return found ;
}

void EditTextNode::setSearchStatus(const QString &message)
{
    ui.searchStatus->setText(message);
}

bool EditTextNode::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui.searchText) {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(!keyEvent->isAutoRepeat()
                    && ((keyEvent->key() == Qt::Key_Enter) || (keyEvent->key() == Qt::Key_Return))) {
                if(keyEvent->modifiers()&Qt::ShiftModifier) {
                    ui.cmdSearchPrev->animateClick();
                } else {
                    ui.cmdSearchNext->animateClick();
                }
                return true ;
            }
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

void EditTextNode::on_searchText_textChanged(const QString & text)
{
    setSearchStatus("");
    enableSearchUI(!text.isEmpty());
}

void EditTextNode::enableSearchUI(const bool isEnabled)
{
    ui.cmdSearchNext->setEnabled(isEnabled);
    ui.cmdSearchPrev->setEnabled(isEnabled);
}

void EditTextNode::on_cmdSearchClose_clicked()
{
    setSearchVisibility(false);
}
