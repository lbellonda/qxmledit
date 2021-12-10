/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef MAINWNDCONTROLLER_H
#define MAINWNDCONTROLLER_H

#include "xmlEdit.h"

class MainWindow;
class AnonContext;
class AnonAlg;
class AnonymizeParameters;
class Regola;
class ReplicaCloneInfo;
class Element;
class SourceMessage;
class ApplicationData;
class XSDToXML;
class XMLToXSD;

class ReplicaInfoProvider
{
public:
    ReplicaInfoProvider();
    virtual ~ReplicaInfoProvider();
    virtual ReplicaCloneInfo *getCloneInfo(QWidget *parent, Element *element) = 0;
};

class XSDTopElementChooser
{
public:
    virtual ~XSDTopElementChooser();
    virtual QString selectTopLevelSchemaElement(Regola *regola) = 0;
};

class XMLVsXSDFactory
{
public:
    virtual ~XMLVsXSDFactory();
    virtual XMLToXSD* newXML2XSD(ApplicationData *appData) = 0;
    virtual XSDToXML* newXSD2XML(ApplicationData *appData) = 0;
};

class MainWndController : public QObject, ReplicaInfoProvider, XSDTopElementChooser, XMLVsXSDFactory
{
    Q_OBJECT
    MainWindow *_w;
    ReplicaInfoProvider *_replicaInfoProvider;
    XSDTopElementChooser *_XSDTopElementChooser;
    XMLVsXSDFactory *_XMLVsXSDFactory;

    bool anonymizeGetParams(AnonymizeParameters *params);

    void innerAnonymize(AnonAlg *alg);
    void innerAnonymize(AnonContext *context);
    QString selectTopLevelSchemaElement(Regola *regola);

    ReplicaCloneInfo * getCloneInfo(QWidget *parent, Element *element);
    MainWindow* createEditorFromXSD2XML(XSDToXML *xsdToXml);
    // interface
    virtual XMLToXSD* newXML2XSD(ApplicationData *appData);
    virtual XSDToXML* newXSD2XML(ApplicationData *appData);

public:
    explicit MainWndController(QObject *parent = 0);
    virtual ~MainWndController();

    void setWindow(MainWindow *newWindow);
    //-----
    void onAnonymize();
    void onAnonymizeFile();
    void anonymize(AnonAlg *alg);
    void anonymize(AnonContext *context);
    void onAnonymize(AnonContext *context);
    bool actionInfo();
    //---
    QString askNewXSIType(Element *selection);
    bool isOpenInNewWindow();
    void createDocumentFromSnippet(Regola* newRegola);
    bool createDocumentFromResources(const QString &path);
    ReplicaInfoProvider *setReplicaInfoProvider(ReplicaInfoProvider *theProvider);
    bool cloneReplica();
    void removePrefix();
    void setPrefix();
    void removeNamespace();
    void setNamespace();
    void replacePrefix();
    void replaceNamespace();
    void normalizeNamespace();
    bool checkSCXML();
    void sourceDecode(QBuffer *dataStream, QList<SourceMessage*> &errors, Regola *regola);
    QBuffer* getDataForSourceDecode();
    void testXML();
    //-----
    MainWindow* generateXSDFromData();
    MainWindow* generateDataFromXSD();
    bool setXSDTopElementChooser(XSDTopElementChooser *newChooser);
    bool setXMLVsXSDFactory(XMLVsXSDFactory *newFactory);

signals:

public slots:

};

#endif // MAINWNDCONTROLLER_H
