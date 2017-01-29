/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
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


#include "qxmleditdata.h"

#include <QDesktopServices>
#include <QDir>
#include "qxmleditconfig.h"
#include "utils.h"
#include "stylepersistence.h"
#include "balsamiqsrc/balsamiqplugin.h"
#include "xsdeditor/xsdplugin.h"
#include "modules/services/systemservices.h"
#include "modules/xsd/xsdmanager.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/search/searchmanager.h"
#include "modules/services/colormanager.h"
#include "modules/copyattr/copiedattribute.h"
#include "modules/xslt/xsltmanager.h"
#include "modules/services/anotifier.h"
#include "modules/encoding/unicodehelper.h"

const QString QXmlEditData::XsltStyleName = "XSLT";
const QString QXmlEditData::XsltStyleDescription = tr("Xslt predefined style");
const QString QXmlEditData::SCXMLStyleName = "SCXML";
const QString QXmlEditData::SCXMLStyleDescription = tr("SCXML predefined style");

QXmlEditData::QXmlEditData()
{
    _notifier = NULL ;
    _xsltManager  = NULL ;
    _unicodeHelper = NULL ;
    _copyAttributesManager = NULL ;
    _colorManager = NULL ;
    _searchManager = NULL ;
    _namespaceManager = NULL ;
    _xsdManager = NULL;
    _dataInterface = NULL ;
    _clipBoardItem = NULL ;
    _defaultStyle = NULL;
    _xsltStyle = NULL ;
    _SCXMLStyle = NULL ;
    _styleVersion = 0 ;
    _experimentalFeaturesEnabled = false;
    internalInit();
}

QXmlEditData::~QXmlEditData()
{
    disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardDataChanged()));
    foreach(VStyle * style, _styles) {
        delete style ;
    }
    foreach(VStyle * style, _predefinedStyles) {
        delete style ;
    }
    cleanClipBoardData();
    if(NULL != _xsdManager) {
        delete _xsdManager ;
    }
    if(NULL != _namespaceManager) {
        delete _namespaceManager ;
    }
    if(NULL != _searchManager) {
        delete _searchManager;
    }
    if(NULL != _colorManager) {
        delete _colorManager;
    }
    if(NULL != _copyAttributesManager) {
        delete _copyAttributesManager;
    }
    if(NULL != _unicodeHelper) {
        delete _unicodeHelper;
    }
    if(NULL != _notifier) {
        delete _notifier;
    }
    if(NULL != _xsltManager) {
        delete _xsltManager;
    }
}

VStyle* QXmlEditData::defaultStyle() const
{
    return _defaultStyle ;
}

VStyle *QXmlEditData::getStyle(const QString &tag) const
{
    if((NULL == tag) || (tag.length() == 0)) {
        return NULL ;
    }
    foreach(VStyle * style, _styles) {
        if(style->name() == tag) {
            return style ;
        }
    }
    return NULL ;
}

VStyle *QXmlEditData::getPredefinedStyle(const QString &tag) const
{
    if(tag.isEmpty()) {
        return NULL ;
    }
    foreach(VStyle * style, _predefinedStyles) {
        if(style->name() == tag) {
            // load from resources
            if(!style->initFromResources()) {
                Utils::error("Unable to activate style");
                return NULL ;
            }
            return style ;
        }
    }
    return NULL ;
}

VStyle *QXmlEditData::createDefaultStyle()
{
    VStyle *defaultStyle = new VStyle(predefinedStyleName(), tr("this is the predefined style"));
    defaultStyle->addId("name", true);
    defaultStyle->addId("id", false);

    StyleEntry *newEntry = new StyleEntry("default");
    bool defaultStyleIsBold = Config::getBool(Config::KEY_VIEW_STYLE_DEFAULT_BOLD, QXmlEditData::DefaultStyleFontBold);
    newEntry->setBold(defaultStyleIsBold);
    defaultStyle->setDefaultStyleEntry(newEntry);

    _styles.append(defaultStyle);
    return defaultStyle ;
}

QString QXmlEditData::predefinedStyleName()
{
    return QString("predefined");
}

void QXmlEditData::init()
{
    // nothing atm
}

