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

#include "xmlEdit.h"
#include "comparemodule.h"
#include "ui_comparemodule.h"
#include "utils.h"
#include "qxmleditconfig.h"
#include "comparesidebysideview.h"
#include "compareresulttextformat.h"
#include "qxmleditdata.h"

//------------------------------------------------------------

DiffModel::DiffModel(QObject *parent) : QAbstractTableModel(parent)
{
    _info = NULL ;
    _items = NULL ;
}

DiffModel::~DiffModel()
{
}

int DiffModel::rowCount(const QModelIndex & /*parent*/) const
{
    return _info->size();
}

int DiffModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnNumber;
}

QVariant DiffModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= _info->size())) {
        return QVariant();
    }

    DiffInfo *info = _info->at(index.row());
    switch(index.column()) {
    case 0:
        if(Qt::DecorationRole == role) {
            switch(info->diffType) {
            case EDiff::ED_ADDED:
                return _chrome.addedIcon();
            case EDiff::ED_MODIFIED:
                return _chrome.modIcon();
            case EDiff::ED_DELETED:
                return _chrome.deletedIcon();
            default:
                break;
            }
        } else if(Qt::TextAlignmentRole == role) {
            return QVariant(Qt::AlignCenter);
        }
        break;
    case 1:
        if(Qt::DisplayRole == role) {
            switch(info->diffType) {
            case EDiff::ED_ADDED:
                return tr("Added");
            case EDiff::ED_MODIFIED:
                return tr("Modified");
            case EDiff::ED_DELETED:
                return tr("Deleted");
            default:
                break;
            }
        } else if(Qt::BackgroundRole == role) {
            switch(info->diffType) {
            case EDiff::ED_ADDED:
                return QBrush(_chrome.addedColor());
            case EDiff::ED_MODIFIED:
                return QBrush(_chrome.modifiedColor());
            case EDiff::ED_DELETED:
                return QBrush(_chrome.deletedColor());
            default:
                break;
            }
        }
        break;
    case 2:
        if(Qt::DisplayRole == role) {
            if(info->isElementOrAttribute) {
                return tr("Element");
            } else {
                return tr("Attribute");
            }
        }
        break;
    case 3:
        if(Qt::DisplayRole == role) {
            return xpathFromData(info);
        }
        break;
    default:
        return QVariant();
    }

    return QVariant();
}

QVariant DiffModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant();
    }
    if(Qt::Vertical == orientation) {
        return QVariant(section + 1);
    } else {
        switch(section) {
        case ColumnIcon:
            return tr("Icon");
        case ColumnType:
            return tr("Status");
        case ColumnElem:
            return tr("Element");
        case ColumnXPath:
            return tr("XPath");
        }
    }
    return QVariant();
}

void DiffModel::setInfo(QList<DiffInfo*> *newInfo, QList<QTreeWidgetItem*> *newItems, QList<QTreeWidgetItem*> *newDItems)
{
    _items = newItems;
    _ditems = newDItems;
    _info = newInfo;
}

QString DiffModel::xpathFromData(DiffInfo* info) const
{
    QString result;
    QTreeWidgetItem *item = NULL ;
    if(EDiff::ED_DELETED != info->diffType) {
        item = _items->at(info->itemPosition);
    } else {
        item = _ditems->at(info->itemPosition);
    }
    int level = 1;
    while(NULL != item) {
        QString local;
        if(!info->isElementOrAttribute && (level == 1)) {
            local = "/@";
        }
        local += item->text(0);
        QTreeWidgetItem *itemParent = item->parent();
        int pos = 0 ;
        if(NULL != itemParent) {
            pos = itemParent->indexOfChild(item);
        }
        if((pos > 1) && info->isElementOrAttribute) {
            result = QString("/[%2]%1%3").arg(local).arg(pos).arg(result);
        } else {
            result = QString("/%1%2").arg(local).arg(result);
        }
        item = itemParent;
        level ++;
    }
    return result;
}

//------------------------------------------------------------

CompareModule::FileInfo::FileInfo()
{
    isReference = false;
    label = NULL ;
    combo = NULL ;
    browse = NULL ;
    info = NULL ;
    regola = NULL ;
}

CompareModule::FileInfo::~FileInfo()
{
    reset();
}

void CompareModule::FileInfo::reset()
{
    if(NULL != regola) {
        delete regola ;
        regola = NULL ;
    }
    filePath = "" ;
}

