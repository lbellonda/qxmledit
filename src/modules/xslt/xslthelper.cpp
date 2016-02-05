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

#include "xslthelper.h"
#include <QAction>
#include "utils.h"
#include "xmlutils.h"
#include "xmleditwidgetprivate.h"
#include "modules/xslt/xsltelementdialog.h"
#include "modules/xslt/saxnamesscan.h"

//-------------------------------------------------------------

XslLevel::XslLevel()
{
    isXsl = false ;
}

//-------------------------------------------------------------

XslContext::XslContext()
{
    inTemplate = false ;
}

XslContext::~XslContext()
{
    foreach(XslLevel * level, levels) {
        delete level;
    }
    levels.clear();
}

//-------------------------------------------------------------

const QString XsltHelper::TemplateTag = "template";

XsltHelper::XsltHelper(XmlEditWidgetPrivate *owner, QObject *parent) :
    QObject(parent)
{
    _owner = owner ;
    _inited = false;
}


XsltHelper::~XsltHelper()
{
    // elements belong to the manager
    _elementsByTag.clear();
}

void XsltHelper::init()
{
    if(_inited) {
        return ;
    }
    // builds the map
    buildMap();
    _inited = true ;
}


void XsltHelper::buildMap()
{
    QMap<QString, XsltElement*> &data = _owner->appData()->xsltManager()->elementsByTag();
    foreach(QString key, data.keys()) {
        _elementsByTag[key] = data[key];
    }
}


void XsltHelper::addElement(XsltElement *el)
{
    _elementsByTag[el->tagName()] = el ;
}

// maybe a Signal mapper?
void XsltHelper::loadAction(QMenu *contextMenu, QAction *action, XsltElement *el)
{
    action->setData(el->tagName());
    contextMenu->addAction(action);
}

void XsltHelper::addMenuItems(Element *selectedElement, QMenu *contextMenu, const QString &namespacePrefix)
{
    init();
    XslContext *context = findContext(selectedElement, namespacePrefix);
    if(NULL == context) {
        Utils::errorOutOfMem(NULL);
        return ;
    }
    // First: direct children:
    bool hasChildren = false;
    bool firstChild = true;
    foreach(XsltElement * el, _elementsByTag.values()) {
        if((NULL != el) && el->use) {
            if(!el->isInsertAtTop() && el->isInsertableAsChild(context, _elementsByTag)) {
                if(firstChild) {
                    firstChild = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5,1,0)) && !defined(ENVIRONMENT_MACOS)
                    contextMenu->addSection(QString("   %1:").arg(tr("Insert as Child")));
#else
                    contextMenu->addAction(new QAction(QString("   %1:__").arg(tr("Insert as Child")), contextMenu));
#endif
                }
                // insert(contesto padre)+append(alla selezione)
                QAction *newAction = new QAction(tr("'%1'").arg(el->tagName()), contextMenu);
                newAction->setObjectName(QString("I%1").arg(el->tagName()));
                loadAction(contextMenu, newAction, el);
                connect(newAction, SIGNAL(triggered()), this, SLOT(onActionInsert()));
                hasChildren = true;
            }
        }
    }
    if(hasChildren) {
        contextMenu->addSeparator();
    }
    bool hasAppend = false;
    bool firstAppend = true;
    foreach(XsltElement * el, _elementsByTag.values()) {
        if((NULL != el) && el->use) {
            if(!el->isInsertAtTop()) {
                if(el->isAppendable(context, _elementsByTag)) {
                    if(firstAppend) {
                        firstAppend = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5,1,0)) && !defined(ENVIRONMENT_MACOS)
                        contextMenu->addSection(QString("   %1:").arg(tr("Append as Sibling")));
#else
                        contextMenu->addAction(new QAction(QString("   %1:__").arg(tr("Append as Sibling")), contextMenu));
#endif
                    }
                    QAction *newAction = new QAction(tr("'%1'").arg(el->tagName()), contextMenu);
                    newAction->setObjectName(QString("A%1").arg(el->tagName()));
                    loadAction(contextMenu, newAction, el);
                    connect(newAction, SIGNAL(triggered()), this, SLOT(onActionAppend()));
                    hasAppend = true ;
                }
            }
        }
    }
    if(hasAppend) {
        contextMenu->addSeparator();
    }
    bool firstFirstLevel = true;
    foreach(XsltElement * el, _elementsByTag.values()) {
        if((NULL != el) && el->use) {
            if(el->isInsertAtTop()) {
                if(firstFirstLevel) {
                    firstFirstLevel = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5,1,0)) && !defined(ENVIRONMENT_MACOS)
                    contextMenu->addSection(QString("   %1:").arg(tr("Insert Top Level")));
#else
                    contextMenu->addAction(new QAction(QString("   %1:__").arg(tr("Insert Top Level")), contextMenu));
#endif
                }
                QAction *newAction = new QAction(tr("'%1'").arg(el->tagName()), contextMenu);
                loadAction(contextMenu, newAction, el);
                newAction->setObjectName(QString("I%1").arg(el->tagName()));
                connect(newAction, SIGNAL(triggered()), this, SLOT(onActionInsert()));
            }
        }
    }

    //-------
    delete context;
}

