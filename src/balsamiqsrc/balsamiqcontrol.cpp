/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "balsamiqcontrol.h"
#include <stdio.h>
#include <QDomDocument>
#include "utils.h"


#define TYPE_A "A"
#define TYPE_P "P"
#define TYPE_C "C"

#define TEMPLATE_ONEROW_START   ":/balsamiq/onerowdataprovider_start.txml"
#define TEMPLATE_ONEROW_ROW     ":/balsamiq/onerowdataprovider_row.txml"
#define TEMPLATE_ONEROW_END     ":/balsamiq/onerowdataprovider_end.txml"

#define TEMPLATE_MULTIROWS_START    ":/balsamiq/multirowsdataprovider_start.txml"
#define TEMPLATE_MULTIROWS_ROWSTART ":/balsamiq/multirowsdataprovider_rowstart.txml"
#define TEMPLATE_MULTIROWS_ROWEND   ":/balsamiq/multirowsdataprovider_rowend.txml"
#define TEMPLATE_MULTIROWS_ROWCOL   ":/balsamiq/multirowsdataprovider_rowcol.txml"
#define TEMPLATE_MULTIROWS_END      ":/balsamiq/multirowsdataprovider_end.txml"
#define TEMPLATE_GRID_COLUMNS_REP   ":/balsamiq/datagridcolrep.txml"
#define TEMPLATE_GRID_START         ":/balsamiq/datagridstart.txml"
#define TEMPLATE_GRID_END           ":/balsamiq/datagridend.txml"

#define TEMPLATE_TREE_START         ":/balsamiq/treestart.txml"
#define TEMPLATE_TREE_ROW           ":/balsamiq/treerow.txml"
#define TEMPLATE_TREE_ROWEND        ":/balsamiq/treerowend.txml"
#define TEMPLATE_TREE_END           ":/balsamiq/treeend.txml"


#define BMML_DATAPROVIDERNAME       "DP_NAME"
#define BMML_DATASOURCE_ROWS        "DP_ROWS"
#define BMML_DATASOURCE_COLUMNS     "DP_COLUMNS"
#define BMML_ROW_COL_N              "DP_ROW_COL_%1_%2"
#define BMML_COL_N                  "DP_COL_%1"
#define BMML_DATASOURCE_CURRENT_ROWDATA    "DP_CURRENT_ROWDATA"
#define BMML_DATASOURCE_CURRENT_COL        "DP_CURRENT_COL"
#define BMML_DATASOURCE_CURRENT_COLDATA    "DP_CURRENT_COLDATA"
#define BMML_ROW_TITLE_N                    "DP_TITLE_N_%1"
#define BMML_DATASOURCE_CURRENT_COLTITLE    "DP_CURRENT_COLTITLE"


//--------------------------------------------------------------------------------------------------------------


BalsamiqOpContext::BalsamiqOpContext() : globalNode(fakeGlobalNode)
{
    isError = false;
}

BalsamiqOpContext::~BalsamiqOpContext()
{
}

void BalsamiqOpContext::setError(const QString &error)
{
    errorCause = error ;
    isError = true ;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqControl)

/** Subsitution syntax:
  {A:N:M} A; attrib/property[A|P|C]  N tipo: string M:nome
*/
bool BalsamiqControl::executeCmd(const QStringList &cmds, QString &inputData, BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields)
{
    if(cmds.size() < 3) {
        return false ;
    }
    QString type = cmds.at(0).trimmed();
    QString name = cmds.at(2).trimmed();
    QString format = cmds.at(1).trimmed();
    QString data;
    if(type == TYPE_A) {
        data = proxy->attribute(name);
    } else if(type == TYPE_P) {
        data = proxy->property(name);
    } else if(type == TYPE_C) {
        if(calculatedFields.contains(name)) {
            data = calculatedFields.value(name);
        } else {
            data = "" ;
        }
    } else {
        Utils::error(tr("unknown type in substitution: %1").arg(type));
        return false;
    }
    inputData.append(data);
    return true;
}

