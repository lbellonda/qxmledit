/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2016 by Luca Bellonda and individual contributors  *
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


#ifndef NODESRELATIONSDIALOG_H
#define NODESRELATIONSDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QTimer>

#include "libQXmlEdit_global.h"
#include "tagnodes.h"

#include <QGraphicsEllipseItem>
#include <QVector2D>



//----------------------------------------------------------------------------------------------------------------

#include "tagmarker.h"
#include "tagspring.h"
#include "nodesrelationscontroller.h"
#include "visualization/attributessummarydata.h"

//----------------------------------------------------------------------------------------------------------------

class AttributeSummaryData;

namespace Ui
{
class NodesRelationsDialog;
}


class LIBQXMLEDITSHARED_EXPORT NodesRelationsDialog : public QDialog
{
    Q_OBJECT

    static const int TimerInterval = 100 ;

    bool canLoadData;
    bool isTest ;
    bool started ;
    int timerInterval ;
    QList<TagNode*> nodes;
    QGraphicsScene _scene;
    QTimer shotTimer;
    bool timerActive;
    NodesRelationsController  controller;
    QString inputFileName;
    QString _saveStatsPath;
    QString _exportCSVAttrs ;
    AttributesSummaryData _localAttributesSummaryData;
    AttributesSummaryData *_attributesSummaryData;
    //--
    void resetData();
    void feedNewData(QList<TagNode*> &dataList, AttributesSummaryData *attributeSummaryData);
    void loadAttributesList(const bool isWhitelist);
    bool innerLoadAttributesList(const QString &filePath, const bool isWhitelist);
    bool loadFileAttributeList(const QString &filePath, const bool isWhitelist, AttributesSummaryData *attributeSummaryData);
    bool resetAttributeLists();
    void exportAttributesCSV();
    bool exportAttributesCSVOnDevice(QIODevice &ioDevice);
    void updateEnableAttributeLists();
public:
    explicit NodesRelationsDialog(const bool canLoadData, QList<TagNode*> &dataList, AttributesSummaryData *attributesSummaryData, QWidget *parent = 0);
    ~NodesRelationsDialog();

    QGraphicsScene *scene();
    bool loadNodesFromFile(QIODevice *inputDevice, const QString &newInputFileName);
    bool loadFile(const QString &filePath);
    NodesRelationsController *getController();
    QTableWidget *getTableWidget();
    void saveStatisticsToStream(QTextStream &outStream);
    AttributesSummaryData *attributesSummaryData();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Ui::NodesRelationsDialog *ui;

private slots:
    void timedUpdatePositions();
    void on_cmdZoom1_clicked();
    void on_cmdZoomIn_clicked();
    void on_cmdZoomOut_clicked();
    void startTimer();
    void on_cmdReset_clicked();
    void on_forceSlider_valueChanged();
    void on_radiusSlider_valueChanged();
    void on_stiffnessSlider_valueChanged();
    void on_dampingSlider_valueChanged();
    void on_springLengthSlider_valueChanged();
    void on_cmdBrowseFile_clicked();
    void on_chkViewOnlySelected_clicked();
    void onExportCmd();
    void on_cmdLoadBlack_clicked();
    void on_cmdLoadWhitelist_clicked();
    void on_exportCSV_clicked();
    void on_cmdResetLists_clicked();

#ifdef QXMLEDIT_TEST
    friend class TestVis;
#endif
};



#endif // NODESRELATIONSDIALOG_H
