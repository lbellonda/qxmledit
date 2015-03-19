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


#ifndef COMPAREENGINE_H
#define COMPAREENGINE_H

#include <QObject>
#include "regola.h"
#include "diffresult.h"
#include "operationresult.h"


class CompareOptions
{
    bool _compareText;
    bool _compareComments;
    bool _denormalizeEOL;

public:
    explicit CompareOptions();
    virtual ~CompareOptions();


    bool isCompareText();
    void setCompareText(const bool value);
    bool isCompareComments();
    void setCompareComments(const bool value);
    bool isDenormalizeEOL();
    void setDenormalizeEOL(const bool value);
};

class CompareData
{
public:
    QList<Element*> finalCollection;
    QList<Element*> compareCollection;
    QSet<int> targetRemoved ;
    int referenceCount ;
    int compareCount ;
    int indexCompare  ;

    CompareData(QList<Element*> &referenceList, QList<Element*> &compareList);
    ~CompareData();

    int nextIndexCompare();
};


class CompareEngine : public QObject
{
    Q_OBJECT

    bool _areDifferent;
    void filterElements(QList<Element*> &inputList, QList<Element*> &outputList, CompareOptions &options);
    void fillAttributeList(QList<Attribute*> &destAttributes, Element *element);
    void fillAttributeMap(QHash<QString, Attribute*> &destAttributes, Element *element);
    void executeCompareElements(DiffSingleNodeResult *diff);
    EDiff::KDiff compareNodes(Element* reference, Element* compare, CompareOptions &options);
    void addChildBranch(OperationResult *result, QList<DiffSingleNodeResult *>& parentList, Element* element, const EDiff::KDiff state);

    void compareOrdered(OperationResult *result, DiffNodesChangeList *root, QList<DiffSingleNodeResult *>& parentList, QList<Element*> &referenceInputList, QList<Element*> &compareInputList, CompareOptions &options);
    void advanceChild(OperationResult *result, DiffNodesChangeList *root, QList<DiffSingleNodeResult *>& parentList, Element *referenceElement, Element *compareElement, CompareData &data, const EDiff::KDiff newState, CompareOptions &options);
    void compareDifferentObjects(OperationResult *result, DiffNodesChangeList *root, QList<DiffSingleNodeResult*>& parentList, Element *referenceElement, CompareData &data, CompareOptions &options);
    Regola *loadRegola(const QString &fileName);
    Regola *loadData(QByteArray *dataIn);
    Regola *loadRule(QByteArray *dataIn);

public:
    explicit CompareEngine(QObject *parent = 0);
    virtual ~CompareEngine();

    void doCompare(OperationResult *results, Regola *one, Regola *two, DiffNodesChangeList *changeList, CompareOptions &options);
    bool compareQuick(Regola *one, Regola *two);
    bool compareQuick(Regola *one, const QString &fileName);
    bool compareQuick(Regola *one, QByteArray *dataIn);

    bool areDifferent();

signals:

public slots:

};

#endif // COMPAREENGINE_H