QString BalsamiqControl::doSubstitutions(BalsamiqOpContext &context, const QString &inputData, BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields)
{
    QString result ;
    int len = inputData.length();
    int actual = 0;
    bool moreData = false;
    do {
        int pos = inputData.indexOf('{', actual);
        int posLast = -1 ;
        if(pos >= 0) {
            // test for escape
            if(pos < (len - 1)) {
                if(inputData.at(pos + 1) != '{') {
                    posLast = inputData.indexOf('}', pos + 1);
                } else {
                    result += inputData.mid(actual, pos - actual + 1);
                    actual = pos + 2 ;
                    moreData = true;
                    continue;
                }
            }
        }
        if((pos >= 0) && (posLast >= 0)) {
            // do it, copy last run
            if(pos > actual) {
                result += inputData.mid(actual, pos - actual);
            }
            QString translate = inputData.mid(pos + 1, posLast - pos - 1); // remember: strip first and last {}
            QStringList cmd = translate.split(':');
            if(!executeCmd(cmd, result, proxy, calculatedFields)) {
                context.setError(tr("Malformed substitution pattern: %1").arg(translate));
            }
            actual = posLast + 1;
            moreData = true ;
        } else {
            // copy the string as is
            result += inputData.mid(actual);
            moreData = false;
        }
    } while(moreData);
    D(printf("SUBSTITUTION: %s||%s\n\n", inputData.toAscii().data(), result.toAscii().data()));
    return result;
}


bool BalsamiqControl::loadTemplate(const QString &templateFilePath)
{
    bool isError = false;
    if(templateData.isEmpty()) {
        templateData = Utils::readUTF8FileString(templateFilePath, isError);
    }
    return !isError ;
}

bool BalsamiqControl::loadDataTemplate()
{
    bool isError = false;
    if(oneRowDataProviderTemplateStart.isEmpty()) {
        oneRowDataProviderTemplateStart = Utils::readUTF8FileString(TEMPLATE_ONEROW_START, isError);
        oneRowDataProviderTemplateRow = Utils::readUTF8FileString(TEMPLATE_ONEROW_ROW, isError);
        oneRowDataProviderTemplateEnd = Utils::readUTF8FileString(TEMPLATE_ONEROW_END, isError);
    }
    return !isError ;
}

bool BalsamiqControl::loadMultiDataTemplate()
{
    bool isError = false;
    if(multiRowsDataProviderTemplateStart.isEmpty()) {
        multiRowsDataProviderTemplateStart = Utils::readUTF8FileString(TEMPLATE_MULTIROWS_START, isError);
        multiRowsDataProviderTemplateDataRowStart = Utils::readUTF8FileString(TEMPLATE_MULTIROWS_ROWSTART, isError);
        multiRowsDataProviderTemplateDataRowEnd = Utils::readUTF8FileString(TEMPLATE_MULTIROWS_ROWEND, isError);
        multiRowsDataProviderTemplateRowCol = Utils::readUTF8FileString(TEMPLATE_MULTIROWS_ROWCOL, isError);
        multiRowsDataProviderTemplateEnd = Utils::readUTF8FileString(TEMPLATE_MULTIROWS_END, isError);
    }
    return !isError ;
}



bool BalsamiqControl::generateNodes(BalsamiqProxy *proxy, QDomNode &node, const QString &text)
{
    QDomDocument outputDoc;
    QDomDocument document = node.ownerDocument();
    if(!outputDoc.setContent(text)) {
        return false;
    }
    bool isDocument = node.isDocument();
    int nodi = outputDoc.childNodes().count();
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = outputDoc.childNodes().item(i) ;
        QDomNode importedNode = document.importNode(childNode, true);
        if(isDocument) {
            proxy->node = document.appendChild(importedNode);
        } else {
            proxy->node = node.appendChild(importedNode);
        }
    }
    return true ;
}

bool BalsamiqControl::generateFromResource(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode &node, const bool isEnd, const QString &fileResource)
{
    if(!isEnd) {
        if(!loadTemplate(fileResource)) {
            context.setError(tr("No template"));
            return false;
        }

        QString final = doSubstitutions(context, templateData, proxy, proxy->calculatedFields());
        if(context.isError) {
            return false ;
        }
        if(!generateNodes(proxy, node, final)) {
            context.setError(tr("Nodes generation"));
            return false;
        }

        D(printf("type: GENERATING %s \n", proxy->type().toAscii().data()));
        proxy->dump();
    }
    return true;
}

