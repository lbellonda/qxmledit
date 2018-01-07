/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "compareengine.h"
#include "utils.h"

//----------------------------------------------------------------------------------------

CompareOptions::CompareOptions()
{
    _compareText = true ;
    _compareComments = true ;
    _denormalizeEOL = false;
}

CompareOptions::~CompareOptions()
{
}

bool CompareOptions::isCompareText()
{
    return _compareText ;
}


void CompareOptions::setCompareText(const bool value)
{
    _compareText = value ;
}

bool CompareOptions::isCompareComments()
{
    return _compareComments ;
}


void CompareOptions::setCompareComments(const bool value)
{
    _compareComments = value ;
}

bool CompareOptions::isDenormalizeEOL()
{
    return _denormalizeEOL ;
}


void CompareOptions::setDenormalizeEOL(const bool value)
{
    _denormalizeEOL = value ;
}

//----------------------------------------------------------------------------------------

CompareData::CompareData(QList<Element*> &referenceList, QList<Element*> &compareList)
{
    referenceCount = referenceList.count();
    compareCount = compareList.count();
    foreach(Element * child, compareList) {
        compareCollection.append(child);
    }
    indexCompare = 0 ;
}

CompareData::~CompareData()
{
}

int CompareData::nextIndexCompare()
{
    indexCompare++;
    return  indexCompare;
}


//----------------------------------------------------------------------------------------


CompareEngine::CompareEngine(QObject *parent) :
    QObject(parent)
{
    _areDifferent = false;
}

CompareEngine::~CompareEngine()
{
}

bool CompareEngine::areDifferent()
{
    return _areDifferent ;
}

Regola *CompareEngine::loadData(QByteArray *dataIn)
{
    QBuffer file(dataIn);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        if(document.setContent(&file)) {
            return new Regola(document, "", true);
        } else {
            Utils::error(tr("Unable to parse XML"));
        }
    } else {
        Utils::error(QString(tr("Unable to load file.")));
    }
    return NULL ;
}

Regola *CompareEngine::loadRegola(const QString &fileName)
{
    Regola *regola = NULL ;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QDomDocument document;
        QString errorMsg ;
        int errorLine = 0, errorColumn = 0;
        if(document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
            regola = new Regola(document, fileName, true);
        }
        file.close();
    }
    return regola ;
}

bool CompareEngine::compareQuick(Regola *one, Regola *two)
{
    OperationResult results;
    _areDifferent = false;
    DiffNodesChangeList changeList;
    CompareOptions options;
    results.setMessage(tr("Engine started"));
    QList<Element*> listOne = one->getItems().toList();
    QList<Element*> listTwo = two->getItems().toList();
    compareOrdered(&results, &changeList, changeList.rootLevel(), listOne, listTwo, options);
    _areDifferent = !changeList.isReferenceEqualToCompare();
    return results.isOk();
}

bool CompareEngine::compareQuick(Regola *one, const QString &fileName)
{
    Regola *two = loadRegola(fileName);
    if(NULL == two) {
        return false;
    }
    bool result = compareQuick(one, two);
    delete two ;
    return result;
}


bool CompareEngine::compareQuick(Regola *one, QByteArray *dataIn)
{
    Regola *two = loadData(dataIn);
    if(NULL == two) {
        return false;
    }
    bool result = compareQuick(one, two);
    delete two ;
    return result;
}

void CompareEngine::doCompare(OperationResult *results, Regola *one, Regola *two, DiffNodesChangeList *changeList, CompareOptions &options)
{
    _areDifferent = false;
    results->setMessage(tr("Engine started"));
    QList<Element*> listOne = one->getItems().toList();
    QList<Element*> listTwo = two->getItems().toList();
    compareOrdered(results, changeList, changeList->rootLevel(), listOne, listTwo, options);
    _areDifferent = !changeList->isReferenceEqualToCompare();
}

