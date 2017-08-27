/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
#include "xsdwindow.h"
#include "utils.h"
#include "ui_xsdwindow.h"
#include "qxmleditconfig.h"
#include "xsdeditor/widgets/xsdgenericeditor.h"
#include "xsdeditor/choosexsdviewrootitemdialog.h"
#include <QSvgGenerator>


#ifdef  QML
#include <QDeclarativeView>
#include <QDeclarativeContext>
#endif


#define RES_IMAGE_LINK ":/xsdimages/link2"

//#define SCHEMA_FILE_READ    ":/data/test.xsd"
//TODO: test
#define SCHEMA_FILE_READ    Config::getString(Config::KEY_TEST_SAMPLE_FILE_XSD, "" )

void xsdTest(QXmlEditData *data)
{
#ifdef  QML
    QDeclarativeView view;
    QDeclarativeContext *context = view.rootContext();
    context->setContextProperty("backgroundColor", QColor(Qt::yellow));
    view.setSource(QUrl::fromLocalFile(QDir::homePath() + QString("/devel/qxmledit/xsd/src/resources/test.qml"));
                   //view.setSource(QUrl("qrc:/data/test.qml"));
                   view.show();
#endif

                   XSDWindow xsdEditor(data) ;
                   //xsdEditor.setWindowModality(Qt::ApplicationModal);

                   QEventLoop eventLoop;
                   xsdEditor.EVENTLOOP = &eventLoop ;
                   //xsdEditor.setEventLoop(&eventLoop);
                   xsdEditor.show();
                   // occorre gestire event loop inside the window.
    if(eventLoop.exec() > 0) {
    return ;
}
return ;
}


//----------------------------------------------------------------------------------------------

MainItemMenuBuilder::MainItemMenuBuilder(XSDWindow *newParent)
{
    _parent = newParent;
}

MainItemMenuBuilder::~MainItemMenuBuilder()
{
}

QMenu *MainItemMenuBuilder::buildMenu(XSchemaObject *target)
{
    if(NULL == target) {
        return NULL ;
    }
    QMenu *contextMenu = new QMenu(NULL);
    if(NULL == contextMenu) {
        return NULL;
    }
    foreach(ESchemaType type, _actionsOrdered) {
        if(target->canAddChild(type)) {
            QAction *action  = _actions.value(type);
            if(NULL != action) {
                contextMenu->addAction(action);
            }
        }
    }
    contextMenu ->addAction(_parent->getCopyNameAction());
    if(target->getType() == SchemaTypeElement) {
        XSchemaElement *element = (XSchemaElement *) target;
        if(!element->isTypeOrElement()) {
            contextMenu ->addAction(_parent->getCopyElementAction());
        }
    }
    if(target->getType() == SchemaTypeSimpleTypeRestriction) {
        contextMenu ->addAction(_parent->getCopyFacetsAction());
    }
    contextMenu ->addSeparator();
    if(target->hasAReference()) {
        contextMenu ->addAction(_parent->getGotoAction());
    }
    //contextMenu ->addAction(_parent->getDeleteAction()); IN THE EDITOR ONLY
    return contextMenu;
}

void MainItemMenuBuilder::addAction(const ESchemaType type, QAction* action)
{
    _actions.insert(type, action);
    _actionsOrdered.append(type);
}

//----------------------------------------------------------------------------------------------

XSDRootChooseProvider::XSDRootChooseProvider()
{
}

XSDRootChooseProvider::~XSDRootChooseProvider()
{
}

//----------------------------------------------------------------------------------------------

XSDWindow::XSDWindow(QXmlEditData *appData, QWidget *parent) :
    QMainWindow(parent),
    _menuBuilder(this),
    _appData(appData),
    ui(new Ui::XSDWindow),
    _scene(NULL),
    _controller(NULL),
    _backgroundConfig(this),
    _itemContext(appData),
    currentHistoryPosition(-1)
{
    _xsdRootChooseProvider = this ;
    _chooseProviderSet = false ;

    _context.setItemContext(&_itemContext);
    EVENTLOOP = NULL ;
    _isInError = false ;
    _compareComments = false ;
    _compareShowOnlyDifferencesAction = false ;
    oldSizeFirstWidget = -1 ;
    _isSwap = false ;
    _context.setMenuBuilder(&_menuBuilder);
    _mainItem = NULL ;

    ui->setupUi(this);
    if(!completeUi()) {
        Utils::error(tr("Error opening XSD viewer."));
        this->close();
        return ;
    }
    _context.setScene(_scene);
    enableHistory();
    evalObjZoom();
    this->showMaximized();
}

XSDWindow::~XSDWindow()
{
    deleteAllItems();
    _context.setItemContext(NULL);
    _context.clear();
    if(NULL != _scene) {
        delete _scene ;
    }
    if(_chooseProviderSet) {
        if(NULL != _xsdRootChooseProvider) {
            delete _xsdRootChooseProvider ;
        }
    }
    delete ui;
}

void XSDWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QAction *XSDWindow::getDeleteAction()
{
    return deleteAction ;
}

QAction *XSDWindow::getGotoAction()
{
    return _gotoAction ;
}

QAction *XSDWindow::getCopyNameAction()
{
    return _copyNameAction ;
}

QAction *XSDWindow::getCopyElementAction()
{
    return _copyElementAction ;
}

QAction *XSDWindow::getCopyFacetsAction()
{
    return _copyFacetsAction;
}

void XSDWindow::loadString(const QString &inputData)
{
    _stringToLoad = inputData ;
    QTimer::singleShot(1, this, SLOT(on_loadFromString_triggered()));
}

void XSDWindow::loadStringImmediate(const QString &inputData)
{
    _stringToLoad = inputData ;
    on_loadFromString_triggered();
}

void XSDWindow::setTitle(const QString &newTitle)
{
    _title = newTitle;
    setWindowTitle(_title);
}

void XSDWindow::setSelectedObjectKey(const QString &theSelectedElementKey)
{
    _selectedElementKey = theSelectedElementKey ;
}

void XSDWindow::setFileName(const QString &newFileName)
{
    fileName = newFileName;
}

void XSDWindow::addEditors()
{
    QStackedWidget *parentWidget = ui->properties;

    QWidget *genericEditor = new XSDGenericEditor(parentWidget);
    parentWidget->addWidget(genericEditor);
    /* TODO: test for declarative view
    QDeclarativeView *xxx=NULL;
    QDeclarativeView *qmlEditorInclude = new QDeclarativeView();
    //TODO
    xxx=qmlEditorInclude;
    qmlEditorInclude->setSource(QUrl(PATH_EDITOR_INCLUDE) );
    QStackedWidget *parentWidget = ui->properties;
    parentWidget->addWidget(qmlEditorInclude);
    */
}

void XSDWindow::setElementsPanel()
{
    /*ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 4);
    int width = ui->splitter->width();
    int width0 = width / 4 ;
    if(width0 < 50) {
        width0 = 50 ;
    }
    int width1 = width - width0 ;
    setNavSplitterWidgetSizes(width0 , width1);*/
}

bool XSDWindow::completeUi()
{
    ui->xsdCompareTools->setVisible(false);
    ui->compareSummary->setVisible(false);
    ui->actionSwapReferenceAndTarget->setVisible(false);
    ui->properties->setCurrentIndex(EDITOR_NONE);
    _view = ui->view ;
    ui->view->setDragMode(QGraphicsView::RubberBandDrag) ;
    _scene = new XSDScene();

    setElementsPanel();

#ifdef  XSD_READONLY
    ui->properties->setEnabled(false);
    ui->newCmd->setVisible(false);
    ui->cmdC->setVisible(false);
    ui->cmdE->setVisible(false);
    ui->cmdA->setVisible(false);
    ui->cmdS->setVisible(false);
    ui->newR->setVisible(false);
    ui->newExt->setVisible(false);
    ui->load->setVisible(false);
    ui->fromClipb->setVisible(false);
    ui->saveToClipbopard->setVisible(false);
#endif

    ui->properties->setVisible(false);

    if(NULL == _scene) {
        return false ;
    }
    connect(_scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    ui->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //TODO _scene->setSceneRect(QRectF(0, 0, InitialWidth, InitialHeight));
    ui->view->setScene(_scene);
    ui->view->centerOn(0, 0);

    _gotoAction = new QAction(QIcon(RES_IMAGE_LINK), tr("Go To Reference"), this);
    _copyNameAction = new QAction(tr("Copy Name to the Clipboard"), this);
    _copyElementAction = new QAction(tr("Copy element definition to the Clipboard"), this);
    _copyFacetsAction = new QAction(tr("Copy facets to the Clipboard"), this);
    addSequenceAction = createMenuAction(tr("Add Sequence"), SchemaTypeSequence);
    addChoiceAction = createMenuAction(tr("Add Choice"), SchemaTypeChoice);
    addRestrictionAction = new QAction(tr("Add Restriction"), this);//TODO
    addExtensionAction = new QAction(tr("Add Extension"), this);//TODO
    addUnionAction = new QAction(tr("Add Union"), this);//TODO
    addListAction = new QAction(tr("Add List"), this);//TODO
    deleteAction = new QAction(QIcon(":/images/edit-delete"), tr("Delete"), this);

    // toolbox actions

    if(
        /*||  ( NULL == addAttributeAction )
        ||  ( NULL == addSequenceAction ) TODO
        ||  ( NULL == addChoiceAction )
        ||  ( NULL == addUnionAction )
        ||  ( NULL == addExtensionAction )
        ||  ( NULL == addListAction )
        ||  ( NULL == addRestrictionAction )
        ||  ( NULL == deleteAction )
        ||*/ (NULL == _gotoAction)
        || (NULL == _copyNameAction)
        || (NULL == _copyElementAction)
        || (NULL == _copyFacetsAction)
        /*|| (NULL == createMenuAction(tr("Add Include"), SchemaTypeInclude))
        || (NULL == createMenuAction(tr("Add Import"), SchemaTypeImport))
        || (NULL == createMenuAction(tr("Add Redefine"), SchemaTypeRedefine))
        || (NULL == createMenuAction(tr("Add Annotation"), SchemaTypeAnnotation))
        || (NULL == createMenuAction(tr("Add Element"), SchemaTypeElement))*/
    ) {
        return false ;
    }
    connect(ui->xsdCompareTools, SIGNAL(compareAnnotationChanged(bool)), this, SLOT(onCompareAnnotationAction(bool)));
    connect(&_backgroundConfig, SIGNAL(onBackgroundConfigurationChanged(XSDGraphicsBackgroundConfiguration *)), this, SLOT(onBackgroundConfigurationChanged(XSDGraphicsBackgroundConfiguration *)));

    itemContextMenu = new QMenu(this);
    if(NULL == itemContextMenu) {
        return false;
    }

    itemContextMenu->addAction(_copyNameAction);
    itemContextMenu->addAction(_copyElementAction);
    itemContextMenu->addSeparator();
    itemContextMenu->addAction(_gotoAction);

    ui->toolMenu->addAction(ui->actionSwapReferenceAndTarget);
    ui->toolMenu->addAction(ui->actionConfigureAspect);
#ifndef  XSD_READONLY
    //itemContextMenu->addAction(addAttributeAction );
    //itemContextMenu->addAction(addSequenceAction);
    //itemContextMenu->addAction(addChoiceAction);
    itemContextMenu->addSeparator();
    //itemContextMenu->addAction(addRestrictionAction);
    //itemContextMenu->addAction(addExtensionAction);
    //itemContextMenu->addAction(addUnionAction);
    itemContextMenu->addAction(addListAction);
    itemContextMenu->addSeparator();
    itemContextMenu->addAction(deleteAction);


    /*connect(addAttributeAction, SIGNAL(triggered()), this, SLOT(on_addAttributeAction_triggered()));
    connect(addSequenceAction, SIGNAL(triggered()), this, SLOT(on_addSequenceAction_triggered()));
    connect(addChoiceAction, SIGNAL(triggered()), this, SLOT(on_addChoiceAction_triggered()));
    connect(addRestrictionAction, SIGNAL(triggered()), this, SLOT(on_addRestrictionAction_triggered()));
    connect(addExtensionAction, SIGNAL(triggered()), this, SLOT(on_addExtensionAction_triggered()));
    connect(addUnionAction, SIGNAL(triggered()), this, SLOT(on_addUnionAction_triggered()));
    connect(addListAction, SIGNAL(triggered()), this, SLOT(on_addListAction_triggered())); TODO
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(on_deleteAction_triggered()));*/

#endif
    connect(_gotoAction, SIGNAL(triggered()), this, SLOT(on_gotoAction_triggered()));
    connect(_copyNameAction, SIGNAL(triggered()), this, SLOT(on_copyNameAction_triggered()));
    connect(_copyElementAction, SIGNAL(triggered()), this, SLOT(on_copyElementAction_triggered()));
    connect(_copyFacetsAction, SIGNAL(triggered()), this, SLOT(on_copyFacetsAction_triggered()));
    //-------- editors ----
    addEditors();

    evalObjZoom();

    setupSplitter();

    return true ;
}

QAction *XSDWindow::createMenuAction(const QString &label, const ESchemaType type)
{
    QAction *action = new QAction(label, this);
    action->setData((int)type);
    _menuBuilder.addAction(type, action);
    //TODO connect(action, SIGNAL(triggered()), this, SLOT(onAddChildAction()));
    return action;
}

XSDItem *XSDWindow::itemData(QGraphicsItem *item)
{
    if(NULL == item) {
        return NULL ;
    }
    QVariant data = item->data(XSD_ITEM_DATA);
    XSDItem *object = (XSDItem *)data.value<void*>();
    return object ;
}


void XSDWindow::clear()
{
    if(NULL != _scene) {
        _scene->clear();
    }
    _context.clear();
}

bool XSDWindow::newSchema()
{
    _context.setSchema(new XSDSchema(NULL));
    if(NULL == _context.schema()) {
        return false;
    }
    return true ;
}

bool XSDWindow::setNewSchema(XSDSchema *schema)
{
    _context.setSchema(schema);
    if(NULL == _context.schema()) {
        return false;
    }
    return true ;
}


bool XSDWindow::showRoot()
{
    _viewStack.clear();
    RootItem *newRootItem = NULL ;
    if(_context.isOutline()) {
        QString chosenRoot;
        if(NULL != _context.schema()) {
            QList<XSchemaElement *> topLevelElements = _context.schema()->collectCandidateRootElement();
            if(topLevelElements.isEmpty()) {
                QList<XSchemaObject *> elements = _context.schema()->topLevelElements(true);
                if(elements.size() == 1) {
                    chosenRoot = elements.first()->name();
                } else if(elements.isEmpty()) {
                    Utils::error(this, tr("QXmlEdit is unable to find a candidate for the root element."));
                } else {
                    foreach(XSchemaObject *obj, elements) {
                        topLevelElements.append((XSchemaElement*)obj);
                    }
                    if(topLevelElements.isEmpty()) {
                        Utils::error(this, tr("QXmlEdit is unable to find a candidate for the root element."));
                    }
                    Utils::TODO_THIS_RELEASE("gestire il caso in cui non c'e' un root");
                    chosenRoot = _xsdRootChooseProvider->chooseRoot(this, topLevelElements);
                }
            }
        }
        RootOutlineItem *anewRootItem = new RootOutlineItem(&_context);
        anewRootItem ->setChosenRoot(chosenRoot);
        newRootItem = anewRootItem ;
    } else {
        newRootItem = new RootItem(&_context);
    }
    _context.resetRoot();
    if(NULL == _context.schema()) {
        return false;
    }
    _context.setRootItem(newRootItem);
    _context.setShowBaseObjects(false);
    if(NULL != _context.rootItem()) {
        _scene->addItem(_context.rootItem()->graphicItem());
    }
    _context.rootItem()->setItem(_context.schema());
    _mainItem = _context.rootItem() ;
    if(NULL == _context.rootItem()) {
        return false ;
    }
    _context.rootItem()->recalcChildrenPos(&_itemContext);
    Utils::TODO_THIS_RELEASE("_context.rootItem()->afterPositionChange();");
    _scene->updateBounds();
    ui->navigation->emptyNavigationBox();
    ui->navigation->loadNavigationBox(_context.schema());
    _viewStack.push(_context.schema());
    _context.setShowBaseObjects(false);
    QString targetNS ;
    if(NULL != _context.schema()) {
        QString target = _context.schema()->targetNamespace();
        if(!target.isEmpty()) {
            targetNS = tr("Target namespace:\"%1\"").arg(_context.schema()->targetNamespace());
        } else {
            targetNS = tr("No target namespace defined");
        }
    }
    ui->targetNamespaceInfo->setText(targetNS);


    evalObjZoom();
    //TODO connect(_rootItem, SIGNAL(deleted(XSchemaObject*)), this, SLOT(rootDeleted(XSchemaObject*)));
    // set boundaries TODO
    /*QRectF totalSizeRect(0,0,0,0);
    _rootItem->totalSize( totalSizeRect );
    _scene->setSceneRect(QRectF(0, 0, totalSizeRect.width(), totalSizeRect.height() ) );*/

    return true ;
}

XSDItem * XSDWindow::setZoomObject(XSchemaObject *subject)
{
    deleteAllItems();
    bool showBase = false;
    if(_viewStack.size() > 1) {
        showBase = ui->cmdShowBase->isChecked();
    }
    _context.setShowBaseObjects(showBase);

    if(_viewStack.size() == 1) {
        showRoot();
        return _context.rootItem();
    } else {
        _context.resetRoot();
        XSDItem *newItem = XSDItem::createItem(&_context, subject, NULL);
        _scene->addItem(newItem->graphicItem());
        newItem->recalcChildrenPos(&_itemContext);
        newItem->afterPositionChange();
        _scene->updateBounds();
        ui->navigation->emptyNavigationBox();
        _mainItem = newItem ;
        return newItem ;
    }
}


void XSDWindow::setupSplitter()
{
    if(oldSizeFirstWidget == -1) {
        oldSizeFirstWidget = ui->splitter->width() / 5;
    }
    int width = ui->splitter->width();
    int width0 = oldSizeFirstWidget ;
    if(width0 < 50) {
        width0 = 50 ;
    }
    int width1 = width - width0 ;
    if(width1 < 0) {
        width1 = 0 ;
    }
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    setNavSplitterWidgetSizes(width0, width1);
}

void XSDWindow::on_navigationCmd_clicked()
{
    bool isShow = ui->navigationCmd->isChecked();
    ui->navigation->setVisible(isShow);
    ui->searchBox->setVisible(isShow);
    ui->clearSearch->setVisible(isShow);
    if(!isShow) {
        oldSizeFirstWidget = ui->navigation->width();
        int width = ui->splitter->width();
        int width0 = 0 ;
        int width1 = width ;
        setNavSplitterWidgetSizes(width0, width1);
    } else {
        setupSplitter();
    }
}

void XSDWindow::setNavSplitterWidgetSizes(const int width0, const int width1)
{
    QList<int> sizes;
    sizes.append(width0);
    sizes.append(width1);
    ui->splitter->setSizes(sizes);
    ui->splitter->update();
}

void XSDWindow::setNavigationTargetSelection(XSchemaObject *newSelection)
{
    ui->navigation->setNavigationTargetSelection(newSelection);
    evalObjZoom();
}

XSchemaObject *XSDWindow::fromItemData(QTreeWidgetItem *item)
{
    if(NULL == item) {
        return NULL ;
    }
    QVariant data = item->data(0, Qt::UserRole);
    XSchemaObject *object = (XSchemaObject *)data.value<void*>();
    return object ;
}

bool XSDWindow::newRoot()
{
    clear();
    if(newSchema()) {
        return showRoot();
    }
    return false ;
}

bool XSDWindow::setSchema(XSDSchema *theNewSchema)
{
    deleteAllItems();
    if(setNewSchema(theNewSchema)) {
        return showRoot();
    }
    return false ;
}

XSchemaObject *XSDWindow::schema()
{
    return _context.schema();
}

XSDItem *XSDWindow::root()
{
    return _mainItem ;
}

XSDItem *XSDWindow::getSelectedItem()
{
    if(NULL == _scene) {
        Utils::error(this, tr("Selection is not valid"));
        return NULL;
    }
    QList<QGraphicsItem*> selection = _scene->selectedItems();
    if(selection.count() != 1) {
        return NULL;
    }
    QGraphicsItem *item = selection.first();
    XSDItem *xitem = itemData(item);
    return xitem;
}

XSchemaObject *XSDWindow::getSelectedSchemaObject()
{
    XSDItem *xitem = getSelectedItem();
    if(NULL == xitem) {
        return NULL ;
    }
    XSchemaObject *object = xitem->item();
    return object ;
}

#ifdef  XSD_EVENTS_HANDLED
void XSDWindow::on_newCmd_clicked()
{
    if(!newRoot()) {
        Utils::error(tr("Error"));
    }
}


void XSDWindow::on_cmdA_clicked()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddAttribute()) {
        Utils::error(this, tr("Cannot add an element here."));
        return ;
    }
    XSchemaAttribute *attribute = object->addAttribute() ;
    if(NULL == attribute) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
    attribute->setName("newAttribute1");
}