QString BalsamiqControl::getDataRowColKey(const int rowIndex, const int colIndex)
{
    return QString(BMML_ROW_COL_N).arg(rowIndex).arg(colIndex);
}

QString BalsamiqControl::getDataColKey(const int colIndex)
{
    return QString(BMML_COL_N).arg(colIndex);
}

QString BalsamiqControl::getTitleKey(const int index)
{
    return QString(BMML_ROW_TITLE_N).arg(index) ;
}

bool BalsamiqControl::calculateOneRowDataProvider(BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields, const QChar separator)
{
    QString text = proxy->property(BMML_PROPERTY_TEXT);
    QStringList providerData = text.split(separator);
    calculatedFields.insert(BMML_DATAPROVIDERNAME, QString("dataProvider_%1").arg(proxy->id()));
    calculatedFields.insert(BMML_DATASOURCE_ROWS, "1");
    calculatedFields.insert(BMML_DATASOURCE_COLUMNS, QString::number(providerData.count()));
    int index = 0;
    foreach(QString oneDatum, providerData) {
        QString colKey = getDataRowColKey(0, index);
        calculatedFields.insert(colKey, oneDatum.trimmed());
        index ++;
    }

    return true;
}

bool BalsamiqControl::calculateMultiRowDataProvider(BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields)
{
    QString text = proxy->property(BMML_PROPERTY_TEXT);
    QStringList providerData = text.split('\n');
    // first row is for titles
    int rows = providerData.size();
    bool hasHeader = true ; //TODO proxy->hasHeader();
    if(rows < 0) {
        rows = 0 ;
    }
    QStringList titles;
    if(hasHeader && (providerData.size() > 0)) {
        QStringList dataTitles = providerData.at(0).split(',');
        foreach(QString title, dataTitles) {
            titles.append(title.trimmed());
        }
    }
    int maxCols = titles.size();
    for(int row = hasHeader ? 1 : 0 ; row < rows ; row++) {
        QString dataRow = providerData.at(row);
        QStringList stringDataRow = dataRow.split(',');
        int col = 0;
        foreach(QString oneDatum, stringDataRow) {
            QString colKey = getDataRowColKey(row, col);
            calculatedFields.insert(colKey, oneDatum.trimmed());
            col ++;
            if(col > maxCols) {
                maxCols = col ;
                titles.append(colKey);
            }
        }
    }
    // insert titles

    calculatedFields.insert(BMML_DATAPROVIDERNAME, QString("dataProvider_%1").arg(proxy->id()));
    calculatedFields.insert(BMML_DATASOURCE_ROWS, QString::number(rows));
    calculatedFields.insert(BMML_DATASOURCE_COLUMNS, QString::number(maxCols));
    int titleIndex = 0;
    foreach(QString singleTitle, titles) {
        QString titleKey = getTitleKey(titleIndex);
        calculatedFields.insert(titleKey, singleTitle.trimmed());
        titleIndex++;
    }
    return true;
}



bool BalsamiqControl::calculateData(BalsamiqOpContext & /*context*/, BalsamiqProxy * /*proxy*/, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool /*isEnd*/)
{
    return true;
}

bool BalsamiqControl::generateGlobalsOneRowDataProvider(BalsamiqOpContext &context, BalsamiqProxy *proxy)
{
    if(!loadDataTemplate()) {
        context.setError(tr("Loading data templates"));
        return false;
    }
    QMap<QString, QString> &calculatedFields = proxy->calculatedFields() ;
    QString final;
    QString start = doSubstitutions(context, oneRowDataProviderTemplateStart, proxy, calculatedFields);
    final = start ;
    int columns = proxy->field(BMML_DATASOURCE_COLUMNS).toInt();
    for(int column = 0 ; column < columns ; column ++) {
        // devo indirizzare la riga corrente per la sostituzione, basta una indirezione nella mappa?
        QString colKey = getDataRowColKey(0, column);
        calculatedFields.insert(BMML_DATASOURCE_CURRENT_ROWDATA, calculatedFields[colKey]);
        QString oneRow = doSubstitutions(context, oneRowDataProviderTemplateRow, proxy, calculatedFields);
        final += oneRow ;
        final += "\n";
    }
    QString end = doSubstitutions(context, oneRowDataProviderTemplateEnd, proxy, calculatedFields);
    final += end;
    if(context.isError) {
        return false ;
    }
    if(!generateNodes(proxy, context.globalNode, final)) {
        context.setError(tr("One row nodes generation"));
        return false;
    }
    return true;
}


