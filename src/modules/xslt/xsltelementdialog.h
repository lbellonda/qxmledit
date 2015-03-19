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


#ifndef XSLTELEMENTDIALOG_H
#define XSLTELEMENTDIALOG_H

#include <QDialog>

class XsltElement;
class Element;
class XsltHelper;
class CopyAttributesSession;

namespace Ui
{
class XsltElementDialog;
}

class XsltElementDialogParam
{
public:
    XsltElement *xsltElement;
    Element *selectedElement;
    Element *element;
    Element *baseElement;
    XsltHelper *helper;
    QStringList *path;
    QWidget *parent;
    QStringList embeddedPath;
    bool useParams;

    XsltElementDialogParam();
    ~XsltElementDialogParam();
};

class XsltElementDialog : public QDialog
{
    Q_OBJECT

protected:
    bool _endedWithOk;
    bool _isStarted;

    XsltElement *_xsltElement;
    Element *_element;
    QStringList *_path;
    Element *_baseElement;
    XsltHelper *_helper;
    CopyAttributesSession*  _result ;
    bool _hasNames;
    bool _useParam;

public:

#ifdef QXMLEDIT_TEST
    class PrivateTest;
    PrivateTest *d;
#endif

    explicit XsltElementDialog(XsltElement *xsltElement, Element *element, Element *baseElement, XsltHelper *helper, QStringList *path, QWidget *parent);
    ~XsltElementDialog();

    static CopyAttributesSession* dialogProperties(XsltElement *xsltElement, Element *element, Element *baseElement, XsltHelper *helper, QStringList *path, bool *useParam, QWidget *parent);
    static CopyAttributesSession *dialogProperties(XsltElementDialogParam *params);
    CopyAttributesSession* getAndClearResult();
    bool useParam();

private:
    Ui::XsltElementDialog *ui;

    void enableOK();
    void setTarget();
    void setDataForCompletion();
    void fillAttributes();

public slots:
    void accept();

};

#endif // XSLTELEMENTDIALOG_H