void XSDWindow::on_cmdE_clicked()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddElement()) {
        Utils::error(this, tr("Cannot add an element here."));
        return ;
    }
    XSchemaElement *element = object->addElement() ;
    if(NULL == element) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
    element->setName("newElement");
}

void XSDWindow::on_cmdC_clicked()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddChoice()) {
        Utils::error(this, tr("Cannot add a choice here."));
        return ;
    }
    XSchemaChoice *choice = object->addChoice() ;
    if(NULL == choice) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}

void XSDWindow::on_cmdS_clicked()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddSequence()) {
        Utils::error(this, tr("Cannot add a sequence here."));
        return ;
    }
    XSchemaSequence *sequence = object->addSequence() ;
    if(NULL == sequence) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}

void XSDWindow::on_newR_clicked()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddRestriction()) {
        Utils::error(this, tr("Cannot add a restriction here."));
        return ;
    }
    XSchemaObject *restriction = object->addRestriction() ;
    if(NULL == restriction) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}

void XSDWindow::on_addExtensionAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddExtension()) {
        Utils::error(this, tr("Cannot add an extension here."));
        return ;
    }
    XSchemaObject *extension = object->addExtension() ;
    if(NULL == extension) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}

void XSDWindow::on_addUnionAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddUnion()) {
        Utils::error(this, tr("Cannot add an union here."));
        return ;
    }
    XSchemaSimpleTypeUnion *xunion = object->addUnion() ;
    if(NULL == xunion) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}