void CompareEngine::filterElements(QList<Element*> &inputList, QList<Element*> &outputList, CompareOptions &options)
{
    bool isTextAllowed = options.isCompareText();
    bool isCommentAllowed = options.isCompareComments();
    foreach(Element * element, inputList) {
        bool canAdd = false ;
        Element::ElType type = element->getType();
        switch(type) {
        case Element::ET_TEXT:
            canAdd = isTextAllowed;
            break;
        case Element::ET_COMMENT:
            canAdd = isCommentAllowed ;
            break;
        default:
            canAdd = true ;
            break;
        }
        if(canAdd) {
            outputList.append(element);
        }
    }
}

void CompareEngine::fillAttributeList(QList<Attribute*> &destAttributes, Element *element)
{
    if(NULL != element) {
        foreach(Attribute * sourceAttribute, element->attributes) {
            destAttributes.append(sourceAttribute);
        }
    }
}

void CompareEngine::fillAttributeMap(QHash<QString, Attribute*> &destAttributes, Element *element)
{
    if(NULL != element) {
        foreach(Attribute * sourceAttribute, element->attributes) {
            destAttributes.insert(sourceAttribute->name, sourceAttribute);
        }
    }
}


void CompareEngine::executeCompareElements(DiffSingleNodeResult *diff)
{
    SourceElementDiffOperation *sourceReference = diff->referenceElement();
    SourceElementDiffOperation *sourceCompare = diff->compareElement();

    Element* reference = NULL ;
    Element* compare = NULL ;
    if(NULL != sourceReference) {
        reference = sourceReference->element();
    }
    if(NULL != sourceCompare) {
        compare = sourceCompare->element();
    }

    QList<Attribute*> sourceAttributes;
    QHash<QString, Attribute*> compareAttributes;

    fillAttributeList(sourceAttributes, reference);
    fillAttributeMap(compareAttributes, compare);

    foreach(Attribute * sourceAttribute, sourceAttributes) {
        Attribute *targetAttr = compareAttributes.value(sourceAttribute->name);
        if(NULL == targetAttr) {
            sourceReference->addAttribute(sourceAttribute);
        } else {
            if(sourceAttribute->value == targetAttr->value) {
                diff->addEqualAttribute(sourceAttribute);
            } else {
                sourceReference->addModifiedAttribute(sourceAttribute);
                sourceCompare->addModifiedAttribute(targetAttr);
            }
            compareAttributes.remove(targetAttr->name);
        }
    }
    // add all the remaining attributes
    foreach(Attribute * sourceAttribute, compareAttributes.values()) {
        sourceCompare->addAttribute(sourceAttribute);
    }
    diff->evaluateAttributes();
}


EDiff::KDiff CompareEngine::compareNodes(Element* reference, Element* compare, CompareOptions &options)
{
    if(reference->getType() != compare->getType()) {
        return EDiff::ED_DIFFERENT ;
    }

    switch(reference->getType()) {

    case Element::ET_COMMENT:
        if(reference->getComment() != compare->getComment()) {
            return EDiff::ED_MODIFIED;
        }
        return EDiff::ED_EQUAL;
        break;

    case Element::ET_ELEMENT:
        if(reference->tag() != compare->tag()) {
            return EDiff::ED_DIFFERENT;
        }
        return EDiff::ED_EQUAL;
        break;

    case Element::ET_PROCESSING_INSTRUCTION:
        if(reference->getPITarget() != compare->getPITarget()) {
            return EDiff::ED_DIFFERENT;
        }
        if(reference->getPIData() != compare->getPIData()) {
            return EDiff::ED_MODIFIED;
        }
        return EDiff::ED_EQUAL;
        break;

    case Element::ET_TEXT:
        if(reference->isCDATA() != compare->isCDATA()) {
            return EDiff::ED_MODIFIED;
        }
        if(reference->isCDATA()) {
            if(options.isDenormalizeEOL()) {
                if(reference->text != compare->text) {
                    return EDiff::ED_MODIFIED;
                } else {
                    // Fix a problem with the parser, crlr must be stripped before.
                    QString dataReference = reference->text.replace("\r\n", "\n");
                    QString dataCompare = compare->text.replace("\r\n", "\n");
                    if(dataReference != dataCompare) {
                        return EDiff::ED_MODIFIED;
                    }
                }
            }
        } else {
            if(reference->text.trimmed() != compare->text.trimmed()) {
                return EDiff::ED_MODIFIED;
            }
        }
        return EDiff::ED_EQUAL;
        break;
    default:
        break;

    }
    return EDiff::ED_DIFFERENT;
}


