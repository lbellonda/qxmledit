/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef COMPAREMODULE_H
#define COMPAREMODULE_H

#include <QDialog>
#include "libQXmlEdit_global.h"
#include "regola.h"
#include "diffresult.h"
#include "compareengine.h"
#include "comparechrome.h"

namespace Ui
{
class CompareModule;
}


#include <QAbstractTableModel>

class LIBQXMLEDITSHARED_EXPORT DiffModel : public QAbstractTableModel
{
    Q_OBJECT

    enum consts {
        ColumnIcon = 0,
        ColumnType = 1,
        ColumnElem = 2,
        ColumnXPath = 3,
        ColumnNumber = 4
    };

    QList<DiffInfo*> *_info;
    QList<QTreeWidgetItem*> *_items;
    QList<QTreeWidgetItem*> *_ditems;
    CompareChrome _chrome;

public:
    DiffModel(QObject *parent = NULL);
    virtual ~DiffModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QString xpathFromData(DiffInfo* info) const ;
    void setInfo(QList<DiffInfo*> *newInfo, QList<QTreeWidgetItem*> *newItems, QList<QTreeWidgetItem*> *newDItems);
};


class LIBQXMLEDITSHARED_EXPORT CompareModule : public QDialog
{
    Q_OBJECT

    enum Consts {
        MinZoom = 6,
        MaxZoom = 50
    };

protected:
    QString _lastOpenedFilePath;
    bool _useEditorReference;
    bool started;
    bool changing;
    int _zoomSize;
    int _originalItemFontSize;
    QFont _originalFont;
    QFont _itemFont;
    Regola *_regola;
    CompareEngine _engine;
    DiffNodesChangeList *_diffList;
    UIDelegate *_uiDelegate;
    CompareOptions _options;
    QList<QTreeWidgetItem*> _itemsLeft;
    QList<QTreeWidgetItem*> _itemsRight;
    QList<DiffInfo*> _infoList;
    QString _textSynteticView;
    DiffModel _model;

    //-- relative to the current item
    int previousDiff;
    int nextDiff;
    int currentDiff;
    int currentItemPos ;

    //--
    QProgressDialog *_progressDialog;

    //---
    class FileInfo
    {
    public:
        bool isReference;
        QLabel *label;
        QComboBox *combo;
        QPushButton *browse;
        QLabel *info;
        Regola *regola;
        QString filePath;
        QStringList lastFiles;

        FileInfo();
        ~FileInfo();

        void reset();
        void resetUI();
        bool loadRegolaFromFile(const QString &filePath);
        bool isEmpty();
        void setFiles(const QStringList &lastFilesParam);
    };

    FileInfo _referenceFile;
    FileInfo _compareFile;


protected:
    void reset();
    void resetInfo();
    void resetCompare(FileInfo &infoFile);
    void startProgress();
    void endProgress();
    void showResults();
    void resetResults();
    void clearMaps();
    void clearDifferencesList();
    void clearAnalyticView();
    void clearSynteticView();
    void showStatus();
    void showMaps();
    void showDifferencesList();
    void showAnalyticView(DiffNodesChangeList *diffList);
    void showSynteticView(DiffNodesChangeList *diffList);
    void clearResultData();
    void showError(const QString &msg);
    void refineUi();
    void enableUI();
    void setChangeList(DiffNodesChangeList *diffList);
    void setTheCurrentSelectedItem(QTreeWidget *tree, QTreeWidgetItem * current, QList<QTreeWidgetItem*> &sourceList, QList<QTreeWidgetItem*> &targetList);
    void normalizeReference();
    void setupTree(QTreeWidget *tree);
    void gotoPosition(const int position);
    void enableDiff(const int currentPos, const int prevDiffPos, const int nextDiffPos);
    void enableDiffUI();
    void afterItemSelection(QTreeWidgetItem* item, const int itemIndex);
    void afterItemSelection(const int itemIndex);
    void clearDiffIndexes();
    QList<QTreeWidgetItem*> &referenceItems();
    void loadConfig();
    void fillUIFileInfo();
    void enableFileInfoUI();

    // drag and drop
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void loadTestData();

    bool loadFile(FileInfo &fileInfoUI, const QString &filePath);
    void changeSelFromMap(int newValue, QTreeWidget *tree, QList<QTreeWidgetItem*> &targetList);
    void enableZoom();
    void applyZoom();
    void reloadTreeItems(QList<QTreeWidgetItem*> &items, QTreeWidget *tree);
    bool browse();
    bool browse1();

public:

    enum Errors {
        ERR_FILEPATHEMPTY,
        ERR_SAMEFILE,
        ERR_CANTLOAD,
        ERR_UNABLENORMALIZE,
        ERR_TEXTUALREPR
    };

    explicit CompareModule(QWidget *parent, const bool isUseEditorReference, Regola *original, UIDelegate *uiDelegate, QStringList lastFiles);
    virtual ~CompareModule();

    static Regola *loadRegola(const QString &fileName);

    void startCompare(Regola *regola1, Regola *regola2);
    QString textForError(const Errors error);

    void dumpInfo();
    void dumpTreeItems();

protected:
    Ui::CompareModule *ui;
    QTreeWidget *leftTree();
    QTreeWidget *rightTree();

private slots:
    void on_treeLeft_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void on_treeRight_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void on_cmdBrowse_clicked();
    void on_cmdBrowseFile1_clicked();
    void on_comboFiles_activated(const QString & text);
    void on_comboFilesFile1_activated(const QString & text);
    void on_cmdCompare_clicked();
    void on_cmdCopyToClipboard_clicked();
    void on_mapLeft_userChangedSelection(int newValue);
    void on_mapRight_userChangedSelection(int newValue);
    void on_differenceTable_clicked(const QModelIndex & index);
    //--
    void on_gotoFirst_clicked();
    void on_gotoPrev_clicked();
    void on_gotoNext_clicked();
    void on_gotoLast_clicked();
    //--
    void on_chkCompareText_stateChanged(int state);
    void on_chkCompareComments_stateChanged(int state);
    void on_chkDenormalizeEOL_stateChanged(int /*state*/);
    //---
    void on_cmdZoomIn_clicked();
    void on_cmdZoomOut_clicked();
    void on_cmdZoom1_clicked();
    //--
    void on_cmdDump_clicked();
    void startAction();
};

#endif // COMPAREMODULE_H