bool BalsamiqControl::generateGlobalsMultiRowsDataProvider(BalsamiqOpContext &context, BalsamiqProxy *proxy)
{
    if(!loadMultiDataTemplate()) {
        context.setError(tr("Loading data templates"));
        return false;
    }
    QMap<QString, QString> &calculatedFields = proxy->calculatedFields() ;
    QString final;
    QString start = doSubstitutions(context, multiRowsDataProviderTemplateStart, proxy, calculatedFields);
    final = start ;
    int columns = proxy->field(BMML_DATASOURCE_COLUMNS).toInt();
    int rows = proxy->field(BMML_DATASOURCE_ROWS).toInt();
    for(int row = 1 ; row < rows ; row ++) {
        QString rowStart = doSubstitutions(context, multiRowsDataProviderTemplateDataRowStart, proxy, calculatedFields);
        final += rowStart ;
        for(int column = 0 ; column < columns ; column ++) {
            QString rowColKey = getDataRowColKey(row, column);
            QString colKey = getDataColKey(column);
            calculatedFields.insert(BMML_DATASOURCE_CURRENT_COLDATA, calculatedFields[rowColKey]);
            calculatedFields.insert(BMML_DATASOURCE_CURRENT_COL, colKey);
            QString oneCol = doSubstitutions(context, multiRowsDataProviderTemplateRowCol, proxy, calculatedFields);
            final += oneCol ;
        }
        QString rowEnd = doSubstitutions(context, multiRowsDataProviderTemplateDataRowEnd, proxy, calculatedFields);
        final += rowEnd ;
        final += "\n";
    }
    QString end = doSubstitutions(context, multiRowsDataProviderTemplateEnd, proxy, calculatedFields);
    final += end;
    if(context.isError) {
        return false ;
    }
    if(!generateNodes(proxy, context.globalNode, final)) {
        context.setError(tr("One row nodes generation"));
        return false;
    }
    return true;
}

bool BalsamiqControl::generateGlobals(BalsamiqOpContext & /*context*/, BalsamiqProxy * /*proxy*/, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool /*isEnd*/)
{
    return true;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqApplication)

QString BalsamiqApplication::controlName()
{
    return BMML_APPLICATION_TAG ;
}

bool BalsamiqApplication::generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    bool isOk = generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/application.txml");
    context.globalNode = proxy->node ;
    return isOk ;
}

bool BalsamiqApplication::generateControls(BalsamiqOpContext & /*context*/, BalsamiqProxy * /*proxy*/, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool /*isEnd*/)
{
    return true;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqGeneric)

QString BalsamiqGeneric::controlName()
{
    return BALSAMIQ_CONTROL_DEFAULT ;
}

bool BalsamiqGeneric::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    bool result = generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/generic.txml");
    D(printf("GENERIC: GENERATING %s\n", proxy->type().toAscii().data())) ;
    return result;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqButton)

QString BalsamiqButton::controlName()
{
    return BMML_BUTTON ;
}

bool BalsamiqButton::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/button.txml");
}


//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqLabel)

QString BalsamiqLabel::controlName()
{
    return BMML_LABEL ;
}

bool BalsamiqLabel::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/label.txml");
}


//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqLinkBar)

QString BalsamiqLinkBar::controlName()
{
    return BMML_LINKBAR ;
}

bool BalsamiqLinkBar::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/linkbar.txml");
}

bool BalsamiqLinkBar::generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!generateGlobalsOneRowDataProvider(context, proxy)) {
            return false;
        }
    }
    return true ;
}

bool BalsamiqLinkBar::calculateData(BalsamiqOpContext & /*context*/, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!calculateOneRowDataProvider(proxy, proxy->calculatedFields(), ',')) {
            return false;
        }
    }
    return true ;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqDataGrid)