void CompareModule::FileInfo::resetUI()
{
    filePath = "" ;
    combo->setCurrentIndex(-1);
    info->setToolTip("");
    info->setVisible(false);
}

bool CompareModule::FileInfo::loadRegolaFromFile(const QString &newFilePath)
{
    filePath = newFilePath;
    regola = CompareModule::loadRegola(filePath);
    if(NULL == regola) {
        return false;
    }
    return true;
}

bool CompareModule::FileInfo::isEmpty()
{
    bool empty = (NULL == regola);
    return empty;
}

void CompareModule::FileInfo::setFiles(const QStringList &lastFilesParam)
{
    foreach(QString file, lastFilesParam) {
        lastFiles.append(file);
    }
    lastFiles.insert(0, "");
}


//------------------------------------------------------------


CompareModule::CompareModule(QWidget *parent, const bool isUseEditorReference, Regola *original, UIDelegate *newUiDelegate, QStringList lastFilesParam) :
    QDialog(parent),
    ui(new Ui::CompareModule)
{
    _useEditorReference = isUseEditorReference ;
    started = false ;
    changing = false ;
    _zoomSize = 8;
    Utils::addMaximizeToDialog(this);

    _uiDelegate = newUiDelegate ;
    _regola = original;
    _diffList = new DiffNodesChangeList();
    loadConfig();
    ui->setupUi(this);
    _compareFile.setFiles(lastFilesParam);
    _referenceFile.setFiles(lastFilesParam);

    _compareFile.reset();
    _referenceFile.reset();
    refineUi();
    normalizeReference();
    started = true ;
    QString fileName = original->fileName();
    _lastOpenedFilePath = fileName;
    QString title ;
    if(_useEditorReference) {
        if(!fileName.isEmpty()) {
            title = QString(tr("Comparing '%1' with")).arg(fileName);
        } else {
            title = tr("Comparing current data with");
        }
    } else {
        title = tr("File Compare");
    }
    setWindowTitle(title);
}

CompareModule::~CompareModule()
{
    ui->differenceTable->setModel(NULL);
    reset();
    resetCompare(_referenceFile);
    resetCompare(_compareFile);
    delete ui;
}

void CompareModule::setChangeList(DiffNodesChangeList *newDiffList)
{
    reset();
    _diffList = newDiffList;
}

void CompareModule::reset()
{
    if(NULL != _diffList) {
        delete _diffList;
        _diffList = NULL ;
    }
    resetInfo();
}

void CompareModule::resetInfo()
{
    foreach(DiffInfo * info, _infoList) {
        delete info ;
    }
    _infoList.clear();
}

void CompareModule::resetCompare(FileInfo &infoFile)
{
    infoFile.reset();
    infoFile.resetUI();
    clearDiffIndexes();
    resetInfo();
}

QTreeWidget *CompareModule::leftTree()
{
    return ui->treeLeft;
}
QTreeWidget *CompareModule::rightTree()
{
    return ui->treeRight;
}

void CompareModule::setupTree(QTreeWidget *tree)
{
    tree->setColumnCount(1);
    tree->setHeaderLabel(tr("Nodes"));
    tree->clear();
    tree->setUniformRowHeights(true);
    //tree->setColumnWidth(0, tree->width());
}

void CompareModule::clearDiffIndexes()
{
    previousDiff = -1 ;
    nextDiff = -1 ;
    currentDiff = -1 ;
    currentItemPos = -1 ;
}

void CompareModule::refineUi()
{
    fillUIFileInfo();

    _referenceFile.resetUI();
    _compareFile.resetUI();

    enableFileInfoUI();
    clearDiffIndexes();
    setupTree(ui->treeLeft);
    setupTree(ui->treeRight);
    enableUI();
    ui->comboFiles->insertItems(0, _compareFile.lastFiles);
    if(!_useEditorReference) {
        ui->comboFilesFile1->insertItems(0, _referenceFile.lastFiles);
    }
    setAcceptDrops(true);
    CompareChrome chrome;
    Utils::setBkColorAndBorderToWidget(ui->colorAdded, chrome.addedColor());
    Utils::setBkColorAndBorderToWidget(ui->colorModified, chrome.modifiedColor());
    Utils::setBkColorAndBorderToWidget(ui->colorDeleted, chrome.deletedColor());
    Utils::setBkColorAndBorderToWidget(ui->colorEquals, chrome.equalsColor());
    this->setWindowState(Qt::WindowMaximized);

    ui->chkCompareText->setChecked(_options.isCompareText());
    ui->chkCompareComments->setChecked(_options.isCompareComments());
    ui->chkDenormalizeEOL->setChecked(_options.isDenormalizeEOL());

    connect(ui->treeLeft->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->treeRight->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->treeRight->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->treeLeft->verticalScrollBar(), SLOT(setValue(int)));

    connect(ui->treeLeft->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->treeRight->verticalScrollBar(), SLOT(setValue(int)));
    connect(ui->treeRight->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->treeLeft->verticalScrollBar(), SLOT(setValue(int)));


    _originalFont = ui->treeLeft->font();
    _itemFont = ui->treeLeft->font();
    _originalItemFontSize = _itemFont.pointSize();
    _zoomSize = _originalItemFontSize;

    enableZoom();

}