void QXmlEditData::internalInit()
{
    if(NULL == _defaultStyle) {
        _defaultStyle = createDefaultStyle() ;
        if(!loadStyles()) {
            Utils::error(tr("Error loading styles"));
        }
    }
    _notifier = new ANotifier();
    _xsltManager = new XsltManager();
    _unicodeHelper = new UnicodeHelper();
    _copyAttributesManager = new CopyAttributesManager();
    _colorManager = new ColorManager();
    _searchManager = new SearchManager();
    _namespaceManager = new NamespaceManager();
    _xsdManager = new XSDManager();
    //--
    _xsltStyle = new VStyle(XsltStyleName, XsltStyleDescription);
    _xsltStyle->setResFileName(":/xslt/xsltStyle");
    _predefinedStyles.append(_xsltStyle);
    //--
    _SCXMLStyle = new VStyle(SCXMLStyleName, SCXMLStyleDescription);
    _SCXMLStyle->setResFileName(":/SCXML/scxmlStyle");
    _predefinedStyles.append(_SCXMLStyle);
    //--
    _colorManager->readCfg();
    _experimentalFeaturesEnabled = Config::getBool(Config::KEY_MAIN_ENABLEEXPERIMENTS, false);
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardDataChanged()));
}

/** hook for pre-delete
  */
void QXmlEditData::end()
{
    //_networkAccessManager.setCache(NULL);
}

const QVector<VStyle*> &QXmlEditData::getStyles() const
{
    return _styles ;
}

const QVector<VStyle*> &QXmlEditData::getPredefinedStyles() const
{
    return _predefinedStyles ;
}


ColorManager *QXmlEditData::colorManager()
{
    return _colorManager;
}

EStylesDir QXmlEditData::getStylesDirLocation()
{
    QString setting = Config::getString(Config::KEY_STYLE_DIRLOCATION, STYLE_DIR_USESTD);
    if(setting == STYLE_DIR_USEDIR) {
        return ESTYLE_DIR_USEDIR;
    }
    if(setting == STYLE_DIR_USEAPPL) {
        return ESTYLE_DIR_USEAPPL;
    }
    return ESTYLE_DIR_USESTD ;
}


void QXmlEditData::setStylesDirLocation(const EStylesDir type)
{
    switch(type) {
    case ESTYLE_DIR_USEAPPL:
        Config::saveString(Config::KEY_STYLE_DIRLOCATION, STYLE_DIR_USEAPPL);
        break;
    case ESTYLE_DIR_USEDIR:
        Config::saveString(Config::KEY_STYLE_DIRLOCATION, STYLE_DIR_USEDIR);
        break;
    default:
        Config::saveString(Config::KEY_STYLE_DIRLOCATION, STYLE_DIR_USESTD);
        break;
    }
}

QString QXmlEditData::getStylesDirSpecific()
{
    return Config::getString(Config::KEY_STYLE_DATADIR, "");
}

QString QXmlEditData::getStylesDirStandard()
{
    return SystemServices::storageDataLocation();
}

QString QXmlEditData::getStylesDirApplication()
{
    QString toReturn = getResourceDir();
    return toReturn ;
}



bool QXmlEditData::loadStyles()
{
    EStylesDir type = getStylesDirLocation();
    QString stylesDirectory ;
    switch(type) {
    case ESTYLE_DIR_USEAPPL:
        stylesDirectory = getStylesDirApplication();
        break;
    case ESTYLE_DIR_USEDIR:
        stylesDirectory = getStylesDirSpecific();
        break;
    default:
        stylesDirectory = getStylesDirStandard();
        break;
    }
    StylePersistence persistence ;
    return persistence.scanDirectory(stylesDirectory, &_styles);
}

QString QXmlEditData::dataDirectory()
{
    return SystemServices::storageDataLocation();
}

QString QXmlEditData::cacheProgramDirectory()
{
    return SystemServices::cacheProgramDirectory();
}

QString QXmlEditData::snippetsLocalDir()
{
    return SystemServices::storageDataLocation() + QDir::separator() + "snippets";
}

QString QXmlEditData::snippetsProgramDir()
{
    QString toReturn = getResourceDir() + QDir::separator() + "snippets" ;
    return toReturn;
}

QString QXmlEditData::getResourceDir()
{
#ifdef ENVIRONMENT_MACOS
    //All the others
    QString toReturn ;
    toReturn = QApplication::applicationDirPath() + QDir::separator() + ".." + QDir::separator() + "SharedSupport" ;
    return toReturn ;
#else

#ifdef UNIX_RESOURCES
    QString toReturn = QString(STRINGIZE(UNIX_RESOURCE_PATH));
    return toReturn;
#else
    //All the others
    return QApplication::applicationDirPath();
#endif

#endif
}

