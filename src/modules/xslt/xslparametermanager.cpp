/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017-2018 by Luca Bellonda and individual contributors  *
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

#include "xslparametermanager.h"
#include "modules/namespace/nscontext.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/xslt/xslthelper.h"
#include "modules/xml/xmlloadcontext.h"
#include "regola.h"
#include "xmlutils.h"
#include "utils.h"

XSLParameterManager::XSLParameterManager()
{
}

XSLParameterManager::~XSLParameterManager()
{
}

bool XSLParameterManager::extractParametersFromString(const QString &literal)
{
    bool isOk = false;
    QByteArray stringAsUtf8 = literal.toUtf8();
    QBuffer file(&stringAsUtf8);
    if(file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xmlReader ;
        xmlReader.setDevice(&file);
        XMLLoadContext context;
        Regola model("");
        // XML rules check for real file encoding
        if(model.readFromStream(&context, &xmlReader)) {
            scanForParameters(&model);
            isOk = true ;
        }
        file.close();
        return isOk ;
    } else {
        return false;
    }
}

bool XSLParameterManager::extractParametersFromFile(const QString &filePath)
{
    bool isOk = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xmlReader ;
        xmlReader.setDevice(&file);
        XMLLoadContext context;
        Regola model(filePath);
        if(model.readFromStream(&context, &xmlReader)) {
            scanForParameters(&model);
            isOk = true ;
        }
        file.close();
        return isOk ;
    } else {
        return false;
    }
}

void XSLParameterManager::scanForParameters(Regola *regola)
{
    _parameters.clear();
    Element *root = regola->root();
    if(NULL != root) {
        NSContext contextRoot(NULL);
        root->handleNamespace(&contextRoot);
        foreach(Element *element, *root->getChildItems()) {
            if(element->isElement()) {
                NSContext thisContext(&contextRoot);
                element->handleNamespace(&thisContext);
                QString name, prefix;
                XmlUtils::decodeQualifiedName(element->tag(), prefix, name);
                const QString &elementNamespace = thisContext.uriFromPrefix(prefix);
                if((elementNamespace == NamespaceManager::XSL1Namespace)
                        && (name == XsltHelper::ParameterTag)) {
                    QString name = element->getAttributeValue(XsltHelper::NameTag);
                    if(!name.isEmpty()) {
                        _parameters.insert(name);
                    }
                }
            }
        }
    }
}

QSet<QString> XSLParameterManager::parameters()
{
    return _parameters;
}