void CompareModule::fillUIFileInfo()
{
    _referenceFile.isReference = true ;
    _referenceFile.label = ui->labelCompareFile1;
    _referenceFile.combo = ui->comboFilesFile1;
    _referenceFile.info = ui->infoLabelFile1;
    _referenceFile.browse = ui->cmdBrowseFile1;

    _compareFile.isReference = false ;
    _compareFile.label = ui->label;
    _compareFile.combo = ui->comboFiles;
    _compareFile.info = ui->infoLabel;
    _compareFile.browse = ui->cmdBrowse;
}

void CompareModule::enableFileInfoUI()
{
    if(_useEditorReference) {
        ui->labelCompareFile1->setVisible(false);
        ui->comboFilesFile1->setVisible(false);
        ui->infoLabelFile1->setVisible(false);
        ui->cmdBrowseFile1->setVisible(false);
        //ui->comboFilesFile1->setAcceptDrops(true);
    }
    //ui->comboFiles->setAcceptDrops(true);
}


//------------------------- region(file management) ---
void CompareModule::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }
}

void CompareModule::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")) {
        QString filePath = "" ;
        event->acceptProposedAction();
        if(event->mimeData()->hasUrls()) {
            foreach(QUrl url, event->mimeData()->urls()) {
                filePath = url.toLocalFile();
                break;
            }
        }
        if(filePath.length() > 0) {
            if(_useEditorReference) {
                if(!loadFile(_compareFile, filePath)) {
                    event->ignore();
                    return ;
                }
            } else {
                bool handled = false;
                if(_referenceFile.isEmpty()) {
                    handled = loadFile(_referenceFile, filePath);
                } else {
                    handled = loadFile(_compareFile, filePath);
                }
                if(!handled) {
                    event->ignore();
                    return ;
                }
            }
        }
        event->acceptProposedAction();
    }
}

void CompareModule::on_cmdBrowse_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Reference File"),
                       QXmlEditData::sysFilePathForOperation(_lastOpenedFilePath), Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        loadFile(_compareFile, filePath);
    }
}

void CompareModule::on_cmdBrowseFile1_clicked()
{
    if(_useEditorReference) {
        return ;
    }
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File to Compare"),
                       QXmlEditData::sysFilePathForOperation(_lastOpenedFilePath), Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        loadFile(_referenceFile, filePath);
    }
}

void CompareModule::on_comboFiles_activated(const QString & text)
{
    loadFile(_compareFile, text);
}

void CompareModule::on_comboFilesFile1_activated(const QString & text)
{
    if(_useEditorReference) {
        return ;
    }
    loadFile(_referenceFile, text);
}

QString CompareModule::textForError(const Errors error)
{
    switch(error) {
    default:
        return tr("Unexpected error");
        break;
    case ERR_FILEPATHEMPTY:
        return tr("The file name is empty.");
        break;
    case ERR_SAMEFILE:
        return tr("Cannot compare a file with itself.");
        break;
    case ERR_CANTLOAD:
        return tr("Error loading the file.");
        break;
    case ERR_UNABLENORMALIZE:
        return tr("Error manipulating internal data.");
        break;
    }

}

