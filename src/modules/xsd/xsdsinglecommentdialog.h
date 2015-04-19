/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef XSDSINGLECOMMENTDIALOG_H
#define XSDSINGLECOMMENTDIALOG_H

#include <QDialog>
#include "xsdeditor/xschema.h"

namespace Ui
{
class XSDSingleCommentDialog;
}

class XSDSingleCommentDialog : public QDialog
{
    Q_OBJECT

    bool _isCallExtended;
    XInfoBase *_origModel;
    /**
     * @brief _textChanged: to know if refresh the XML view
     */
    bool _textChanged ;
    /**
     * @brief _wasOpenedAsStarting: if the dialog was invoked as first option in edit annotations
     */
    bool _wasOpenedAsStarting;
    bool _wasAccepted ;

    bool doAccept();
    bool isXML();
    void activateButtons();

public:
    explicit XSDSingleCommentDialog(const bool argOpenedAsStarting, XInfoBase *newModel, QWidget *parent = 0);
    virtual ~XSDSingleCommentDialog();

    virtual bool isCallExtended();
    virtual XInfoBase* createModel();
    void loadLangCodes();

public slots:
    void accept();

private:
    Ui::XSDSingleCommentDialog *ui;

private slots:
    void on_cmdAdvanced_clicked();
    void on_textValue_modificationChanged(bool changed);
    void on_textValue_textChanged();
    void on_textValue_undoAvailable(bool available);
    void on_tabWidget_currentChanged(int index);
    void on_typeAppInfo_clicked();
    void on_typeDoc_clicked();
};

#endif // XSDSINGLECOMMENTDIALOG_H