QString BalsamiqDataGrid::controlName()
{
    return BMML_DATAGRID ;
}


bool BalsamiqDataGrid::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode &node, const bool isEnd)
{
    if(isEnd) {
        return true ;
    }
    if(!loadDataGridTemplate()) {
        context.setError(tr("Loading data templates"));
        return false;
    }
    QMap<QString, QString> &calculatedFields = proxy->calculatedFields() ;
    QString final;
    QString start = doSubstitutions(context, dataGridTemplateStart, proxy, calculatedFields);
    final = start ;
    int columns = proxy->field(BMML_DATASOURCE_COLUMNS).toInt();
    for(int column = 0 ; column < columns ; column ++) {
        QString colKey = getDataColKey(column);
        calculatedFields.insert(BMML_DATASOURCE_CURRENT_COL, colKey);
        QString titleKey = getTitleKey(column);
        calculatedFields.insert(BMML_DATASOURCE_CURRENT_COLTITLE, calculatedFields[titleKey]);
        QString oneCol = doSubstitutions(context, columnTemplateRep, proxy, calculatedFields);
        final += oneCol ;
        final += "\n";
    }
    QString end = doSubstitutions(context, dataGridTemplateEnd, proxy, calculatedFields);
    final += end;
    if(context.isError) {
        return false ;
    }
    if(!generateNodes(proxy, node, final)) {
        context.setError(tr("Data grid nodes generation"));
        return false;
    }
    return true;
}

bool BalsamiqDataGrid::generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!generateGlobalsMultiRowsDataProvider(context, proxy)) {
            return false;
        }
    }
    return true ;
}

bool BalsamiqDataGrid::calculateData(BalsamiqOpContext & /*context*/, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!calculateMultiRowDataProvider(proxy, proxy->calculatedFields())) {
            return false;
        }
    }
    return true ;
}

bool BalsamiqDataGrid::loadDataGridTemplate()
{
    bool isError = false;
    if(columnTemplateRep.isEmpty()) {
        columnTemplateRep = Utils::readUTF8FileString(TEMPLATE_GRID_COLUMNS_REP, isError);
        dataGridTemplateStart = Utils::readUTF8FileString(TEMPLATE_GRID_START, isError);
        dataGridTemplateEnd = Utils::readUTF8FileString(TEMPLATE_GRID_END, isError);
    }
    return !isError ;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqGroup)

QString BalsamiqGroup::controlName()
{
    return BMML_GROUP ;
}

bool BalsamiqGroup::generateControls(BalsamiqOpContext & context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    bool result = generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/group.txml");
    return result ;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqList)

QString BalsamiqList::controlName()
{
    return BMML_LIST ;
}

bool BalsamiqList::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/list.txml");
}

bool BalsamiqList::generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!generateGlobalsOneRowDataProvider(context, proxy)) {
            return false;
        }
    }
    return true ;
}

bool BalsamiqList::calculateData(BalsamiqOpContext & /*context*/, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!calculateOneRowDataProvider(proxy, proxy->calculatedFields(), '\n')) {
            return false;
        }
    }
    return true ;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqTabBar)

QString BalsamiqTabBar::controlName()
{
    return BMML_TABBAR ;
}

bool BalsamiqTabBar::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/tabbar.txml");
}

bool BalsamiqTabBar::generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!generateGlobalsOneRowDataProvider(context, proxy)) {
            return false;
        }
    }
    return true ;
}

bool BalsamiqTabBar::calculateData(BalsamiqOpContext & /*context*/, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!calculateOneRowDataProvider(proxy, proxy->calculatedFields(), ',')) {
            return false;
        }
    }
    return true ;
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqVRule)

QString BalsamiqVRule::controlName()
{
    return BMML_VRULE ;
}

bool BalsamiqVRule::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/vrule.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqHRule)

QString BalsamiqHRule::controlName()
{
    return BMML_HRULE ;
}

bool BalsamiqHRule::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/hrule.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqTextInput)

QString BalsamiqTextInput::controlName()
{
    return BMML_TEXTINPUT ;
}