void CompareEngine::addChildBranch(OperationResult *result, QList<DiffSingleNodeResult *>& parentList, Element* element, const EDiff::KDiff state)
{
    DiffSingleNodeResult *localParent = NULL ;
    SourceElementDiffOperation* source = NULL ;

    if(state == EDiff::ED_ADDED) {
        source = new SourceElementDiffOperation(element);
        localParent  = new AddedDiffNodeResult(source);
    } else if(state == EDiff::ED_DELETED) {
        source = new SourceElementDiffOperation(element);
        localParent  = new DeletedDiffNodeResult(source);
    } else {
        result->setErrorWithText(tr("Inconsistent state (0001)"));
        return ;
    }
    if((NULL == localParent) || (NULL == source)) {
        result->setErrorWithText(tr("Unable to allocate objects (0002)"));
        if(NULL != localParent) {
            delete localParent;
        }
    } else {

        parentList.append(localParent);
        // add all the attributes
        foreach(Attribute * sourceAttribute, element->attributes) {
            source->addAttribute(sourceAttribute);
        }
        foreach(Element * child, *element->getChildItems()) {
            addChildBranch(result, localParent->children(), child, state);
        }
        localParent->evaluateAttributes();
    }
}

void CompareEngine::advanceChild(OperationResult *result, DiffNodesChangeList *root, QList<DiffSingleNodeResult *>& parentList, Element *referenceElement, Element *compareElement, CompareData &data, const EDiff::KDiff newState, CompareOptions &options)
{
    DiffSingleNodeResult *nodeDiff = NULL ;
    SourceElementDiffOperation* sourceReference = new SourceElementDiffOperation(referenceElement);
    SourceElementDiffOperation* sourceCompare = new SourceElementDiffOperation(compareElement);

    if((NULL == sourceReference) || (NULL == sourceCompare)) {
        result->setErrorWithText(tr("Unable to allocate (003)"));
    }

    if(EDiff::ED_EQUAL == newState) {
        nodeDiff = new EqualsDiffNodeResult(sourceReference, sourceCompare);
    } else if(EDiff::ED_MODIFIED == newState) {
        nodeDiff = new ModifiedDiffNodeResult(sourceReference, sourceCompare);
    } else {
        result->setErrorWithText(tr("Inconsistent state (0004)"));
        return ;
    }

    if((NULL == nodeDiff) || (NULL == sourceReference) || (NULL == sourceCompare)) {
        result->setErrorWithText(tr("Unable to allocate objects (0005)"));
        if(NULL != nodeDiff) {
            delete nodeDiff;
        }
        return ;
    }
    if(referenceElement->getType() == Element::ET_ELEMENT) {
        executeCompareElements(nodeDiff);
    }

    parentList.append(nodeDiff);

    QList<Element*> referenceChildrenList;
    QList<Element*> compareChildrenList;

    referenceElement->addElementChildrenInList(referenceChildrenList);
    compareElement->addElementChildrenInList(compareChildrenList);

    compareOrdered(result, root, nodeDiff->children(), referenceChildrenList, compareChildrenList, options);

    data.nextIndexCompare();
}



