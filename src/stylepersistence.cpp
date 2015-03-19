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

#include "xmlEdit.h"
#include "utils.h"
#include "stylepersistence.h"

#include <QFile>
#include <QDir>

#define STYLE_ROOT_ELEMENT          "style"
#define STYLESETENTRY_TAGNAME       "styles"
#define KEYWORDSETENTRY_TAGNAME     "keywords"
#define IDS_TAGNAME                 "ids"
#define STYLE_NAMESPACE   "namespace"

//-----------------------------------------------------------------------
#define STYLE_TAGNAME   "style"
#define STYLE_ID "id"
#define STYLE_COLOR "color"
#define STYLE_BACK_COLOR "backColor"
#define STYLE_FONTFAMILY "family"
#define STYLE_FONTSIZE "size"
#define STYLE_FONTBOLD "bold"
#define STYLE_FONTITALIC "italic"
#define STYLE_ICON "icon"
#define STYLE_TAGDEFAULT    "default"
//-----------------------------------------------------------------------

#define ATTRIBUTE_ENTITY "entity"
#define ATTRIBUTE_NAME "name"
#define ATTRIBUTE_OP "op"
#define ATTRIBUTE_TYPE "type"
#define ATTRIBUTE_VALUE "value"
#define ATTRIBUTE_CASE "case"
#define ATTRIBUTE_CONNECTOR "connector"
#define ATTRIBUTE_AXIS   "axis"
//-----------------------------------------------------------------------

#define KEYWORD_TAGNAME "keyword"
#define KEYWORD_IDSTYLE  "idStyle"
#define KEYWORD_KEYW     "keyword"
#define KEYWORD_TAGDEFAULT "default"
#define RULESET_TAGNAME  "ruleSet"
#define RULE_TAGNAME  "rule"
//-----------------------------------------------------------------------
#define IDATTRS_TAGNAME  "id"
#define ST_ID_STYLE      "id"
#define ST_ID_ALPHA      "alpha"

#define CONNECTOR_OR    "or"
#define RULE_A_ELEMENT  "element"
//-----------------------------------------------------------------------

StylePersistence::StylePersistence()
{
}

StylePersistence::~StylePersistence()
{

}

//-----------------------------------------------------------------------

bool StylePersistence::collectAnId(VStyle *style, QDomElement *element)
{
    QString id = element->attribute(ST_ID_STYLE, "");
    QString alpha = element->attribute(ST_ID_ALPHA, "");

    return style->addId(id, Utils::decodeBoolean(alpha));
}

bool StylePersistence::collectIds(VStyle *style, QDomNodeList &nodes)
{
    bool isOk = true ;
    int nodi = nodes.count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = nodes.item(i) ;
        D(printf("K1 trovato %d %s=%s\n", childNode.nodeType(), childNode.nodeName().toAscii().data(), childNode.nodeValue().toAscii().data()));

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(IDATTRS_TAGNAME == element.tagName()) {
                if(!collectAnId(style, &element)) {
                    isOk = false;
                }
            }
        }
    }
    return isOk ;
} // collectIds

//-----------------------------------------------------------------------------------------------------

StyleRule* StylePersistence::collectRule(QDomElement *element)
{
    QString entity = element->attribute(ATTRIBUTE_ENTITY, "").toLower();
    QString name = element->attribute(ATTRIBUTE_NAME, "");
    QString op = element->attribute(ATTRIBUTE_OP, "").toUpper();
    QString type = element->attribute(ATTRIBUTE_TYPE, "").toLower();
    QString value = element->attribute(ATTRIBUTE_VALUE, "");
    QString caseSensitive = element->attribute(ATTRIBUTE_CASE, "").toLower();
    QString axis = element->attribute(ATTRIBUTE_AXIS, "");

    StyleRule *rule = new StyleRule();
    if(NULL != rule) {
        rule->setEntity(entity == RULE_A_ELEMENT);
        rule->setName(name);
        rule->setOp(op);
        rule->setType(type);
        rule->setValue(value);
        rule->setCaseSensitive(caseSensitive != "false");
        rule->setAxis(axis);
    }
    return rule;
}//()