void XSDWindow::on_addListAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddList()) {
        Utils::error(this, tr("Cannot add a list here."));
        return ;
    }
    XSchemaSimpleTypeList *list = object->addList() ;
    if(NULL == list) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}

void XSDWindow::on_addRestrictionAction_triggered()
{
    on_newR_clicked();
}

void XSDWindow::on_addElementAction_triggered()
{
    on_cmdE_clicked();
}

void XSDWindow::on_addAttributeAction_triggered()
{
    on_cmdA_clicked();
}

void XSDWindow::on_addSequenceAction_triggered()
{
    on_cmdS_clicked();
}

void XSDWindow::on_addChoiceAction_triggered()
{
    on_cmdC_clicked();
}

void XSDWindow::on_saveToClipbopard_clicked()
{
    if(NULL != _schema) {
        _schema->saveToClipboard();
    } else {
        Utils::error(tr("No root item"));
    }
}

//TODO: use try catch for IO
void XSDWindow::on_load_clicked()
{
    _isInError = true;
    if(newSchema()) {
        try {
            _schema->read(SCHEMA_FILE_READ);
            showRoot();
            _isInError = false;
        } catch(XsdException *ex) {
            Utils::error(tr("Error loading schema.\n%1").arg(ex->cause()));
        } catch(...) {
            Utils::error(tr("Unknown exception."));
        }
    } else {
        Utils::error(tr("No root item"));
    }
}