void CompareEngine::compareOrdered(OperationResult *result, DiffNodesChangeList *root, QList<DiffSingleNodeResult *>& parentList, QList<Element*> &referenceInputList, QList<Element*> &compareInputList, CompareOptions &options)
{
    QList<Element*> referenceList;
    QList<Element*> compareList;

    filterElements(referenceInputList, referenceList, options);
    filterElements(compareInputList, compareList, options);

    CompareData data(referenceList, compareList);

    int indexRef = 0 ;
    for(indexRef = 0 ; indexRef < data.referenceCount ; indexRef++) {
        Element *referenceChild = referenceList.at(indexRef);

        if(data.indexCompare >= data.compareCount) {
            // no target exists, add all as new items
            addChildBranch(result, parentList, referenceChild, EDiff::ED_ADDED);
            root->setDifferent();
        } else {
            Element *compareChild = compareList.at(data.indexCompare);
            EDiff::KDiff compareResult = compareNodes(referenceChild, compareChild, options);
            switch(compareResult) {
            default:
                result->setErrorWithText(tr("Illegal state comparing objects."));
                break;
            case EDiff::ED_EQUAL:
                advanceChild(result, root, parentList, referenceChild, compareChild, data, EDiff::ED_EQUAL, options);
                break;

            case EDiff::ED_MODIFIED:
                advanceChild(result, root, parentList, referenceChild, compareChild, data, EDiff::ED_MODIFIED, options);
                root->setDifferent();
                break;

            // if the object is not found, examine the list until it is found.
            // if a match is found, all the elements between the current+1 and the found, are marked deleted, the index of target is moved to the last pos
            // if no match is found, the scan follows with the next element and the current is new.
            // all the elements that are remaining in the target list, are considedered deleted.
            // the cloned elements are inserted at the end of the scan.
            case EDiff::ED_DIFFERENT:
                compareDifferentObjects(result, root, parentList, referenceChild, data, options);
                root->setDifferent();
                break;
            } // switch
            data.finalCollection.append(referenceChild);
        } // if indexTarget

        // break on error
        if(result->isError()) {
            return ;
        }

    } // for()

    // take all the non examined target elements and add them as deleted to the source collection
    for(int indexRefOuter = data.indexCompare ; indexRefOuter < data.compareCount ; indexRefOuter++) {
        addChildBranch(result, parentList, data.compareCollection.at(indexRefOuter), EDiff::ED_DELETED);
        root->setDifferent();
    }
} // compareOrdered()



void CompareEngine::compareDifferentObjects(OperationResult *result, DiffNodesChangeList *root, QList<DiffSingleNodeResult*>& parentList, Element *referenceElement, CompareData &data, CompareOptions &options)
{
    for(int indexRefOuter = data.indexCompare + 1 ; indexRefOuter < data.compareCount ; indexRefOuter ++) {
        Element *compareTest = data.compareCollection.at(indexRefOuter);
        EDiff::KDiff compareResult = compareNodes(referenceElement, compareTest, options);

        if((EDiff::ED_EQUAL == compareResult)
                || (EDiff::ED_MODIFIED == compareResult)) {
            // mark deleted the elements, and set it to the current element
            for(int indexToDel = data.indexCompare ; indexToDel < indexRefOuter ; indexToDel ++) {
                Element *targetToRemove = data.compareCollection.at(indexToDel);
                // this operation does not update the target counter
                addChildBranch(result, parentList, targetToRemove, EDiff::ED_DELETED);
            } // for
            // Move the index to the next available.
            data.indexCompare = indexRefOuter ; // + 1 see note below
            //NOTE: advanceChild increments the index by one
            // set the value
            if(EDiff::ED_EQUAL == compareResult) {
                advanceChild(result, root, parentList, referenceElement, compareTest, data, EDiff::ED_EQUAL, options);
            } else {
                advanceChild(result, root, parentList, referenceElement, compareTest, data, EDiff::ED_MODIFIED, options);
                root->setDifferent();
            }
            Q_ASSERT(data.indexCompare == (indexRefOuter + 1));
            return ;
        } // if same object found
    } // for
    // no match, the source is added
    addChildBranch(result, parentList, referenceElement, EDiff::ED_ADDED);
    // No change to the target index.
}