QString QXmlEditData::getDocsDir()
{
#ifdef ENVIRONMENT_MACOS
    //All the others
    QString toReturn ;
    toReturn = QApplication::applicationDirPath() + QDir::separator() + ".." + QDir::separator() + "SharedSupport" ;
    return toReturn ;
#else

#ifdef UNIX_RESOURCES
    QString toReturn = QString(STRINGIZE(UNIX_DOC_PATH));
    return toReturn;
#else
    //All the others
    return QApplication::applicationDirPath();
#endif

#endif
}

//--- region(notify)
ANotifier *QXmlEditData::notifier()
{
    return _notifier;
}

//--- endregion(notify)


//--- region(xsd)

bool QXmlEditData::isAutovalidationOn()
{
    return Config::getBool(Config::KEY_AUTOLOAD_VALIDATION, false);
}

void QXmlEditData::setAutovalidationOn(const bool newState)
{
    Config::saveBool(Config::KEY_AUTOLOAD_VALIDATION, newState);
}

//--- endregion(xsd)


//--- region(xsdDisplay)
bool QXmlEditData::isXsdDisplayHoriz()
{
    return true;
}

void QXmlEditData::setXsdDisplayHoriz(const bool value)
{
    Config::saveInt(Config::KEY_XSD_DISPLAY_LAYOUT, value ? 1 : 0);
}

//--- endregion(xsdDisplay)


//--- region(behaviour)
void QXmlEditData::enableAutoscroll(const bool enabled)
{
    Config::saveBool(Config::KEY_GENERAL_AUTOSCROLL_ENABLED, enabled) ;
}


bool QXmlEditData::isAutoscroll()
{
    return Config::getBool(Config::KEY_GENERAL_AUTOSCROLL_ENABLED, true) ;
}

//--- endregion(behaviour)


//--- region(network)
QNetworkAccessManager *QXmlEditData::xsdNetworkAccessManager()
{
    return &_networkAccessManager;
}
//--- endregion(network)

bool QXmlEditData::areExperimentalFeaturesEnabled()
{
    return _experimentalFeaturesEnabled;
}

void QXmlEditData::setExperimentalFeaturesEnabled(const bool value)
{
    _experimentalFeaturesEnabled = value ;
    Config::saveBool(Config::KEY_MAIN_ENABLEEXPERIMENTS, _experimentalFeaturesEnabled);
}


//--- region(copyAttributes)
CopyAttributesManager *QXmlEditData::copyAttributesManager()
{
    return _copyAttributesManager;
}
//--- endregion(copyAttributes)

//--- region(searchTerms)
QStringList &QXmlEditData::searchTerms()
{
    return _searchTerms;
}
//--- endregion(searchTerms)

//--- region(xslt)

XsltManager *QXmlEditData::xsltManager()
{
    return _xsltManager ;
}
bool QXmlEditData::isShowXSLTPanel()
{
    return Config::getBool(Config::KEY_XSL_SHOW_PANEL, true);
}

bool QXmlEditData::isAutoXSLTMode()
{
    return Config::getBool(Config::KEY_XSL_AUTOMODE, false);
}

void QXmlEditData::setShowXSLTPanel(const bool value)
{
    Config::saveBool(Config::KEY_XSL_SHOW_PANEL, value);
}

void QXmlEditData::setAutoXSLTMode(const bool value)
{
    Config::saveBool(Config::KEY_XSL_AUTOMODE, value);
}

//--- endregion(xslt)

//--- region(scxml)

bool QXmlEditData::isShowSCXMLPanel()
{
    return Config::getBool(Config::KEY_SCXML_SHOW_PANEL, true);
}

bool QXmlEditData::isAutoSCXMLMode()
{
    return Config::getBool(Config::KEY_SCXML_AUTOMODE, false);
}

void QXmlEditData::setShowSCXMLPanel(const bool value)
{
    Config::saveBool(Config::KEY_SCXML_SHOW_PANEL, value);
}

void QXmlEditData::setAutoSCXMLMode(const bool value)
{
    Config::saveBool(Config::KEY_SCXML_AUTOMODE, value);
}

//--- endregion(scxml)

//--- region(metadata)
bool QXmlEditData::isUpdateMetadata()
{
    return Config::getBool(Config::KEY_METADATA_UPDATE, true);
}

void QXmlEditData::setUpdateMetadata(const bool value)
{
    Config::saveBool(Config::KEY_METADATA_UPDATE, value);
}

