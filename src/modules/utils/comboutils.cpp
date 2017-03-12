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
#include "comboutils.h"
#include "utils.h"
#include "qxmleditconfig.h"
#include "applicationdata.h"
#include "qxmleditconfig.h"
#include "modules/services/systemservices.h"
#include <QString>
#include <QSet>

ComboUtils::ComboUtils()
{
    _selection = NULL ;
}

ComboUtils::~ComboUtils()
{
    reset();
}

void ComboUtils::reset()
{
    EMPTYPTRLIST(_items, ComboUtils::ComboItem);
    _items.clear();
    _selection = NULL ;
}

QString ComboUtils::titleForEditor(XmlEditWidget* editor)
{
    QString fileName = editor->getRegola()->fileName();
    if(fileName.isEmpty()) {
        fileName = QObject::tr("<No file>");
    }
    return QObject::tr("Editor: %1").arg(fileName);
}

void ComboUtils::setupItemsForFile(QXmlEditData *data, QList<XmlEditWidget*> editors, const bool useEditors, const bool isSave, const QString &selectedFile, XmlEditWidget *selectedEditor)
{
    reset();
    QStringList preferredDirsNames;
    Config::loadStringArray(Config::KEY_MAIN_PREFDIRS, preferredDirsNames);
    preferredDirsNames.sort();
    _selection = NULL ;
    if(useEditors) {
        if(isSave) {
            ComboItem *comboItem = new ComboItem(QObject::tr("<New Editor>"), TypeOutputEditor);
            _items.append(comboItem);
        } else {
            foreach(XmlEditWidget* editor,  editors) {
                QString text = titleForEditor(editor);
                ComboItem *comboItem = new ComboItem(text, TypeEditor);
                comboItem->data = editor ;
                _items.append(comboItem);
                if(editor == selectedEditor) {
                    _selection = comboItem ;
                }
            }
        }
    }

    _items.append(new ComboItem(QObject::tr("Browse..."), TypeBrowse));
    foreach(QString s, preferredDirsNames) {
        ComboItem *comboItem = new ComboItem(s, TypeBrowse);
        _items.append(comboItem);
        comboItem->dataString = s ;
    }
    if(!preferredDirsNames.isEmpty()) {
        _items.append(new ComboItem("------------", TypeBrowse));
    }
    if(isSave) {
        QSet<QString> foldersSet;
        foreach(QString last, data->lastFiles()) {
            QFile lastFile(last);
            QFileInfo lastFileInfo(lastFile);
            QDir dir = lastFileInfo.dir();
            foldersSet.insert(dir.absolutePath());
        }
        foreach(QString f, foldersSet.values()) {
            ComboItem *comboItem = new ComboItem(f, TypeFile);
            _items.append(comboItem);
            comboItem->dataString = f ;
        }
    } else {
        foreach(const QString & last, data->lastFiles()) {
            ComboItem *comboItem = new ComboItem(last, TypeFile);
            comboItem->dataString = last;
            _items.append(comboItem);
            if(last == selectedFile) {
                _selection = comboItem ;
            }
        }
        if((NULL == _selection) && !selectedFile.isEmpty()) {
            ComboItem *comboItem = new ComboItem(selectedFile, TypeFile);
            comboItem->dataString = selectedFile;
            _items.append(comboItem);
            _selection = comboItem ;
        }
    }
}

void ComboUtils::loadButtonMenu(QToolButton *button, QObject *target, const char *method)
{
    QMenu *menu = new QMenu(button);
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    int index = 0;
    foreach(ComboUtils::ComboItem *item, _items) {
        QAction *action = new QAction(item->text, menu);
        item->action = action ;
        menu->addAction(action);
        action->setData(qVariantFromValue((void*)item));
        if(NULL != target) {
            QObject::connect(action, SIGNAL(triggered()), target, method);
        }
        index ++;
    }
}

void ComboUtils::fireSelection()
{
    if(NULL != _selection) {
        if(NULL != _selection->action) {
            _selection->action->trigger();
        }
    }
}


ComboUtils::ComboItem *ComboUtils::actionData(QAction *action)
{
    if(NULL == action) {
        return NULL ;
    }
    QVariant data = action->data();
    ComboUtils::ComboItem *item = (ComboUtils::ComboItem *)data.value<void*>();
    return item ;
}