bool XsltHelper::isTemplate(const QString &tag)
{
    return tag == TemplateTag ;
}

XslContext *XsltHelper::findContext(Element *selectedElement, const QString &nsPrefix)
{
    XslContext *context = new XslContext();
    if(NULL != context) {
        Element *parent = selectedElement ;
        while(NULL != parent) {
            XslLevel *level = new XslLevel();
            if(NULL == level) {
                delete context ;
                return NULL;
            }
            QString tag = parent->tag();
            level->tag = tag ;
            context->levels.append(level);
            if(XmlUtils::hasPrefix(tag, nsPrefix)) {
                level->isXsl = true ;
                level->simpleTag = XmlUtils::stripNs(tag);
                if(isTemplate(level->simpleTag)) {
                    context->inTemplate = true ;
                }
            }
            parent = parent->parent();
        }
    }
    return context;
}


XsltElement *XsltHelper::dataFromAction()
{
    QAction * action = qobject_cast<QAction *>(sender());
    QString key = action->data().toString();
    return dataFromString(key);
}

XsltElement *XsltHelper::dataFromString(const QString &key)
{
    XsltElement *el = _elementsByTag[key];
    return el;
}


void XsltHelper::onActionInsert()
{
    XsltElement *el = dataFromAction();
    if(NULL == el) {
        return ;
    }
    doAction(true, el);
}

void XsltHelper::onActionAppend()
{
    XsltElement *el = dataFromAction();
    if(NULL == el) {
        return ;
    }
    doAction(false, el);
}

void XsltHelper::doAction(const bool isInsert, XsltElement *el)
{
    if(NULL == el) {
        return ;
    }
    CopyAttributesSession *cas = NULL ;
    // open up dialog
    //QStringList pathList;
    XsltElementDialogParam params;
    if(prepareInsertElement(&params, isInsert, el)) {
        cas = XsltElementDialog::dialogProperties(&params);
        if(NULL == cas) {
            if(NULL != params.element) {
                delete params.element;
            }
            return ;
        }
        insertElement(&params, cas, isInsert);
        if(NULL != cas) {
            cas->clear();
            delete cas ;
        }
    }
}

QString XsltHelper::xsltQName(const QString &name)
{
    QString prefix = _owner->namespacePrefixXslt();
    QString tagTemplate = name;
    if(!prefix.isEmpty()) {
        tagTemplate = prefix + ":" + tagTemplate  ;
    }
    return tagTemplate;
}