bool BalsamiqTextInput::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/textinput.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqComboBox)

QString BalsamiqComboBox::controlName()
{
    return BMML_COMBOBOX ;
}

bool BalsamiqComboBox::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/combobox.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqTextArea)

QString BalsamiqTextArea::controlName()
{
    return BMML_TEXTAREA ;
}

bool BalsamiqTextArea::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/textarea.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqParagraph)

QString BalsamiqParagraph::controlName()
{
    return BMML_PARAGRAPH  ;
}

bool BalsamiqParagraph::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/paragraph.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqCheckBox)

QString BalsamiqCheckBox::controlName()
{
    return BMML_CHECKBOX ;
}

bool BalsamiqCheckBox::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/checkbox.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqRadioButton)

QString BalsamiqRadioButton::controlName()
{
    return BMML_RADIOBUTTON ;
}

bool BalsamiqRadioButton::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/radio.txml");
}

//--------------------------------------------------------------------------------------------------------------

IMPL_CONSTRUCTOR(BalsamiqTree)

QString BalsamiqTree::controlName()
{
    return BMML_TREE ;
}

bool BalsamiqTree::generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd)
{
    return generateFromResource(context, proxy, parent, node, isEnd, ":/balsamiq/tree.txml");
}

bool BalsamiqTree::generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode & /*node*/, const bool isEnd)
{
    if(!isEnd) {
        if(!generateTreeDataProvider(context, proxy)) {
            return false;
        }
    }
    return true ;
}

bool BalsamiqTree::generateTreeItemDataProvider(BalsamiqOpContext &context, BalsamiqDataTreeItem *item, BalsamiqProxy *proxy, QString &text)
{
    QString value = item->text() ;
    proxy->setField(BMML_FIELD_ATTRIBUTE_TREEDATA, value);
    text += doSubstitutions(context, treeDataProviderTemplateRow, proxy, proxy->calculatedFields());
    foreach(BalsamiqDataTreeItem * childItem, item->children()) {
        if(!generateTreeItemDataProvider(context, childItem, proxy, text)) {
            return false;
        }
    }
    proxy->setField(BMML_FIELD_ATTRIBUTE_TREEDATA, value);
    text += doSubstitutions(context, treeDataProviderTemplateRowEnd, proxy, proxy->calculatedFields());
    return true;
}

bool BalsamiqTree::generateTreeDataProvider(BalsamiqOpContext &context, BalsamiqProxy *proxy)
{
    if(!loadTemplateTree()) {
        return false;
    }

    QString text = proxy->property(BMML_PROPERTY_TEXT);
    BalsamiqDataTree dataTree;
    if(!dataTree.scanText(text)) {
        return false;
    }

    proxy->calculatedFields().insert(BMML_DATAPROVIDERNAME, QString("dataProvider_%1").arg(proxy->id()));

    QString final ;
    QString start = doSubstitutions(context, treeDataProviderTemplateStart, proxy, proxy->calculatedFields());
    final = start ;

    foreach(BalsamiqDataTreeItem * item, dataTree.children()) {
        if(! generateTreeItemDataProvider(context, item, proxy, final)) {
            return false;
        }
    }
    QString end = doSubstitutions(context, treeDataProviderTemplateEnd, proxy, proxy->calculatedFields());
    final += end;
    if(context.isError) {
        return false ;
    }
    if(!generateNodes(proxy, context.globalNode, final)) {
        context.setError(tr("Tree row nodes generation"));
        return false;
    }
    return true;
}

bool BalsamiqTree::loadTemplateTree()
{
    bool isError = false;
    if(treeDataProviderTemplateStart.isEmpty()) {
        treeDataProviderTemplateStart = Utils::readUTF8FileString(TEMPLATE_TREE_START, isError);
        treeDataProviderTemplateRow = Utils::readUTF8FileString(TEMPLATE_TREE_ROW, isError);
        treeDataProviderTemplateRowEnd = Utils::readUTF8FileString(TEMPLATE_TREE_ROWEND, isError);
        treeDataProviderTemplateEnd = Utils::readUTF8FileString(TEMPLATE_TREE_END, isError);
    }
    return !isError ;
}

//--------------------------------------------------------------------------------------------------------------