void XSDWindow::on_fromClipb_clicked()
{
    if(newSchema()) {
        _schema->readFromClipboard();
        showRoot();
    } else {
        Utils::error(tr("No root item"));
    }
}

void XSDWindow::on_deleteAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(Utils::askYN(tr("Do you really want to delete this item?"))) {
        object->deleteObject();
        if(object == _schema) {
            _rootItem = NULL ;
            _schema = NULL ;
        }
    }
}

void XSDWindow::onAddChildAction()
{
    //get the type
    QAction *action = qobject_cast<QAction*>(sender());
    if(NULL == action) {
        return ;
    }
    ESchemaType type = (ESchemaType)(action->data().toInt());
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    if(!object->canAddChild(type)) {
        Utils::error(this, tr("Cannot add a child here."));
        return ;
    }
    XSchemaObject *newChild = object->addNewChild(type) ;
    if(NULL == newChild) {
        Utils::error(this, tr("Error creating item."));
        return ;
    }
}
#endif

bool XSDWindow::checkNullObject(XSchemaObject *object)
{
    if(NULL == object) {
        return false ;
    }
    return true ;
}


void XSDWindow::on_gotoAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    QString referenceName = object->referencedObjectName();
    XReferenceType referenceType = object->referencedObjectType();
    XSchemaObject *target = NULL;
    Utils::TODO_NEXT_RELEASE("handle namespaces in a less bovine mode");
    int indexOfColon = referenceName.indexOf(":");
    if(indexOfColon >= 0) {
        referenceName = referenceName.mid(indexOfColon + 1);
    }

    switch(referenceType) {
    default:
        // do nothing here
        break;
    case XRT_ELEMENT:
        target = _context.rootItem()->schema()->topLevelElement(referenceName);
        break;
    case XRT_TYPE:
        target = _context.rootItem()->schema()->topLevelType(referenceName);
        break;
    case XRT_ATTRIBUTE:
        target = _context.rootItem()->schema()->topLevelAttribute(referenceName);
        break;
    }
    if(NULL == target) {
        Utils::error(this, tr("Error finding the referenced element."));
        return ;
    }
    jumpToObject(target);
}

