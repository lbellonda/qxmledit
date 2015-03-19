/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#include "compareresulttextformat.h"
#include "utils.h"

// Used in this order.
#define CLR_ADDED   "#484"      //1
#define CLR_DELETED   "#F42"    //2
#define CLR_EQUAL   "#888"      //3
#define CLR_MODIFIED   "#CA2"   //4


#define CSS_TEXT    "\
<style type='text/css'>\
body {                  \
   font-size: 10pt;     \
   background-color:#FFF;\
}                       \
.Ta {                   \
    color: %1;        \
    font-family: monospace; \
    font-style:italic;  \
    font-size:80%;  \
}                      \
.Tm {                   \
    color: %4;        \
    font-family: monospace; \
    font-style:italic;  \
    font-size:80%;  \
}                      \
.Te {                   \
   color: %3;        \
    font-family: monospace; \
   font-style:italic;  \
    font-size:80%;  \
}                      \
.Td {                   \
   color: %2;        \
    font-family: monospace; \
   font-style:italic;  \
    font-size:80%;  \
}                      \
.Em {                   \
    color: %4;        \
}                      \
.Ee {                   \
    color: %3;        \
}                      \
.Ed {                   \
    color: %2;        \
}                      \
.Ea {                   \
    color: %1;        \
}                      \
.Aa {                   \
    color: %1;        \
}                      \
.Ae {                   \
    color: %3;        \
}                      \
.Ad {                   \
    color: %2;        \
}                      \
.Am {                   \
    color: %4;        \
}                      \
.Ca {                   \
    color: %1;        \
    font-style:italic;  \
    font-size:80%;  \
}                      \
.Cm {                   \
    color: %4;        \
    font-style:italic;  \
    font-size:80%;  \
}                      \
.Cd {                   \
    color: %2;        \
    font-style:italic;  \
    font-size:80%;  \
}                      \
.Ce {                   \
    color: %3;        \
    font-style:italic;  \
    font-size:80%;  \
}                      \
.AVe {                   \
    color: #BB92C5;        \
}                      \
</style>"


CompareResultTextFormat::CompareResultTextFormat()
{
    stateE = "e";
    stateA = "a";
    stateM = "m";
    stateD = "d";
}

CompareResultTextFormat::~CompareResultTextFormat()
{
}

QString CompareResultTextFormat::text()
{
    return resultText;
}

QString CompareResultTextFormat::stateToClassCode(const EDiff::KDiff state)
{
    switch(state) {
    case EDiff::ED_EQUAL:
        return stateE;
    case EDiff::ED_ADDED:
        return stateA;
    case EDiff::ED_MODIFIED:
        return stateM;
    case EDiff::ED_DELETED:
        return stateD;
    case EDiff::ED_DIFFERENT:
        return stateM;
    default:
        return "";
    }
}


QString CompareResultTextFormat::formatText(OperationResult &result, DiffNodesChangeList *diffList)
{
    result.setOk();
    resultText = "<html><head>";
    resultText += QString(CSS_TEXT).arg(CLR_ADDED).arg(CLR_DELETED).arg(CLR_EQUAL).arg(CLR_MODIFIED);
    resultText += "</head><body>";
    foreach(DiffSingleNodeResult * childNode, diffList->rootLevel()) {
        scanRecursive(childNode, 0);
    }
    resultText += "</body></html>";
    return resultText;
}

void CompareResultTextFormat::scanRecursive(DiffSingleNodeResult * node, const int indent)
{
    Element *element = dumpElement(node, indent, node->type());
    dumpAttributes(node);
    bool isEmpty = node->children().isEmpty();
    closeTag(element, node->type(), isEmpty);
    if(!node->children().isEmpty()) {
        foreach(DiffSingleNodeResult * child, node->children()) {
            scanRecursive(child, indent + 1);
        }
        endElement(element, indent, node->type());
    }
}


void CompareResultTextFormat::closeTag(Element *element, const EDiff::KDiff state, const bool isEmpty)
{
    QString classCode = stateToClassCode(state);
    switch(element->getType()) {
    default:
        break;
    case Element::ET_ELEMENT:
        if(isEmpty) {
            resultText += QString("<span class='E%1'>/&gt;</span><br/>").arg(classCode);
        } else {
            resultText += QString("<span class='E%1'>&gt;</span><br/>").arg(classCode);
        }
        break;
    }
}

