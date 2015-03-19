/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef XSDLOADCONTEXT_H
#define XSDLOADCONTEXT_H

#include <QString>
#include <QList>
#include <QHash>

enum EXSDLoadError {
    XSD_LOAD_NOERROR, //0
    XSD_LOAD_GENERICERROR, //1
    XSD_LOAD_ERROR_ATTRIBUTE_FIXED_AND_USE, //2
    XSD_LOAD_ERROR_ATTRIBUTE_DEFAULT_AND_USE, //3
    XSD_LOAD_ERROR_ATTRIBUTE_NAME_AND_REF, //4
    XSD_LOAD_ERROR_ATTRIBUTE_REF_AND_TYPE, //5
    XSD_LOAD_ERROR_ATTRIBUTE_TYPE_AND_SIMPLETYPE, //6
    XSD_LOAD_ERROR_ATTRIBUTE_REF_GLOBAL, //7
    XSD_LOAD_ERROR_ATTRIBUTE_NAME_GLOBAL, //8
    XSD_LOAD_ERROR_ELEMENT_DEFAULT_AND_FIXED, //9
    XSD_LOAD_ERROR_ELEMENT_NAME_AND_REF, //10
    XSD_LOAD_ERROR_ELEMENT_REF_AND_TYPE, //11
    XSD_LOAD_ERROR_ELEMENT_SIMPLE_AND_COMPLEX_TYPE,  // 12
    XSD_LOAD_ERROR_ELEMENT_ABSTRACT, // 13
    XSD_LOAD_ERROR_TYPE_TOP_NAME_NOT_SET, // 14
    XSD_LOAD_ERROR_TYPE_TOP_REF_SET, //15
    XSD_LOAD_ERROR_TYPE_AN_NAME_SET //16
};

/**
 * @brief The EXSDLoadErrorPolicy enum
 * describe how to behave in case of errors when loading schema
 */
enum EXSDLoadErrorPolicy {
    XSD_LOADPOLICY_ERRORSTOP,
    XSD_LOADPOLICY_CONTINUE
};


class XSDOpError
{

    EXSDLoadError _errorCode;
    QString _msg;
public:
    XSDOpError();
    ~XSDOpError();
    //---
    EXSDLoadError errorCode() const;
    void setErrorCode(const EXSDLoadError &errorCode);
    QString msg() const;
    void setMsg(const QString &msg);
};

class Element ;
class XSchemaObject;

class XSDLoadContext
{
    EXSDLoadErrorPolicy _errorPolicy;
    QList<XSDOpError*> _messages;
    EXSDLoadError _lastError;
    QHash<QString, XSchemaObject*> _keyMap;
    bool _loadKeys;

public:
    XSDLoadContext();
    ~XSDLoadContext();

    void reset();
    bool isPolicyThrowError();

    bool hasErrors();
    int errorsCount();
    XSDOpError* errorAt(const int index);
    EXSDLoadErrorPolicy errorPolicy() const;
    void setErrorPolicy(const EXSDLoadErrorPolicy &errorPolicy);

    void addError(const EXSDLoadError errorCode, const QString &msg);
    bool containsCode(const EXSDLoadError errorCode);

    QHash<QString, XSchemaObject*> *inputKeyMap();
    void setKey(const QString &key, XSchemaObject *ptr);
    XSchemaObject *findObjectForKey(const QString &key);
    bool loadKeys() const;
    void setLoadKeys(bool loadKeys);

    void cloneSettingsFrom(XSDLoadContext *theTemplate);
};

#endif // XSDLOADCONTEXT_H
