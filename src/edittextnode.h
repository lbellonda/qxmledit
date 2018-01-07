/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#ifndef EDIT_TEXTNODE_H
#define EDIT_TEXTNODE_H

#include <QtGui>

#include <QDialog>
#include "ui_EditTextNode.h"

class EditTextNode : public QDialog
{
    Q_OBJECT

    static const int InputSizeLimit;
    QString target;
    QString _fileDataPath;

public:
    EditTextNode(const bool isBase64Value, const QString &startPath, QWidget * parent = NULL);
    ~EditTextNode();

    void setText(const QString &pTarget);
    QString getText() const ;

    bool testLoadBinaryFile(const QString &filePath);

public slots:
    void accept();

private slots:
    void on_fromBase64_clicked();
    void on_toBase64_clicked();
    void onWrapChanged(int newState);
    void on_loadFromBinaryFile_clicked();
    void on_saveBase64IntoFile_clicked();

private:

    void setWrapMode(const bool show);
    void error(const QString& message);
    void saveToBinaryFile(const QString &filePath);

public:
    bool saveToBinaryDevice(QIODevice *device);
    bool loadFromBinaryFile(const QString &filePath);

private:

    Ui::EditTN ui;
    bool    isBase64;
};

#endif