Element *CompareResultTextFormat::dumpElement(DiffSingleNodeResult *node, const int indent, const EDiff::KDiff state)
{
    Element *element = NULL ;
    if(EDiff::ED_DELETED == state) {
        element = node->compareElement()->element();
    } else {
        element = node->referenceElement()->element();
    }

    QString classCode = stateToClassCode(state);
    for(int i = 0 ; i < indent ; i++) {
        resultText += "&nbsp;";
    }
    switch(element->getType()) {
    case Element::ET_COMMENT:
        resultText += QString("<span class='C%1'>&lt;--%2--&gt;</span><br/>\n").arg(classCode).arg(element->getComment());
        break;
    case Element::ET_ELEMENT:
        resultText += QString("<span class='E%1'>&lt;%2</span>\n").arg(classCode).arg(element->tag());
        break;
    case Element::ET_PROCESSING_INSTRUCTION:
        resultText += QString("<span class='C%1'>&lt;?%2 %3?&gt;</span><br/>\n").arg(classCode).arg(element->getPITarget()).arg(element->getPIData());
        break;
    case Element::ET_TEXT:
        dumpText(element->text, state, element->isCDATA(), indent + 1);
        break;
    default:
        break;
    }
    return element;
}

void CompareResultTextFormat::endElement(Element *element, const int indent, const EDiff::KDiff state)
{
    switch(element->getType()) {
    case Element::ET_TEXT:
        break;
    case Element::ET_COMMENT:
        break;
    case Element::ET_ELEMENT:
        for(int i = 0 ; i < indent ; i++) {
            resultText += "&nbsp;";
        }
        resultText += QString("<span class='E%1'>&lt;/%2&gt;</span><br/>").arg(stateToClassCode(state)).arg(element->tag());
        break;
    case Element::ET_PROCESSING_INSTRUCTION:
        break;
    default:
        break;
    }
}

void CompareResultTextFormat::dumpText(const QString text, const EDiff::KDiff state, const bool isCData, const int indent)
{
    for(int i = 0 ; i < indent ; i++) {
        resultText += "&nbsp;";
    }
    if(isCData) {
        resultText += "&lt;![CDATA[";
    }
    resultText += QString("<span class='T%1'>%2</span><br/>\n").arg(stateToClassCode(state)).arg(convertTextInHTML(text));
    if(isCData) {
        resultText += "]]&gt;<br>";
    }
}

QString CompareResultTextFormat::convertTextInHTML(const QString &strIn)
{
    QString res ;
    int len = strIn.length();
    for(int i = 0 ; i < len ; i ++) {
        QChar ch = strIn.at(i);
        if(ch.isDigit() || (('a' <= ch) && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) {
            res.append(ch) ;
        } else {
            ushort valueInt = ch.unicode();
            res.append(QString("&#%1;"). arg(valueInt));
        }
    }
    return res;
}

void CompareResultTextFormat::dumpAttributes(DiffSingleNodeResult *node)
{
    if(node->hasAttributes()) {
        SourceElementDiffOperation* reference = node->referenceElement();
        if(NULL != reference) {
            dumpAttributeList(reference->addedAttributes(), EDiff::ED_ADDED);
            dumpAttributeList(reference->modifiedAttributes(), EDiff::ED_MODIFIED);
        }
        dumpAttributeList(node->equalsAttributes(), EDiff::ED_EQUAL);
        SourceElementDiffOperation* compare = node->compareElement();
        if(NULL != compare) {
            dumpAttributeList(compare->addedAttributes(), EDiff::ED_DELETED);
        }
    }
}

void CompareResultTextFormat::dumpAttributeList(QList<Attribute*> &attributes, const EDiff::KDiff state)
{
    foreach(Attribute * attribute, attributes) {
        dumpAttribute(attribute, state);
    }
}

void CompareResultTextFormat::dumpAttribute(Attribute *attribute, const EDiff::KDiff state)
{
    if(state == EDiff::ED_EQUAL) {
        resultText += QString(" <span class='Ae'>%1=&quot;<span class='AVe'>%2</span>&quot;</span>").arg(attribute->name).arg(convertTextInHTML(attribute->value));
    } else {
        resultText += QString(" <span class='A%1'>%2=&quot;%3&quot;</span>").arg(stateToClassCode(state)).arg(attribute->name).arg(convertTextInHTML(attribute->value));
    }

}