void XSDWindow::on_copyNameAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(!checkNullObject(object)) {
        return ;
    }
    QString objectName = object->name();
    if(objectName.isEmpty()) {
        QString referenceName = object->referencedObjectName();
        objectName = referenceName ;
    }

    QClipboard *clipBoard = QApplication::clipboard();
    clipBoard->setText(objectName);
}

void XSDWindow::on_copyElementAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    copyElementActionExecute(object);
}

void XSDWindow::informationError()
{
    Utils::error(this, tr("Could not retrieve the information."));
}

bool XSDWindow::copyFacetsActionExecute(XSchemaObject *object)
{
    if(!checkNullObject(object)) {
        return false ;
    }
    QString objectDescription ;
    if(object->getType() == SchemaTypeSimpleTypeRestriction) {
        XSchemaSimpleTypeRestriction *restriction = (XSchemaSimpleTypeRestriction*)object ;
        objectDescription = restriction->exportFacets();
    }

    if(!objectDescription.isEmpty()) {
        QClipboard *clipBoard = QApplication::clipboard();
        clipBoard->setText(objectDescription);
        return true;
    } else {
        informationError();
    }
    return false;
}
void XSDWindow::on_copyFacetsAction_triggered()
{
    XSchemaObject *object = getSelectedSchemaObject();
    copyFacetsActionExecute(object);
}