StyleRuleSet* StylePersistence::collectRuleSet(QDomElement *element)
{
    bool isAnd = true ;
    QString styleId = element->attribute(KEYWORD_IDSTYLE, "");
    QString connector = element->attribute(ATTRIBUTE_CONNECTOR, "");
    if(connector == CONNECTOR_OR) {
        isAnd = false;
    }
    StyleRuleSet *rSet = new StyleRuleSet();
    rSet->setConnectorAnd(isAnd);
    rSet->setIdStyle(styleId);
    int nodi = element->childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = element->childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            if(RULESET_TAGNAME == childElement.tagName()) {
                rSet->addRule(collectRuleSet(&childElement));
            } else if(RULE_TAGNAME) {
                rSet->addRule(collectRule(&childElement));
            }
        }
    }
    return rSet;
}//()

bool StylePersistence::collectAKeyword(VStyle *style, QDomElement *element)
{
    QString idStyle = element->attribute(KEYWORD_IDSTYLE, "");
    QString keyword = element->attribute(KEYWORD_KEYW, "");

    TokenEntry *token = new TokenEntry(keyword, NULL);
    if(NULL != token) {
        mapTokens.insertMulti(idStyle, token);
        if(style->addToken(token)) {
            return true ;
        }
    }
    return false;
}//()

bool StylePersistence::collectKeywords(VStyle *style, QDomNodeList &nodes)
{
    bool isOk = true ;
    int nodi = nodes.count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = nodes.item(i) ;
        D(printf("K1 trovato %d %s=%s\n", childNode.nodeType(), childNode.nodeName().toAscii().data(), childNode.nodeValue().toAscii().data()));

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(KEYWORD_TAGNAME == element.tagName()) {
                if(!collectAKeyword(style, &element)) {
                    isOk = false;
                }
            } else if(RULESET_TAGNAME) {
                style->addRuleSet(collectRuleSet(&element));
            }
        }
    }
    return isOk ;
} // collectKeywords

//-----------------------------------------------------------------------------------------------------

StyleEntry *StylePersistence::collectStyle(QDomElement *element)
{
    QString id = element->attribute(STYLE_ID, "");
    QString color = element->attribute(STYLE_COLOR, "");
    QString backColor = element->attribute(STYLE_BACK_COLOR, "");
    QString fontFamily = element->attribute(STYLE_FONTFAMILY, "");
    QString fontSize = element->attribute(STYLE_FONTSIZE, "");
    QString bold = element->attribute(STYLE_FONTBOLD, "");
    QString italic = element->attribute(STYLE_FONTITALIC, "");
    QString iconName = element->attribute(STYLE_ICON, "");

    StyleEntry *styleEntry = new StyleEntry(id);
    if(NULL != styleEntry) {
        styleEntry->setBold(Utils::decodeBoolean(bold));
        styleEntry->setItalic(Utils::decodeBoolean(italic));
        styleEntry->setFontSize(Utils::decodeInt(fontSize, 0));
        styleEntry->setFontFamily(fontFamily);
        styleEntry->setColor(color);
        styleEntry->setBackColor(backColor);
        styleEntry->setIconName(iconName);
    }
    return styleEntry;
}//()


bool StylePersistence::collectAStyle(VStyle *style, QDomElement *element)
{
    StyleEntry *styleEntry = collectStyle(element);
    if(NULL != styleEntry) {
        if(!style->addEntry(styleEntry)) {
            return false;
        }
        return true ;
    }
    return false;
}//()

bool StylePersistence::collectDefault(VStyle *style, QDomNodeList nodes)
{
    int defNodes = nodes.count();
    for(int nd = 0 ; nd < defNodes ; nd ++) {
        QDomNode defNode = nodes.item(nd) ;
        if(defNode.isElement()) {
            QDomElement elementDef = defNode.toElement();
            if(STYLE_TAGNAME == elementDef.tagName()) {
                style->setDefaultStyleEntry(collectStyle(&elementDef));
                return true ;
            }
        }
    }
    // no error
    return true;
}

bool StylePersistence::collectStyles(VStyle *style, QDomNodeList &nodes)
{
    bool isOk = true ;
    int nodi = nodes.count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = nodes.item(i) ;
        D(printf("K1 trovato %d %s=%s\n", childNode.nodeType(), childNode.nodeName().toAscii().data(), childNode.nodeValue().toAscii().data()));

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            if(STYLE_TAGNAME == element.tagName()) {
                if(!collectAStyle(style, &element)) {
                    isOk = false;
                }
            } else if(STYLE_TAGDEFAULT == element.tagName()) {
                collectDefault(style, element.childNodes());
            }// default
        }
    }
    return isOk ;
} // collectStyles


