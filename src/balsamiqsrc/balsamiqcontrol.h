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



#ifndef BALSAMIQCONTROL_H
#define BALSAMIQCONTROL_H

#include <QDomDocument>
#include <QApplication>
#include <QMap>
#include "balsamiqproxy.h"
#include "balsamiqtree.h"
#include "xmlEdit.h"

#define BALSAMIQ_CONTROL_DEFAULT    "*"


class BalsamiqOpContext
{
    QDomNode fakeGlobalNode;
public:
    QString errorCause;
    bool isError ;
    QString fileName;
    QDomNode &globalNode;

    BalsamiqOpContext();
    ~BalsamiqOpContext();

    void setError(const QString &error) ;
};

class BalsamiqControl
{
    Q_DECLARE_TR_FUNCTIONS(BalsamiqControl)

protected:
    QString templateData;
    QString oneRowDataProviderTemplateStart;
    QString oneRowDataProviderTemplateRow;
    QString oneRowDataProviderTemplateEnd;

    QString multiRowsDataProviderTemplateStart ;
    QString multiRowsDataProviderTemplateDataRowStart ;
    QString multiRowsDataProviderTemplateDataRowEnd ;
    QString multiRowsDataProviderTemplateRowCol ;
    QString multiRowsDataProviderTemplateEnd ;

    bool generateNodes(BalsamiqProxy *proxy, QDomNode &node, const QString &text);
    bool generateFromResource(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd, const QString &fileResource);
    QString doSubstitutions(BalsamiqOpContext &context, const QString &inputData, BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields);
    bool executeCmd(const QStringList &cmds, QString &inputData, BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields);
    bool calculateOneRowDataProvider(BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields, const QChar separator);
    bool calculateMultiRowDataProvider(BalsamiqProxy *proxy, QMap<QString, QString> &calculatedFields);
    bool generateGlobalsMultiRowsDataProvider(BalsamiqOpContext &context, BalsamiqProxy *proxy);
    bool generateGlobalsOneRowDataProvider(BalsamiqOpContext &context, BalsamiqProxy *proxy);
    bool loadTemplate(const QString &templateName);
    bool loadDataTemplate();
    bool loadMultiDataTemplate();
    QString getDataRowColKey(const int rowIndex, const int colIndex);
    QString getDataColKey(const int colIndex);
    QString getTitleKey(const int index);

public:
    DECL_CONSTRUCTOR(BalsamiqControl)

    virtual QString controlName() = 0;
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) = 0;
    virtual bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};


class BalsamiqApplication : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqApplication)

    virtual QString controlName();
    virtual bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};


class BalsamiqGeneric : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqGeneric)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqButton : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqButton)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqLabel : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqLabel)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqLinkBar : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqLinkBar)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd);
};

class BalsamiqDataGrid : public BalsamiqControl
{
    QString dataGridTemplateStart ;
    QString dataGridTemplateEnd ;
    QString columnTemplateRep ;
    bool loadDataGridTemplate();
public:
    DECL_CONSTRUCTOR(BalsamiqDataGrid)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd);
};

class BalsamiqGroup : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqGroup)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqList : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqList)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd);
};

class BalsamiqTabBar : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqTabBar)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
    virtual bool calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd);
};

class BalsamiqVRule : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqVRule)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqHRule : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqHRule)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqTextInput : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqTextInput)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqComboBox : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqComboBox)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqTextArea : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqTextArea)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqParagraph : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqParagraph)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqCheckBox : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqCheckBox)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqRadioButton : public BalsamiqControl
{
public:
    DECL_CONSTRUCTOR(BalsamiqRadioButton)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;
};

class BalsamiqTree : public BalsamiqControl
{
    QString treeDataProviderTemplateStart;
    QString treeDataProviderTemplateRow;
    QString treeDataProviderTemplateRowEnd;
    QString treeDataProviderTemplateEnd;

    bool loadTemplateTree();
    bool generateTreeDataProvider(BalsamiqOpContext &context, BalsamiqProxy *proxy);
    bool generateTreeItemDataProvider(BalsamiqOpContext &context, BalsamiqDataTreeItem *item, BalsamiqProxy *proxy, QString &text);

public:
    DECL_CONSTRUCTOR(BalsamiqTree)

    virtual QString controlName();
    virtual bool generateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node, const bool isEnd) ;

    bool generateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy * /*parent*/, QDomNode &node, const bool isEnd);

};

#endif // BALSAMIQCONTROL_H