bool CompareModule::loadFile(FileInfo &fileInfoUI, const QString &filePath)
{
    Q_ASSERT(NULL != _regola);
    if(filePath.isEmpty()) {
        _uiDelegate->error(this, textForError(ERR_FILEPATHEMPTY));
        return false ;
    }
    _lastOpenedFilePath = filePath;
    if(_regola->fileName() == filePath) {
        _uiDelegate->error(this, textForError(ERR_SAMEFILE));
        return false ;
    }
    //--- set data
    resetResults();
    resetCompare(fileInfoUI);
    enableUI();
    if(!fileInfoUI.loadRegolaFromFile(filePath)) {
        _uiDelegate->error(this, textForError(ERR_CANTLOAD));
        return false;
    }
    if(!fileInfoUI.lastFiles.contains(filePath)) {
        fileInfoUI.lastFiles.insert(1, filePath);
        fileInfoUI.combo->clear();
        fileInfoUI.combo->insertItems(0, fileInfoUI.lastFiles);
    }
    int theIndex = fileInfoUI.lastFiles.indexOf(filePath);
    fileInfoUI.combo->setCurrentIndex(theIndex);
    //-- make tooltip
    QFileInfo regolaFileInfo(filePath);
    qint64 fileSize = regolaFileInfo.size();
    QDateTime dtLastMod = regolaFileInfo.lastModified();
    QString dateMod = dtLastMod.toString(Qt::DefaultLocaleLongDate);
    QString dateAccessed = regolaFileInfo.lastRead().toString(Qt::DefaultLocaleLongDate);
    QString toolTip = tr("%1\n  last accessed on %2\n  last updated on %3\n  size %4").arg(filePath).arg(dateAccessed).arg(dateMod).arg(Utils::getSizeForPresentation(fileSize));
    fileInfoUI.info->setToolTip(toolTip);
    fileInfoUI.info->setVisible(true);

    if(_useEditorReference) {
        on_cmdCompare_clicked();
    }
    enableUI();
    return true ;
}


//------------------------- endregion(file management) ---

void CompareModule::startProgress()
{
    _progressDialog = new QProgressDialog(tr("Compare in progress..."), QString(), 0, 100, this);
    if(NULL != _progressDialog) {
        _progressDialog->setAutoClose(false);
        _progressDialog->setAutoReset(true);
        _progressDialog->setValue(10);
    }
}

void CompareModule::endProgress()
{
    if(NULL != _progressDialog) {
        delete _progressDialog ;
        _progressDialog = NULL;
    }
}


void CompareModule::on_treeLeft_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * /*previous*/)
{
    setTheCurrentSelectedItem(ui->treeRight, current, _itemsLeft, _itemsRight);
}

void CompareModule::on_treeRight_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * /*previous*/)
{
    setTheCurrentSelectedItem(ui->treeLeft, current, _itemsRight, _itemsLeft);
}

// TODO: A double hash table will be better for performance.
void CompareModule::setTheCurrentSelectedItem(QTreeWidget *tree, QTreeWidgetItem * current, QList<QTreeWidgetItem*> &sourceList, QList<QTreeWidgetItem*> &targetList)
{
    if(changing) {
        return ;
    }
    changing = true;
    if(NULL != current) {
        int index = sourceList.indexOf(current);
        if(-1 != index) {
            QTreeWidgetItem *target = targetList.at(index);
            if(tree->currentItem() != target) {
                tree->setCurrentItem(target);
                ui->mapLeft->setSelection(index);
                ui->mapRight->setSelection(index);
                afterItemSelection(index);
                ui->differenceTable->selectRow(currentDiff);
                enableDiffUI();
            }
        }
    }
    changing = false;
}

void CompareModule::on_differenceTable_clicked(const QModelIndex & index)
{
    int currentIndex = -1 ;
    if(index.isValid()) {
        currentIndex = index.row();
    }
    QTreeWidgetItem * currentItem = NULL ;
    if((currentIndex != -1) && (_infoList.size() > currentIndex)) {
        DiffInfo *info = _infoList.at(currentIndex);
        if(NULL != info) {
            currentItem = _itemsLeft.at(info->itemPosition);
        }
    }
    ui->treeLeft->setCurrentItem(currentItem);
}

void CompareModule::on_cmdCompare_clicked()
{
    Q_ASSERT(NULL != _referenceFile.regola);
    Q_ASSERT(NULL != _compareFile.regola);
    startCompare(_referenceFile.regola, _compareFile.regola);
}