bool XSDWindow::copyElementActionExecute(XSchemaObject *object)
{
    if(!checkNullObject(object)) {
        return false ;
    }
    QString objectDescription ;
    if(object->getType() == SchemaTypeElement) {
        XSchemaElement *element = (XSchemaElement *)object ;
        objectDescription = element->xmlRepresentation();
    }

    if(!objectDescription.isEmpty()) {
        QClipboard *clipBoard = QApplication::clipboard();
        clipBoard->setText(objectDescription);
        return true;
    } else {
        informationError();
    }
    return false;
}


void XSDWindow::closeEvent(QCloseEvent *event)
{
    selectLastObject();
    if(NULL != EVENTLOOP) {
        EVENTLOOP->quit();
    }
    QMainWindow::closeEvent(event);
}

void XSDWindow::on_loadFromString_triggered()
{
    try {
        _isInError = true;
        if(newSchema()) {
            QString folder = fileName;
            if(!fileName.isEmpty()) {
                QFileInfo info(fileName);
                if(!info.isDir()) {
                    folder = info.absoluteDir().absolutePath();
                }
            }
            XSDLoadContext loadContext;
            loadContext.setLoadKeys(true);
            _context.schema()->readFromString(&loadContext, _stringToLoad, true, _appData->xsdNetworkAccessManager(), folder);
            showRoot();
            if(!_selectedElementKey.isEmpty()) {
                //select the element
                XSchemaObject *target = loadContext.findObjectForKey(_selectedElementKey);
                if(NULL != target) {
                    jumpToObject(target);
                }
            }
            _isInError = false;
        } else {
            Utils::error(tr("No root item"));
        }
    } catch(XsdException *ex) {
        Utils::error(tr("Error loading schema.\n%1").arg(ex->cause()));
    } catch(...) {
        Utils::error(tr("Unknown exception."));
    }
}


// activates the right editor
//TODO: investigare se l'editor non attivo viene disabilitato o meno.
// IMPORTANTE
// uso Q_INTERFACE
void XSDWindow::setPropertyTarget(XSchemaObject *target)
{
    EEditor currentIndex = EDITOR_NONE ;
    if(NULL == target) {
        currentIndex = EDITOR_NONE ;
    } else {
        ESchemaType type = target->getType()  ;
        if(SchemaTypeSchema == type) {
            currentIndex = EDITOR_SCHEMA ;
        } else if(SchemaTypeChoice  == type) {
            currentIndex = EDITOR_CHOICE ;
        } else if(SchemaTypeSequence  == type) {
            currentIndex = EDITOR_SEQUENCE ;
        } else if(SchemaTypeElement  == type) {
            currentIndex = EDITOR_ELEMENT ;
        } else if(SchemaTypeAttribute  == type) {
            currentIndex = EDITOR_ATTRIBUTE ;
        } else if(SchemaTypeInclude  == type) {
            currentIndex = EDITOR_INCLUDE ;
        } else {
            currentIndex = EDITOR_GENERIC ;
        }
        /* TODO qml experimental code
        currentIndex = EDITOR_INCLUDE ;
        if( NULL != xxx ) {
            QDeclarativeContext *context = xxx->rootContext();
            context->setContextProperty("item", target);
        }
        */
    }
    //TODO QWidget *actualWidget = ui->properties->currentWidget();
    /*TODO if( NULL != actualWidget ) {
        actualWidget->setEnabled(false);
    }*/
    ui->properties->setCurrentIndex((int)currentIndex);
    QWidget *actualWidget = ui->properties->currentWidget();
    if(NULL != actualWidget) {
        if(actualWidget->children().size() > 0) {
            QWidget *firstChild = qobject_cast<QWidget *>(actualWidget->children().at(0));
            if(NULL != firstChild) {
                XEditorInterface *iEditor  = qobject_cast<XEditorInterface *>(firstChild);
                if(NULL != iEditor) {
                    iEditor->setTarget(target);
                }
            }
        }
    }
}

void XSDWindow::selectionChanged()
{
    XSchemaObject *newSelection = NULL ;
    if(NULL != _scene) {
        QList<QGraphicsItem*> selection = _scene->selectedItems();
        if(selection.count() == 1) {
            QGraphicsItem *item = selection.first();
            XSDItem *xitem = itemData(item);
            if(NULL != xitem) {
                newSelection = xitem->item();
            }
        }
    }
    setPropertyTarget(newSelection);
    setNavigationTargetSelection(newSelection);
}


void XSDWindow::on_cmdZoomIn_clicked()
{
    if(NULL != _view) {
        _view->scale(1.1, 1.1);
    }
}

void XSDWindow::on_cmdZoomOut_clicked()
{
    if(NULL != _view) {
        _view->scale(0.9, 0.9);
    }
}

void XSDWindow::on_cmdZoom1_clicked()
{
    if(NULL != _view) {
        _view->resetTransform();
    }
}

void XSDWindow::on_closeButton_clicked()
{
    QMainWindow::close();
}

void XSDWindow::on_clearSearch_clicked()
{
    ui->searchBox->setText("");
}

void XSDWindow::on_searchBox_textChanged(const QString &newText)
{
    ui->navigation->newSearchText(newText);
}

void XSDWindow::truncateHistory()
{
    if((currentHistoryPosition >= 0)  && (currentHistoryPosition < history.count())) {
        history.resize(currentHistoryPosition + 1);
    }
}

void XSDWindow::enableHistory()
{
    ui->backButton->setEnabled(currentHistoryPosition > 0);
    ui->forwardButton->setEnabled(currentHistoryPosition <= (history.count() - 2));
}

