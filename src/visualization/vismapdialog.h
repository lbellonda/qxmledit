/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2016 by Luca Bellonda and individual contributors  *
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


#ifndef VISMAPDIALOG_H
#define VISMAPDIALOG_H

#include "xmlEdit.h"
#include <QDialog>
#include <QSet>
#include "elementbase.h"
#include "colormap.h"
#include "stdcolormap.h"
#include "graycolormap.h"
#include "visdatamap.h"
#include "summarydata.h"
#include "modules/graph/tagnodes.h"
#include "qxmleditdata.h"

class VisDataSax;

namespace Ui
{
class VisMapDialog;
}

class VisMapDialog : public QDialog
{
    Q_OBJECT

    VisDataMap _dataMap;
    ColorMap _colorMap;
    StdColorMap _stdColorMap;
    GrayColorMap _grayColorMap;
    QString _filePath;
    SummaryData _summary;
    QSet<QString> names;
    QString _saveStatsPath;
    ElementBase *_dataRoot;
    QHash<QString, TagNode*> _tagNodes ;
    QXmlEditData *_appData;
    bool _isAutoDelete;

    static void calcSize(ElementBase *e, VisDataMap &dataMap);
public:
    explicit VisMapDialog(QXmlEditData *newData, QWidget *parent = 0, const QString &fileName = "");
    ~VisMapDialog();
    void setAutoDelete();

private:
    Ui::VisMapDialog *ui;

    void loadFile(const QString &fileName);
    void recalc();
    void displayNumbers();
    void newNumbersItem(const QString &label, const QString &data);
    void calcVerticalPosition();
    void calcSlice(const int nSlice);
    ElementBase *getElement(const int x, const int y);
    void newData(ElementBase *newRoot);
    void clearTagNodes();
    // drag and drop
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent * event);
    void loadFileWorkerMethod(VisDataSax *handler, const QString &fileName);

private slots:
    void on_loadFile_clicked();
    void onLoadFile();
    void on_zoom_valueChanged(int value);
    void on_sliceLevel_currentIndexChanged(int index);
    void on_verticalPosition_valueChanged(int value);
    void on_visType_currentIndexChanged(int index);
    void on_cumulative_stateChanged(int state);
    void on_exportStatsCmd_clicked();
    void on_dataWidget_extractFragment(const int fragment, const int depth);
    void on_extractFragment_clicked();
    void on_comboLoudness_currentIndexChanged(int index);
    void on_changeColorMap_clicked();
    void on_threeD_stateChanged(int state);
    void on_cmdViewGraph_clicked();
    void on_checkAnalyzeNodes_stateChanged(int newState);
    void on_cbGrid_stateChanged(int /*state*/);
    void on_cbPoints_stateChanged(int /*state*/);
    void on_copyImageToClipboard_clicked();
#ifdef QXMLEDIT_TEST
    friend class TestVis;
#endif

};

#endif // VISMAPDIALOG_H