void CompareModule::startCompare(Regola *regola1, Regola *regola2)
{
    setEnabled(false);
    Utils::showWaitCursor();
    ui->statusLabel->setText(tr("Comparing..."));
    ui->statusLabel->update();
    ui->optionsLabel->setText(tr("Comparing..."));
    ui->optionsLabel->update();
    startProgress();
    //---
    resetResults();
    _diffList->reset();
    OperationResult results;
    _engine.doCompare(&results, regola1, regola2, _diffList, _options);
    //---
    endProgress();
    if(!results.isOk()) {
        showError(tr("Compare operation error: '%1'").arg(results.message()));
    } else {
        ui->treeLeft->setHeaderLabel(regola1->fileName());
        ui->treeRight->setHeaderLabel(regola2->fileName());
        showResults();
    }
    setEnabled(true);
    Utils::restoreCursor();
}

void CompareModule::showError(const QString &msg)
{
    _uiDelegate->error(this, msg);
}

void CompareModule::showResults()
{
    showStatus();
    showMaps();
    showAnalyticView(_diffList);
    showSynteticView(_diffList);
    showDifferencesList();
}

void CompareModule::resetResults()
{
    clearResultData();
    _diffList->reset();
}

void CompareModule::clearResultData()
{
    ui->statusLabel->setText(tr("No compare results."));
    clearMaps();
    clearDifferencesList();
    clearAnalyticView();
    clearSynteticView();
}

void CompareModule::clearMaps()
{
    ui->mapLeft->setDiffList(NULL);
    ui->mapRight->setDiffList(NULL);
}

void CompareModule::showMaps()
{
    ui->mapLeft->setDiffList(&_itemsLeft);
    ui->mapRight->setDiffList(&_itemsRight);
}

void CompareModule::clearDifferencesList()
{
    ui->differenceTable->setModel(NULL);
    resetInfo();
}

void CompareModule::clearAnalyticView()
{
    _itemsLeft.clear();
    _itemsRight.clear();
    ui->treeLeft->clear();
    ui->treeRight->clear();
}

void CompareModule::clearSynteticView()
{
    _textSynteticView = "" ;
    ui->textBrowser->clear();
}

void CompareModule::showStatus()
{
    if(_diffList->isReferenceEqualToCompare()) {
        ui->statusLabel->setText(tr("Files are equal."));
    } else {
        ui->statusLabel->setText(tr("Files are different."));
    }

    QString optionsText = QString("%1 %2 %3")
                          .arg(_options.isCompareText() ? "" : tr("no text"))
                          .arg(_options.isCompareComments() ? "" : tr("no comments"))
                          .arg(_options.isDenormalizeEOL() ? tr("denorm. EOL") : "");

    ui->optionsLabel->setText(optionsText);
}


void CompareModule::dumpInfo()
{
    foreach(DiffInfo * info, _infoList) {
        printf("Info %d, %d %s\n", info->id, info->itemPosition, info->isElementOrAttribute ? "el" : "a");
    }
    printf("\n\n");
    fflush(stdout);
}

void CompareModule::dumpTreeItems()
{
    int index = 0;
    foreach(QTreeWidgetItem * item, _itemsLeft) {

        int lastDiffIndex = CompareSideBySideView::lastDiffIndexFromItem(item);
        int currDiffIndex = CompareSideBySideView::thisDiffIndexFromItem(item);

        QString s = QString("Item %1 prev:%2 curr:%3 text='%4'").arg(index).arg(lastDiffIndex).arg(currDiffIndex).arg(item->text(0));
        printf("%s\n", s.toLatin1().data());
        index++;
    }
    printf("\n\n");
    fflush(stdout);
}


void CompareModule::on_cmdDump_clicked()
{
    dumpInfo();
    dumpTreeItems();
}

void CompareModule::showDifferencesList()
{
    _model.setInfo(&_infoList, &_itemsLeft, &_itemsRight);
    ui->differenceTable->setModel(&_model);
    ui->differenceTable->resizeColumnsToContents();
}

void CompareModule::showAnalyticView(DiffNodesChangeList *diffList)
{
    ui->treeLeft->setUpdatesEnabled(false);
    ui->treeRight->setUpdatesEnabled(false);

    CompareSideBySideView viewItemBuilder;
    if(_zoomSize != _originalItemFontSize) {
        viewItemBuilder.setItemsFonts(_itemFont);
    }
    viewItemBuilder.loadSinglePart(diffList->rootLevel(), ui->treeLeft, true, _itemsLeft, _infoList);
    QList<DiffInfo*> nullList;
    viewItemBuilder.loadSinglePart(diffList->rootLevel(), ui->treeRight, false, _itemsRight, nullList);
    Q_ASSERT(nullList.isEmpty());
    ui->treeLeft->expandAll();
    ui->treeRight->expandAll();
    ui->treeLeft->setUpdatesEnabled(true);
    ui->treeRight->setUpdatesEnabled(true);
}