bool StylePersistence::scanStyleData(VStyle *style, QDomNode &rootNode)
{
    bool isOk = true ;
    int nodi = rootNode.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;
        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QDomNodeList childs = element.childNodes();
            if(STYLESETENTRY_TAGNAME == element.tagName()) {
                if(!collectStyles(style, childs)) {
                    isOk = false;
                }
            } else if(KEYWORDSETENTRY_TAGNAME == element.tagName()) {
                if(!collectKeywords(style, childs)) {
                    isOk = false;
                }
            } else if(IDS_TAGNAME == element.tagName()) {
                if(!collectIds(style, childs)) {
                    isOk = false;
                }
            }
        }//if
    }//for
    return isOk ;
}//scanStyleData()


bool StylePersistence::scanData(QVector<VStyle*> *styles, QDomNode &rootNode)
{
    bool isOk = true ;
    mapTokens.clear();
    int nodi = rootNode.childNodes().count();
    //D(printf("sono in assegna con %d nodi\n", nodi));
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            QString name = element.attribute("name", "");
            QString description = element.attribute("description", "");
            VStyle *style = new VStyle(name, description);
            QString nameSpace = element.attribute(STYLE_NAMESPACE, "");
            if(!nameSpace.isEmpty()) {
                style->setNamespace(nameSpace);
            }

            styles->append(style);
            if(STYLE_ROOT_ELEMENT == element.tagName()) {
                if(!scanStyleData(style, element)) {
                    isOk = false;
                }
            }
            if(isOk) {
                completeStyle(style);
            }
        }
    }//for
    return isOk ;
}//scanData()

bool StylePersistence::scanDataSingleStyle(VStyle* style, QDomNode &rootNode)
{
    bool isOk = true ;
    mapTokens.clear();
    int nodi = rootNode.childNodes().count();
    //D(printf("sono in assegna con %d nodi\n", nodi));
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = rootNode.childNodes().item(i) ;

        if(childNode.isElement()) {
            QDomElement element = childNode.toElement();
            //QString name = element.attribute("name", "");
            //QString description = element.attribute("description", "");
            QString nameSpace = element.attribute("namespace", "");
            if(!nameSpace.isEmpty()) {
                style->setNamespace(nameSpace);
            }
            if(STYLE_ROOT_ELEMENT == element.tagName()) {
                if(!scanStyleData(style, element)) {
                    isOk = false;
                }
            }
            if(isOk) {
                completeStyle(style);
            }
        }
    }//for
    return isOk ;
}//scanData()


void StylePersistence::completeStyle(VStyle *style)
{
    foreach(QString key, mapTokens.keys()) {
        QList<TokenEntry*>values = mapTokens.values(key);
        StyleEntry *e = style->getStyleEntry(key);
        foreach(TokenEntry * token, values) {
            token->setStyle(e);
        }
    }
}

bool StylePersistence::readStyleFile(QVector<VStyle*> *styles, const QString & filePath)
{
    bool isOk = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            isOk = scanData(styles, document);
        } else {
            Utils::error(tr("Unable to parse XML"));
        }
        file.close();
    } else {
        Utils::error(QString(tr("Unable to load file.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}

bool StylePersistence::readStyleFile(VStyle* style, const QString & filePath)
{
    bool isOk = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            isOk = scanDataSingleStyle(style, document);
        } else {
            Utils::error(tr("Unable to parse XML"));
        }
        file.close();
    } else {
        Utils::error(QString(tr("Unable to load file.\n Error code is '%1'")).arg(file.error()));
    }
    return isOk ;
}


bool StylePersistence::scanDirectory(const QString &dirName, QVector<VStyle*> *styles)
{
    bool isOK = true ;
    QDir dir(dirName);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList names;
    names.append("*.style");
    dir.setNameFilters(names);
    QFileInfoList fileList = dir.entryInfoList();
    foreach(QFileInfo info, fileList) {
        if(!readStyleFile(styles, info.absoluteFilePath())) {
            isOK = false ;
        }
    }
    return isOK;
}

