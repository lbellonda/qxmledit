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


#ifndef SEARCHRESULTSDIALOG_H
#define SEARCHRESULTSDIALOG_H

#include <QDialog>

#include "regola.h"
#include "qxmleditdata.h"
#include "xmlwidgetutilities.h"

namespace Ui
{
class SearchResultsDialog;
}

class SearchResultsDialog : public QDialog
{
    Q_OBJECT


    Regola *_regola;
    PaintInfo *_paintInfo;
    QXmlEditData *_appData;
    Element *_currentSelection;
    QList<Element*> _results;
    XmlWidgetUtilities _helper;

    void setXQueryResults(QList<Element*> &nodes);
    void finishSetup(FindTextParams* settings);
    PaintInfo* getPaintInfo();
    bool setUpDelegates();


    void findTextOperation(const bool isFindOrCount);

public:
    explicit SearchResultsDialog(QWidget *parent, Regola *regola, Element *currentSelection, QXmlEditData *appData, const QString &curText, FindTextParams* settings);
    ~SearchResultsDialog();

    static void go(QWidget *parent, QXmlEditData *appData, QList<Element*> &items, Regola *regola, Element *currentSelection, const QString &curText, FindTextParams* settings);
private:
    Ui::SearchResultsDialog *ui;

private slots:
    void findText();
    void countTextOccurrences();
    void treeContextMenu(const QPoint& position);
    void on_copyAction_triggered();
protected:
    bool eventFilter(QObject * obj, QEvent * event);
};

#endif // SEARCHRESULTSDIALOG_H