void CompareModule::showSynteticView(DiffNodesChangeList *diffList)
{
    OperationResult result;
    CompareResultTextFormat formatter;
    _textSynteticView = formatter.formatText(result, diffList);

    if(result.isError()) {
        showError(result.message());
        ui->textBrowser->setText(textForError(ERR_TEXTUALREPR));
    } else {
        ui->textBrowser->setHtml(_textSynteticView);
    }
}


Regola *CompareModule::loadRegola(const QString &fileName)
{
    Regola *regola = NULL ;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        QString errorMsg ;
        int errorLine = 0, errorColumn = 0;
        if(document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
            regola = new Regola(document, fileName, true);
        }
        file.close();
    }
    return regola ;
}


void CompareModule::loadTestData()
{
    Regola *regola1 = loadRegola("../test/data/compare/reference1.xml");
    Regola *regola2 = loadRegola("../test/data/compare/compare1.xml");
    if((NULL != regola1) && (NULL != regola2)) {
        startCompare(regola1, regola2);
    }
    delete regola1;
    delete regola2;
}

void CompareModule::normalizeReference()
{
    if(NULL != _regola) {
        if(_regola->hasChildren()) {
            QByteArray resultData = _regola->writeMemory();
            QBuffer outputData(&resultData);
            QDomDocument document;
            QString errorMsg ;
            int errorLine = 0, errorColumn = 0;
            if(document.setContent(&outputData, &errorMsg, &errorLine, &errorColumn)) {
                _referenceFile.regola = new Regola(document, _regola->fileName(), true);
            }
        } else {
            _referenceFile.regola = new Regola();
        }
    }
    if(NULL == _referenceFile.regola) {
        Utils::error(this, textForError(ERR_UNABLENORMALIZE));
        close();
    }
}

void CompareModule::enableUI()
{
    bool isEnabledCompare = !_referenceFile.isEmpty() && !_compareFile.isEmpty();
    ui->cmdCompare->setEnabled(isEnabledCompare);
    enableDiffUI();
}

void CompareModule::on_cmdCopyToClipboard_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(_textSynteticView);
}

void CompareModule::changeSelFromMap(int newValue, QTreeWidget *tree, QList<QTreeWidgetItem*> &targetList)
{
    if(-1 != newValue) {
        QTreeWidgetItem *target = targetList.at(newValue);
        if(tree->currentItem() != target) {
            tree->setCurrentItem(target);
        }
    }
}

void CompareModule::on_mapLeft_userChangedSelection(int newValue)
{
    changeSelFromMap(newValue, ui->treeLeft, _itemsLeft);
}

void CompareModule::on_mapRight_userChangedSelection(int newValue)
{
    changeSelFromMap(newValue, ui->treeRight, _itemsRight);
}

void CompareModule::gotoPosition(const int position)
{
    if((position < 0) || (position >= _infoList.size())) {
        return ;
    }
    DiffInfo *info = _infoList.at(position);
    QTreeWidgetItem *item  = _itemsLeft.at(info->itemPosition);
    ui->treeLeft->setCurrentItem(item);
}

void CompareModule::on_gotoFirst_clicked()
{
    gotoPosition(0);
}

void CompareModule::on_gotoPrev_clicked()
{
    gotoPosition(previousDiff);
}

void CompareModule::on_gotoNext_clicked()
{
    gotoPosition(nextDiff);
}

void CompareModule::on_gotoLast_clicked()
{
    gotoPosition(_infoList.size() - 1);
}

QList<QTreeWidgetItem*> &CompareModule::referenceItems()
{
    return _itemsLeft;
}

void CompareModule::afterItemSelection(const int itemIndex)
{
    QTreeWidgetItem* item = referenceItems().at(itemIndex);
    afterItemSelection(item, itemIndex);
}

void CompareModule::afterItemSelection(QTreeWidgetItem* item, const int itemIndex)
{
    currentItemPos = itemIndex;
    previousDiff = CompareSideBySideView::lastDiffIndexFromItem(item);
    currentDiff = CompareSideBySideView::thisDiffIndexFromItem(item);
    nextDiff = previousDiff + 1;
    if(currentDiff >= 0) {
        nextDiff = currentDiff + 1;
    } else {
        nextDiff = previousDiff + 1;
    }
    if(nextDiff >= _infoList.size()) {
        nextDiff = -1 ;
    }
}