QString QXmlEditData::defaultEncoding()
{
    return "UTF-8";
}

//--- endregion(metadata)



//--- region(prolog)
bool QXmlEditData::isAutoInsertProlog()
{
    return Config::getBool(Config::KEY_XML_AUTOINSERT_PROLOG, false);
}

void QXmlEditData::setAutoInsertProlog(const bool value)
{
    Config::saveBool(Config::KEY_XML_AUTOINSERT_PROLOG, value);
}

QString QXmlEditData::autoInsertPrologEncoding()
{
    return Config::getString(Config::KEY_XML_AUTOINSERT_ENCODING, "");
}

void QXmlEditData::setAutoInsertPrologEncoding(const QString &value)
{
    Config::saveString(Config::KEY_XML_AUTOINSERT_ENCODING, value);
}

QString QXmlEditData::insertPrologEncoding()
{
    return Config::getString(Config::KEY_XML_INSERT_ENCODING, defaultEncoding());
}

void QXmlEditData::setInsertPrologEncoding(const QString &value)
{
    Config::saveString(Config::KEY_XML_INSERT_ENCODING, value);
}


//--- endregion(prolog)


SearchManager *QXmlEditData::searchManager()
{
    return _searchManager;
}

//--- endregion(xslt)

//--- region(clipboard)

//Supported formats:
//    1 text (as XML)
//    2 internal element: using the key in our mime data equal to detect if we ca unse the interal pointer or resort to the textual representation.
//
//How to insert:
//    if internal object, use the object
//    if text:
//       if the text starts with <?xml, then decode it and use it, if possible
//       if not, create a fake root tag, paste the text and decode and use all of the contents

const QString QXmlEditData::QXmlEditMimeDataFormat("x-application-qxmledit");

ClipboardElementList::ClipboardElementList(bool newDeleteItems, QList<Element *> &newElements)
{
    _deleteItems = newDeleteItems;
    foreach(Element * el, newElements) {
        _elements.append(el);
    }
}

ClipboardElementList::~ClipboardElementList()
{
    if(_deleteItems) {
        foreach(Element * element, _elements) {
            if(NULL != element) {
                delete element ;
            }
        }
    }
}

QList<Element *> &ClipboardElementList::elements()
{
    return _elements;
}

//---------------------------------------------------------

void QXmlEditData::onClipboardDataChanged()
{
    const QMimeData *mimeData = QApplication::clipboard()->mimeData();
    if(mimeData->hasFormat(QXmlEditMimeDataFormat)) {
        QByteArray mimeDataArray = mimeData->data(QXmlEditMimeDataFormat);
        QString key = QString::fromUtf8(mimeDataArray);
        if(key != _clipBoardKey) {
            cleanClipBoardData();
        }
    } else {
        cleanClipBoardData();
    }
    emit clipboardDataChanged(!mimeData->text().isEmpty());
}

Element *QXmlEditData::internalClipBoardItem()
{
    if(NULL != _clipBoardItem) {
        return _clipBoardItem;
    }
    return NULL ;
}

ClipboardElementList *QXmlEditData::clipBoardItemList(const bool onlyElements)
{
    bool isDelete = true ;
    QList<Element*> result;
    if(NULL != _clipBoardItem) {
        result << _clipBoardItem;
        isDelete = false ;
    } else {
        QString scrapBookData = QApplication::clipboard()->text();
        if(!scrapBookData.isEmpty()) {
            bool isOnlyRoot = false;
            QString text ;
            if(scrapBookData.startsWith("<?xml")) {
                text = scrapBookData ;
                isOnlyRoot = true ;
            } else {
                text = "<root>";
                text += scrapBookData ;
                text += "</root>";
            }
            result = Regola::decodeXMLFromString(text, isOnlyRoot, onlyElements);
        }
    }
    ClipboardElementList * container = new ClipboardElementList(isDelete, result);
    return container ;
}

void QXmlEditData::setClipBoardItem(Element *value, const QString &textualRepresentation)
{
    cleanClipBoardData();
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(textualRepresentation);
    QString key ;
    _clipBoardKey = key.sprintf("%p#%p", value, qApp);
    mimeData->setData(QXmlEditMimeDataFormat, _clipBoardKey.toUtf8());
    QApplication::clipboard()->setMimeData(mimeData);
    _clipBoardItem = value ;

    setClipBoardActionsState();
}