bool XsltHelper::updateParamaters(Element *source, const bool isInsert, const QString &nameAttribute)
{
    bool isChanged = false;
    QMap<QString, QString> templateParameters;
    QString tagTemplate = xsltQName("call-template");
    if(source->tag() == tagTemplate) {
        QString templateName = nameAttribute;
        Element *target = findTemplate(templateName);
        if(NULL != target) {
            // get the parameters
            QString tagParam = xsltQName("param");
            foreach(Element * child, *target->getChildItems()) {
                if(child->isElement() && (child->tag() == tagParam)) {
                    QString name = getXslName(child);
                    if(!name.isEmpty()) {
                        templateParameters.insert(name, name);
                    }
                }// if param
            } // foreach
            Element *lastWithParameter = NULL ;
            QString tagWithParam = xsltQName("with-param");
            foreach(Element * child, *source->getChildItems()) {
                if(child->isElement() && (child->tag() == tagWithParam)) {
                    QString name = getXslName(child);
                    if(!name.isEmpty()) {
                        if(templateParameters.keys().contains(name)) {
                            templateParameters.remove(name);
                            lastWithParameter = child ;
                        }
                    }
                }
            }// for each

            // adding missing parameters
            foreach(QString key, templateParameters.keys()) {
                Element *newParam = new Element(tagWithParam, "", source->getParentRule());
                newParam->setAttribute("name", key);
                newParam->markEdited();
                if(NULL == lastWithParameter) {
                    source->addChildAt(newParam, 0);
                } else {
                    source->addChildAfter(newParam, lastWithParameter);
                }
                isChanged = true ;
                lastWithParameter = newParam ;
            }
            // ----
        } // null target
    } else {//source
        QString tagChoose = xsltQName("choose");
        if(isInsert && (source->tag() == tagChoose)) {
            Element *when = new Element(xsltQName("when"), "", source->getParentRule());
            when->setAttribute("test", "");
            when->markEdited();
            source->addChildAt(when, 0);

            Element *otherwise = new Element(xsltQName("otherwise"), "", source->getParentRule());
            source->addChildAt(otherwise, 1);
            otherwise->markEdited();

            isChanged = true ;
        }
    }// source
    return isChanged ;
}


void XsltHelper::insertElement(XsltElementDialogParam *params, CopyAttributesSession *cas, const bool isInsert)
{
    XsltElement *el = params->xsltElement ;
    Element *newElementToInsert = params->element;
    if(params->useParams) {
        updateParamaters(params->element, isInsert, cas->attributeValue("name"));
    }
    Element *selectedElement = params->selectedElement;
    bool isModified = true ;
    if(params->xsltElement->isClearAttributes) {
        // remove empty attributes
        cas->removeEmptyAttributes();
    }
    newElementToInsert->pasteAttributes(cas->attributes(), NULL, isModified);
    newElementToInsert->markEdited();

    Regola *regola = _owner->getRegola();

    // find the correct place and insert it
    if(el->isInsertAtTop()) {
        _owner->insertElementComplete(newElementToInsert, regola->root());
    } else {
        if(isInsert) {
            if(el->isInsertAnywhere()) {
                Utils::TODO_NEXT_RELEASE("think to assert that the selection is not changed from the previous step");
                _owner->insertElementComplete(newElementToInsert, selectedElement);
            } else {
                // look for a specific position
                Element *parentXsl = selectedElement;
                if(!isXSLTElement(parentXsl)) {
                    Utils::error(_owner->getEditor()->window(), tr("The selection is not a valid XSLT element."));
                } else {
                    Element *lastSibling = findLastSibling(parentXsl, el);
                    if(NULL != lastSibling) {
                        _owner->appendElementComplete(newElementToInsert, lastSibling);
                    } else {
                        _owner->insertElementComplete(newElementToInsert, selectedElement);
                    }
                }
            }
        } else {
            // is append
            if(el->isInsertAnywhere()) {
                _owner->appendElementComplete(newElementToInsert, selectedElement);
            } else {
                // find the specific position, this should be checked when creating the menu
                Element *parentContextElement = selectedElement->parent();
                if(NULL != parentContextElement) {
                    Element *lastSibling = findLastSibling(parentContextElement, el, true);
                    _owner->appendElementComplete(newElementToInsert, lastSibling);
                } else {
                    // log no work to do
                    Utils::message("No suitable place to appent the element");
                }
            }
        }
    }
}

