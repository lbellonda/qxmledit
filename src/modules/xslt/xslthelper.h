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

#ifndef XSLTHELPER_H
#define XSLTHELPER_H

#include <QObject>
#include <QHash>

#include "modules/xslt/xsltmode.h"
#include "regola.h"

class XmlEditWidgetPrivate;
class XsltElementDialogParam;
class HandlerForInsert;

class XsltHelper : public QObject
{
    Q_OBJECT

    QMap<QString, XsltElement*> _elementsByTag;
    bool _inited;
    XmlEditWidgetPrivate *_owner;
    QSet<QString> _namesForAutocompletion;


    static const QString TemplateTag ;

    void init();
    void addElement(XsltElement *el);
    void buildMap();
    XslContext *findContext(Element *selectedElement, const QString &nsPrefix);
    //------------
    XsltElement *dataFromAction();
    void doAction(const bool isInsert, XsltElement *el, Element *selElement);
    void loadAction(QMenu *contextMenu, QAction *action, XsltElement *el);
    bool isTemplate(const QString &tag);
    Element *findLastSibling(Element *parentElement, XsltElement *el, const bool useLast = false);
    bool isXsdElement(Element *referenceElement, const QString &prefix);
    bool isXsdElementOfKind(Element *element, const QString &prefix, const QString &kind);
    Element *findRootTag(const QString &nameToFind, const QString &typeName);
    bool prepareEditElement(Element *selectedElement, XsltElementDialogParam *params);
    XsltElement *dataFromString(const QString &key);
    void insertElement(XsltElementDialogParam *params, CopyAttributesSession *cas, const bool isInsert);
    bool prepareInsertElement(XsltElementDialogParam *params, const bool isInsert, XsltElement *el, Element *selElement);
    void execEditElement(CopyAttributesSession *attributesCopied, Element *selectedElement, const bool isClear, const bool isUseParams);
    bool updateParamaters(Element *source, const bool isInsert, const QString &nameAttribute);

    bool loadNamesFromFile(const QString &fileName);
    QSet<QString> topLevelNames(const QString &nameToSearch);
    QHash<QString, Element*> topLevelNamesRef(const QString &nameToSearch);
    QString xsltQName(const QString &name);
public:
    explicit XsltHelper(XmlEditWidgetPrivate *owner, QObject *parent = 0);
    virtual ~XsltHelper();

    void addMenuItems(Element *selectedElement, QMenu *contextMenu, const QString &nameSpacePrefix);

    void editElement(Element *selectedElement);

    QSet<QString> allNames(Element *referenceElement);
    QSet<QString> parameterNames(const QString &templateName);
    QSet<QString> templateNames();
    QSet<QString> functionNames();
    QHash<QString, Element *> templateNamesMap();
    QHash<QString, Element *> functionNamesMap();
    Element *getTemplateParent(Element *referenceElement);
    XsltElement *elementFromName(const QString tag);
    QString getXslName(Element *el);

    void addNameForAutocompletion(const QString &newName);
    bool loadNames();
    bool hasAutoCompletionNames();

    Element *findTemplate(const QString &nameToFind);
    Element *findFunction(const QString &nameToFind);

    bool isXSLTElement(Element* el);
    //--
    HandlerForInsert *findElementsForInsert(Element *selectedElement, const QString &namespacePrefix);
    HandlerForInsert *findElementsForAppend(Element *selectedElement, const QString &namespacePrefix);
    void execOperation(const QString &id, const bool isInsert, Element *selElement);

signals:

public slots:

private slots:
    void onActionInsert();
    void onActionAppend();

    friend class TestXSLTMode ;
};

#endif // XSLTHELPER_H