void CompareModule::enableDiffUI()
{
    enableDiff(currentDiff, previousDiff, nextDiff);
}


void CompareModule::enableDiff(const int currentDiffPos, const int prevDiffPos, const int nextDiffPos)
{
    bool enableFirst = false;
    bool enableLast = false;
    bool enableNext = false;
    bool enablePrev = false;

    int numDiffs = _infoList.size();
    if(numDiffs > 0) {

        if(currentDiffPos != 0) {
            enableFirst = true ;
        }
        if(currentDiffPos != (numDiffs - 1)) {
            enableLast = true ;
        }

        // if not exists a prev diff, i am on the first or before
        if(prevDiffPos >= 0) {
            enablePrev = true;
        }

        // a lot of logic inside
        if((-1 != nextDiffPos) && (nextDiffPos < (numDiffs))) {
            enableNext = true ;
        }
    }
    ui->gotoFirst->setEnabled(enableFirst);
    ui->gotoPrev->setEnabled(enablePrev);
    ui->gotoNext->setEnabled(enableNext);
    ui->gotoLast->setEnabled(enableLast);
}

void CompareModule::loadConfig()
{
    _options.setCompareComments(Config::getBool(Config::KEY_COMPARE_OPTION_COMMENT, true));
    _options.setCompareText(Config::getBool(Config::KEY_COMPARE_OPTION_TEXT, true));
    _options.setDenormalizeEOL(Config::getBool(Config::KEY_COMPARE_OPTION_DENORMALIZEEOL, false));
}

void CompareModule::on_chkCompareText_stateChanged(int /*state*/)
{
    if(!started) {
        return ;
    }
    _options.setCompareText(ui->chkCompareText->isChecked());
    Config::saveBool(Config::KEY_COMPARE_OPTION_TEXT, _options.isCompareText());
}

void CompareModule::on_chkCompareComments_stateChanged(int /*state*/)
{
    if(!started) {
        return ;
    }
    _options.setCompareComments(ui->chkCompareComments->isChecked());
    Config::saveBool(Config::KEY_COMPARE_OPTION_COMMENT, _options.isCompareComments());
}

void CompareModule::on_chkDenormalizeEOL_stateChanged(int /*state*/)
{
    if(!started) {
        return ;
    }
    _options.setDenormalizeEOL(ui->chkDenormalizeEOL->isChecked());
    Config::saveBool(Config::KEY_COMPARE_OPTION_DENORMALIZEEOL, _options.isDenormalizeEOL());
}

void CompareModule::enableZoom()
{
    bool enableZoomIn = true ;
    bool enableZoomOut = true ;
    bool enableZoomReset = true ;
    if(_originalItemFontSize == _zoomSize) {
        enableZoomReset = false;
    }
    if(MinZoom == _zoomSize) {
        enableZoomOut = false ;
    }
    if(MaxZoom == _zoomSize) {
        enableZoomIn = false ;
    }
    ui->cmdZoom1->setEnabled(enableZoomReset);
    ui->cmdZoomIn->setEnabled(enableZoomIn);
    ui->cmdZoomOut->setEnabled(enableZoomOut);
}


void CompareModule::applyZoom()
{
    _itemFont = _originalFont;
    _itemFont.setPointSize(_zoomSize);
    // reload all the items in the list
    reloadTreeItems(_itemsLeft, ui->treeLeft);
    reloadTreeItems(_itemsRight, ui->treeRight);
}

void CompareModule::reloadTreeItems(QList<QTreeWidgetItem*> &items, QTreeWidget *tree)
{
    tree->setUpdatesEnabled(false);
    foreach(QTreeWidgetItem * item, items) {
        item->setFont(0, _itemFont);
    }
    tree->setUpdatesEnabled(true);
}

void CompareModule::on_cmdZoomIn_clicked()
{
    if(MaxZoom > _zoomSize) {
        _zoomSize++;
        applyZoom();
    }
    enableZoom();
}

void CompareModule::on_cmdZoomOut_clicked()
{
    if(MinZoom < _zoomSize) {
        _zoomSize--;
        applyZoom();
    }
    enableZoom();
}

void CompareModule::on_cmdZoom1_clicked()
{
    if(_originalItemFontSize != _zoomSize) {
        _zoomSize = _originalItemFontSize;
        applyZoom();
    }
    enableZoom();
}