bool XsltHelper::prepareInsertElement(XsltElementDialogParam *params, const bool isInsert, XsltElement *el)
{
    if(NULL == el) {
        return false ;
    }
    Regola *regola = _owner->getRegola();
    if(NULL == regola->root()) {
        Utils::error(tr("Need a root element."));
        return false ;
    }
    Element *selectedElement = _owner->getSelectedItem();
    if(!el->isInsertAtTop() && (NULL == selectedElement)) {
        Utils::error(tr("Select an item to insert the new data."));
        return false ;
    }
    //------------------------
    Element *parentForPath = NULL ;
    Element *baseElement = NULL ;
    if(el->isInsertAtTop()) {
        parentForPath = regola->root();
        baseElement = parentForPath;
    } else {
        if(isInsert) {
            if(el->isInsertAnywhere()) {
                parentForPath = selectedElement ;
                baseElement = selectedElement;
            } else {
                // look for a specific position
                Element *parentXsl = selectedElement;
                Element *lastSibling = findLastSibling(parentXsl, el);
                if(NULL != lastSibling) {
                    parentForPath = lastSibling->parent();
                    baseElement = lastSibling ;
                } else {
                    parentForPath = selectedElement;
                    baseElement = selectedElement ;
                }
            }
        } else {
            // is append
            if(el->isInsertAnywhere()) {
                parentForPath = selectedElement->parent();
                baseElement = selectedElement ;
            } else {
                // find the specific position, this should be checked when creating the menu
                Element *parentContextElement = selectedElement->parent();
                if(NULL != parentContextElement) {
                    Element *lastSibling = findLastSibling(parentContextElement, el);
                    if(NULL == lastSibling) {
                        parentForPath = selectedElement;
                        baseElement = selectedElement ;
                        /*Utils::error(_owner->getMainTreeWidget()->window(), tr("Invalid XSLT structure for the operation."));
                        return false;*/
                    } else {
                        parentForPath = lastSibling->parent();
                        baseElement = lastSibling ;
                    }
                }
            }
        }
    }
    if(NULL != parentForPath) {
        params->embeddedPath = parentForPath->path();
        params->path = &params->embeddedPath;
    }
    //-----------------------

    QString tag = el->tagName();
    QString prefix =  _owner->namespacePrefixXslt();
    if(!prefix.isEmpty()) {
        tag = prefix + ":" + tag;
    }
    Element *newElementToInsert = new Element(tag, "", _owner->getRegola());
    params->selectedElement = selectedElement ;
    params->xsltElement = el;
    params->element = newElementToInsert ;
    params->baseElement = baseElement ;
    params->helper = this ;
    params->path = &params->embeddedPath;
    params->parent = _owner->getEditor()->window() ;
    return true;
}

/**
 * @brief XsltHelper::findLastSibling
 * @abstract finds the element that will be before the new one.
 * @param selectedElement
 * @param parentElement
 * @param el
 * @return the element before the new one
 */
Element *XsltHelper::findLastSibling(Element *parentElement, XsltElement *el, const bool useLast)
{
    Element *lastChildren = NULL ;
    QString prefix = _owner->namespacePrefixXslt();
    QString tag = el->tagName();
    if(!prefix.isEmpty()) {
        tag = prefix + ":" + tag ;
    }

    foreach(Element * element, *parentElement->getChildItems()) {
        if(element->isElement()) {
            if(element->tag() == tag) {
                lastChildren = element;
            } else {
                if(!useLast) {
                    return lastChildren ;
                }
                lastChildren = element ;
            }
        }
    }
    return lastChildren ;
}

/*

if the xsl parent can have children (top level excluded)

get elements to insert

find xsl parent
-foreach parent, check children

if the type is inside a sequence, insert the new one as the last one.
is no sequence exists, insert it as first

*/