void QXmlEditData::setClipBoardActionsState()
{
    bool isAction = false;
    if(NULL != _clipBoardItem) {
        isAction = true ;
    }
    // warning: rely on the clipboard signaling
    // it is dangerous if it is blocked
    emit clipboardDataChanged(isAction);
}

void QXmlEditData::cleanClipBoardData()
{
    if(_clipBoardItem != NULL) {
        delete _clipBoardItem;
    }
    _clipBoardItem = NULL ;
    _clipBoardKey = "" ;
}

//--- endregion(clipboard)

//--- region(files)

void QXmlEditData::setLastFileForOperation(const QString &lastFile)
{
    _filePathForOperation = lastFile ;
}

QString QXmlEditData::filePathForOperation(const QString &opFile)
{
    if(!opFile.isEmpty()) {
        return opFile;
    }
    if(!_filePathForOperation.isEmpty()) {
        return _filePathForOperation;
    }
    return SystemServices::userDocumentsDirectory();
}

QString QXmlEditData::sysFilePathForOperation(const QString &opFile)
{
    if(!opFile.isEmpty()) {
        return opFile;
    }
    return SystemServices::userDocumentsDirectory();
}

QStringList &QXmlEditData::lastFiles()
{
    return _lastFilesNames;
}

bool QXmlEditData::addFileAccess(const QString &entry)
{
    if(entry.isEmpty()) {
        return false ;
    }
    _lastFilesNames.removeOne(entry);
    _lastFilesNames.insert(0, entry);
    while(_lastFilesNames.length() > MaxEntriesLastFiles) {
        _lastFilesNames.removeLast();
    }
    if(!Config::saveStringArray(Config::KEY_MAIN_RECENTFILES, _lastFilesNames)) {
        return false;
    }
    return true;
}
//--- endregion(files)


//--- region(data)
DataInterface *QXmlEditData::storageManager()
{
    return _dataInterface;
}

void QXmlEditData::setStorageManager(DataInterface *value)
{
    _dataInterface = value ;
}

//--- endregion(data)

//--- region(indentxml)
int QXmlEditData::xmlIndent()
{
    return Config::getInt(Config::KEY_XML_INDENT, XmlIndentDefault);
}

void QXmlEditData::setXmlIndent(const int value)
{
    Config::saveInt(Config::KEY_XML_INDENT, value);
}

int QXmlEditData::xmlIndentAttributes()
{
    return Config::getInt(Config::KEY_XML_INDENT_ATTRIBUTES_COLUMNS, XmlIndentAttributesColumnsDefault);
}

void QXmlEditData::setXmlIndentAttributes(const int value)
{
    Config::saveInt(Config::KEY_XML_INDENT_ATTRIBUTES_COLUMNS, value);
}

QXmlEditData::EIndentAttributes QXmlEditData::xmlIndentAttributesType()
{
    return (QXmlEditData::EIndentAttributes)Config::getInt(Config::KEY_XML_INDENT_ATTRIBUTES_TYPE, XmlIndentAttributesTypeDefault);
}

void QXmlEditData::setXmlIndentAttributesType(const QXmlEditData::EIndentAttributes value)
{
    Config::saveInt(Config::KEY_XML_INDENT_ATTRIBUTES_TYPE, value);
}

//--- endregion(indentxml)

//--- region(xsdMode)
XSDManager *QXmlEditData::xsdManager()
{
    return _xsdManager;
}

//--- endregion(xsdMode)

//--- region(NamespaceManager)
NamespaceManager *QXmlEditData::namespaceManager()
{
    return _namespaceManager;
}
//--- endregion(NamespaceManager)

//--- region(multi-threading)
bool QXmlEditData::isMtEnabled()
{
    return Config::getBool(Config::KEY_MULTITHREADING_ENABLED, true);
}

void QXmlEditData::setMtEnabled(const bool value)
{
    Config::saveBool(Config::KEY_MULTITHREADING_ENABLED, value);
}
//--- endregion(multi-threading)

//--- region(styleVersion)
void QXmlEditData::updateStyleVersion()
{
    _styleVersion ++;
}

int QXmlEditData::styleVersion()
{
    return _styleVersion ;
}

//--- endregion(styleVersion)

UnicodeHelper *QXmlEditData::unicodeHelper()
{
    return _unicodeHelper ;
}

bool QXmlEditData::isShowImagesInTooltip()
{
    return Config::getBool(Config::KEY_ELEMENT_TEXT_TOOLTIP_IMAGE, false);
}