void XSDWindow::on_backButton_clicked()
{
    if((currentHistoryPosition > 0) && (history.count() > currentHistoryPosition)) {
        XSDItem *item = _context.getItemOfObject(history.at(currentHistoryPosition - 1));
        if(NULL != item) {
            currentHistoryPosition -- ;
            enableHistory();
            _scene->gotoItem(item->graphicItem());
        }
    }
}

void XSDWindow::on_forwardButton_clicked()
{
    if((currentHistoryPosition >= 0) && ((history.count() - 2) >= currentHistoryPosition)) {
        XSDItem *item = _context.getItemOfObject(history.at(currentHistoryPosition + 1));
        if(NULL != item) {
            currentHistoryPosition ++ ;
            enableHistory();
            _scene->gotoItem(item->graphicItem());
        }
    }
}

void XSDWindow::on_navigation_itemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    XSchemaObject *target = fromItemData(item);
    if(NULL == target) {
        return ;
    }
    // jump to the object
    jumpToObject(target);
}

void XSDWindow::historyNewTarget(XSchemaObject *target)
{
    history.append(target);
    currentHistoryPosition = history.count() - 1;
}

void XSDWindow::jumpToObject(XSchemaObject *target)
{
    XSDItem *item = _context.getItemOfObject(target);
    if(NULL != item) {
        truncateHistory();
        historyNewTarget(target);
        enableHistory();
        _scene->gotoItem(item->graphicItem());
    }
}

//TODO: error checking
void XSDWindow::on_printCmd_clicked()
{
    printPDF();
}

//TODO: error checking
void XSDWindow::on_svgCmd_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export as SVG"),
                       Utils::changeFileType(fileName, ".svg"),
                       tr("SVG images (*.svg);;All files (*)"));

    if(filePath.isEmpty()) {
        return ;
    }

    QSvgGenerator svg;
    svg.setFileName(filePath);
    svg.setTitle(_title);
    svg.setDescription(tr("View"));
    //svg.setSize(_scene. printer->paperSize(QPrinter::Point).toSize());
    QRectF sceneRect = _scene->sceneRect();
    svg.setSize(QSize(sceneRect.width(), sceneRect.height()));
    {
        QPainter painter(&svg);
        QBrush solidBrush(Qt::NoBrush);
        QBrush oldBrush = _scene->backgroundBrush();
        setUpdatesEnabled(false);
        _scene->setBackgroundBrush(solidBrush);
        paintScene(NULL, &painter, QRectF(), QRectF(), 0, 0, 0, 0);
        _scene->setBackgroundBrush(oldBrush);
        setUpdatesEnabled(true);
    }
    setWindowTitle(_title);
    Utils::message(this, tr("Diagram exported in SVG format."));

}

void XSDWindow::evalObjZoom()
{
    bool enableIn = false;
    bool enableOut = false;
    bool enableReset = false;
    bool enableShowBase = false;

    if(_viewStack.size() > 1) {
        enableOut = true ;
        enableReset = true ;
        enableShowBase = true ;
    } else {
        ui->cmdShowBase->setChecked(false);
    }
    XSchemaObject *object = getSelectedSchemaObject();
    if(checkNullObject(object)) {
        enableIn = true ;
    }
    ui->cmdObjZoomIn->setEnabled(enableIn);
    ui->cmdObjZoomOut->setEnabled(enableOut);
    ui->cmdObjZoomReset->setEnabled(enableReset);
    ui->cmdShowBase->setEnabled(enableShowBase);
    bool visibleObjects = (contextType() == XsdGraphicContext::CONTEXT_GRAPHICS);
    ui->cmdObjZoomIn->setVisible(visibleObjects);
    ui->cmdObjZoomOut->setVisible(visibleObjects);
    ui->cmdObjZoomReset->setVisible(visibleObjects);
    ui->cmdShowBase->setVisible(visibleObjects);
}

// display with base objects
void XSDWindow::on_cmdShowBase_clicked()
{
    if(_viewStack.size() > 1) {
        setZoomObject(_viewStack.top());
        truncateHistory();
        evalObjZoom();
    }
}

void XSDWindow::on_cmdObjZoomIn_clicked()
{
    XSchemaObject *object = getSelectedSchemaObject();
    if(checkNullObject(object)) {
        _viewStack.push(object);
        setZoomObject(object);
        truncateHistory();
        evalObjZoom();
    }
}

void XSDWindow::on_cmdObjZoomOut_clicked()
{
    if(_viewStack.size() > 1) {
        _viewStack.pop();
        XSchemaObject *object = _viewStack.first();
        setZoomObject(object);
        _context.setShowBaseObjects(false);
        truncateHistory();
        evalObjZoom();
    }
}

void XSDWindow::on_cmdObjZoomReset_clicked()
{
    if(_viewStack.size() > 1) {
        _viewStack.resize(1);
        deleteAllItems();
        _scene->clear();
        showRoot();
        truncateHistory();
        evalObjZoom();
    }
}


void XSDWindow::restoreSelection(QList<QGraphicsItem*> &itemsToSelect)
{
    foreach(QGraphicsItem * item, itemsToSelect) {
        item->setSelected(true);
    }
}

void XSDWindow::calculatePageRect(QPainter *painter, QRectF &destArea)
{
    Utils::TODO_THIS_RELEASE("inutile?");
    QFontMetrics fm = painter->fontMetrics();
    QString text = QString(tr("Page %1/%1")).arg(999).arg(999);
    QRectF measRect = fm.boundingRect(text);
    int x = destArea.left() + (destArea.width() - measRect.width()) / 2;
    int y = destArea.bottom() - measRect.height();
    destArea.setRect(x, y, measRect.width() + measRect.width() / 10, measRect.height());
}