void XsltHelper::editElement(Element *selectedElement)
{
    if(NULL == selectedElement) {
        return ;
    }
    XsltElementDialogParam params;
    if(!prepareEditElement(selectedElement, &params)) {
        return ;
    }
    CopyAttributesSession *attributesCopied = XsltElementDialog::dialogProperties(&params);
    if(NULL == attributesCopied) {
        return ;
    }
    // update the element and insert it in the undo chain.
    execEditElement(attributesCopied, selectedElement, params.xsltElement->isClearAttributes, params.useParams);
    // update the element and insert it in the undo chain.
    delete attributesCopied ;
}

void XsltHelper::execEditElement(CopyAttributesSession *attributesCopied, Element *selectedElement, const bool isClear, const bool isUpdateParameters)
{
    // update the element and insert it in the undo chain.
    if(isClear) {
        _owner->getRegola()->pasteClearAttributesInternals(_owner->getEditor(), selectedElement, attributesCopied);
    } else {
        _owner->getRegola()->pasteAttributesInternals(_owner->getEditor(), selectedElement, attributesCopied);
    }
    // update the element and insert it in the undo chain.
    if(isUpdateParameters) {
        if(updateParamaters(selectedElement, false, attributesCopied->attributeValue("name"))) {
            _owner->getRegola()->emptyUndoStack();
            _owner->getRegola()->refreshChildrenOfElement(selectedElement, _owner->getEditor());
        }
    }
}


bool XsltHelper::prepareEditElement(Element *selectedElement, XsltElementDialogParam *params)
{
    if(NULL == selectedElement) {
        return false ;
    }
    Regola *regola = _owner->getRegola();
    if(NULL == regola->root()) {
        Utils::error(tr("Need a root element."));
        return false ;
    }
    // find the reference element for the symbols autocompletion
    int pos = selectedElement->indexOfSelfAsChild();
    Element *baseElement = NULL ;
    if((pos > 0) && (NULL != selectedElement->parent())) {
        baseElement = selectedElement->parent()->getChildAt(pos - 1);
    } else {
        baseElement = selectedElement->parent();
    }
    //------------------------
    params->embeddedPath = selectedElement->parentPath();
    //-----------------------

    // open up dialog
    QString tag = selectedElement->tag();
    QString prefix =  _owner->namespacePrefixXslt();
    if(!prefix.isEmpty()) {
        QString pre = prefix + ":";
        if(tag.startsWith(pre)) {
            tag = tag.mid(pre.length());
        }
    }
    XsltElement *el = NULL ;
    init();
    if(_elementsByTag.contains(tag)) {
        el = _elementsByTag[tag];
    }

    if(NULL == el) {
        Utils::error(_owner->getEditor()->window(), tr("Invalid XSLT element: '%1'").arg(tag));
        return false ;
    }
    params->xsltElement = el ;
    params->element = selectedElement ;
    params->baseElement = baseElement ;
    params->helper = this ;
    params->path = &params->embeddedPath;
    params->parent = _owner->getEditor()->window();

    return true;
}


bool XsltHelper::loadNamesFromFile(const QString &fileName)
{
    SaxNamesScan handler(this);
    QXmlSimpleReader reader;
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    bool isOk = true ;
    QXmlInputSource xmlInput(&file);
    if(!reader.parse(xmlInput)) {
        isOk = false  ;
    }
    file.close();
    return isOk ;
}

bool XsltHelper::loadNames()
{
    QString filePath = QFileDialog::getOpenFileName(
                           _owner->getEditor()->window(), tr("Open File to Associate"),
                           QXmlEditData::sysFilePathForOperation(_owner->getRegola()->fileName()),
                           Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        if(!loadNamesFromFile(filePath)) {
            Utils::error(_owner->getEditor()->window(), tr("Error reading data."));
            return false;
        }
        return true;
    }
    return false;
}

bool XsltHelper::isXSLTElement(Element* el)
{
    if(NULL == el) {
        return false;
    }
    QString prefix = _owner->namespacePrefixXslt();
    if(!prefix.isEmpty()) {
        QString tag = prefix + ":" ;
        return el->tag().startsWith(tag);
    } else {
        return el->tag().contains(":");
    }
}