void XSDWindow::callController()
{
    if(NULL != controller() && (contextType() == XsdGraphicContext::CONTEXT_DIFF)) {
        XSDCompareOptions options;
        options.setCompareComment(ui->xsdCompareTools->isCompareAnnotations());
        controller()->evaluate(this, options, _isSwap);
    }
}


void XSDWindow::onCompareAnnotationAction(bool /*newState*/)
{
    callController();
}

void XSDWindow::on_xsdCompareTools_swapReference()
{
    _isSwap = !_isSwap ;
    callController();
}

void XSDWindow::on_actionSwapReferenceAndTarget_triggered()
{
    on_xsdCompareTools_swapReference();
}

void XSDWindow::deleteAllItems()
{
    _scene->clearSelection();
    if(NULL != _mainItem) {
        delete _mainItem;
        _mainItem = NULL;
    }
    _context.setRootItem(NULL);
    if(NULL != _scene) {
        _scene->clear();
    }
    Utils::TODO_THIS_RELEASE("rebuild cache on repaint when switchng from a mode to another one.");
    _context.clearCache();
}

void XSDWindow::on_cmdShowDepend_clicked()
{
    if(contextType() != XsdGraphicContext::CONTEXT_DIFF) {
        _context.setShowAllSchema(ui->cmdShowDepend->isChecked());
        deleteAllItems();
        showRoot();
    }
}

void XSDWindow::on_actionConfigureAspect_triggered()
{
    if(!_backgroundConfig.isVisible()) {
        _backgroundConfig.show();
    }
}

void XSDWindow::onBackgroundConfigurationChanged(XSDGraphicsBackgroundConfiguration * configuration)
{
    _scene->setBackgroundCfg(configuration);
}

IXSDController *XSDWindow::controller()
{
    return _controller ;
}

bool XSDWindow::isInError()
{
    return _isInError;
}

void XSDWindow::setController(IXSDController *value)
{
    _controller = value ;
    ui->xsdCompareTools->setVisible(contextType() == XsdGraphicContext::CONTEXT_DIFF);
    ui->compareSummary->setVisible(contextType() == XsdGraphicContext::CONTEXT_DIFF);
    ui->actionSwapReferenceAndTarget->setVisible(contextType() == XsdGraphicContext::CONTEXT_DIFF);
    ui->cmdShowDepend->setVisible(contextType() != XsdGraphicContext::CONTEXT_DIFF);
    ui->targetNamespaceInfo->setVisible(contextType() != XsdGraphicContext::CONTEXT_DIFF);
}

XsdGraphicContext::EContextType XSDWindow::contextType()
{
    return _context.contextType() ;
}

void XSDWindow::setContextType(const XsdGraphicContext::EContextType value)
{
    _context.setContextType(value) ;
}

void XSDWindow::setXSDCompareReferencePath(const QString &path)
{
    ui->compareSummary->setReference(path);
}

void XSDWindow::setXSDCompareTargetPath(const QString &path)
{
    ui->compareSummary->setTarget(path);
}

void XSDWindow::setXSDCompareSummary(QList<XSchemaObject*> &added, QList<XSchemaObject*> &modified, QList<XSchemaObject*> &deleted)
{
    ui->compareSummary->setSummaryData(added, modified, deleted);
}

void XSDWindow::on_compareSummary_objectDoubleClicked(XSchemaObject* target)
{
    if(NULL == target) {
        return ;
    }
    // jump to the object
    jumpToObject(target);
}

void XSDWindow::selectLastObject()
{
    XSchemaObject *obj = getSelectedSchemaObject();
    if(NULL != obj) {
        QString key = obj->key();
        setSelectedExitKey(key);
    }
}

bool XSDWindow::close()
{
    selectLastObject();
    _backgroundConfig.close();
    return QMainWindow::close();
}

void XSDWindow::setSelectedExitKey(const QString &key)
{
    if(!key.isEmpty()) {
        _returnKey = key ;
    }
}

QString XSDWindow::selectedExitKey()
{
    return _returnKey ;
}

void XSDWindow::on_cmdOutline_clicked()
{
    Utils::TODO_THIS_RELEASE("abilitare a seconda degli stati");
    const bool isOutline = ui->cmdOutline->isChecked();
    if(isOutline && ! _context.isOutline()) {
        _context.setContextType(XsdGraphicContext::CONTEXT_OUTLINE) ;
    } else if(!isOutline && _context.isOutline()) {
        _context.setContextType(XsdGraphicContext::CONTEXT_GRAPHICS);
    }
    deleteAllItems();
    showRoot();
}

void XSDWindow::setOutlineMode(const bool isOutline)
{
    ui->cmdOutline->setChecked(isOutline);
    on_cmdOutline_clicked();
}

void XSDWindow::setChooseProvider(XSDRootChooseProvider *newValue)
{
    if(_chooseProviderSet) {
        if(NULL != _xsdRootChooseProvider) {
            delete _xsdRootChooseProvider ;
        }
    }
    if(NULL == newValue) {
        _chooseProviderSet = false;
        _xsdRootChooseProvider = this ;
    } else {
        _xsdRootChooseProvider = newValue ;
        _chooseProviderSet = true ;
    }
}

QString XSDWindow::chooseRoot(QWidget *parent, QList<XSchemaElement*> elements)
{
    ChooseXSDViewRootItemDialog dlg(parent, elements);
    if(dlg.exec() == QDialog::Accepted) {
        QString nameResult = dlg.selection();
        return nameResult ;
    }
    return "";
}
