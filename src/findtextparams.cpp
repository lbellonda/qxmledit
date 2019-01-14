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

#include <QtGui>
#include "xmlEdit.h"
#include "findtextparams.h"
#include "undo/elupdateinplacecommand.h"
#include "utils.h"
#include "regola.h"
#include "qxmleditconfig.h"

#define SPLIT_SCOPE_CHAR '/'
#define ATTR_CHAR        '@'

FindTextParams::FindTextParams(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isMatchExact,
                               const bool isCaseSensitive, const bool isOnlyChildren, const EFindTarget findTarget,
                               const bool isSelToBookmarks, const bool isCloseUnrelated, const bool isShowSize,
                               const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection)
{
    init(findType, textToFind, isCountingOnly, isMatchExact,
         isCaseSensitive, isOnlyChildren, findTarget,
         isSelToBookmarks, isCloseUnrelated, isShowSize,
         scope, isWrapAround, useXQuery, selection);

}

void FindTextParams::init(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isMatchExact,
                          const bool isCaseSensitive, const bool isOnlyChildren, const EFindTarget findTarget,
                          const bool isSelToBookmarks, const bool isCloseUnrelated, const bool isShowSize,
                          const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection)
{
    setFindType(findType);
    mSize = 0 ;
    mSelection = selection ;
    mIsCountingOnly = isCountingOnly ;
    mTextToFind = textToFind ;
    mIsMatchExact = isMatchExact ;
    mIsCaseSensitive = isCaseSensitive ;
    mIsOnlyChildren = isOnlyChildren ;
    mFindTarget = findTarget ;
    mIsSelToBookmarks = isSelToBookmarks ;
    mIsCloseUnrelated = isCloseUnrelated ;
    mIsShowSize = isShowSize ;
    mIsSearchAttribute = false;
    mIsScoped = false ;
    mOccurrences = 0 ;
    mScopes = scope.split(SPLIT_SCOPE_CHAR, QString::KeepEmptyParts);
    int scopeCount = mScopes.count();
    if((scopeCount > 0) && mScopes.at(scopeCount - 1).startsWith(ATTR_CHAR)) {
        mIsSearchAttribute = true ;
        mAttributeName = mScopes.at(scopeCount - 1);
        mAttributeName = mAttributeName.right(mAttributeName.length() - 1);
        mScopes.removeAt(scopeCount - 1);
    }
    scopeCount = mScopes.count();
    if((scopeCount > 0)) {
        mIsScoped = true;
        int scopeCount = mScopes.count();
        mScope = mScopes.at(scopeCount - 1);
        mScopes.removeAt(scopeCount - 1);
    }
    mIsWrapAround = isWrapAround ;
    mUseXQuery = useXQuery;
    mFirstMatch = NULL ;
}

FindTextParams::FindTextParams()
{
    setFindType(FindAllOccurrences);
    mIsCountingOnly = false;
    mSize = 0 ;
    mTextToFind = "" ;
    mIsMatchExact = false ;
    mIsCaseSensitive = false ;
    mIsOnlyChildren = false ;
    mFindTarget = FindTextParams::FIND_ALL ;
    mIsSelToBookmarks = false ;
    mIsCloseUnrelated = false ;
    mIsSearchAttribute = false;
    mIsShowSize = false ;
    mAttributeName = "" ;
    mIsScoped = false ;
    mOccurrences = 0 ;
    mUseXQuery = false;
    mIsWrapAround = true ;
    mSelection = NULL ;
    mFirstMatch = NULL ;
}

FindTextParams::~FindTextParams()
{
}

FindTextParams *FindTextParams::cloneFind()
{
    FindTextParams *cloned = new FindTextParams();
    cloned->mFindType = mFindType ;
    cloned->mIsCountingOnly = mIsCountingOnly;
    cloned->mTextToFind = mTextToFind;
    cloned->mIsMatchExact = mIsMatchExact;
    cloned->mIsCaseSensitive = mIsCaseSensitive;
    cloned->mIsOnlyChildren = mIsOnlyChildren;
    cloned->mFindTarget = mFindTarget;
    cloned->mIsSelToBookmarks = mIsSelToBookmarks;
    cloned->mIsCloseUnrelated = mIsCloseUnrelated;
    cloned->mScopes = mScopes;
    cloned->mIsSearchAttribute = mIsSearchAttribute;
    cloned->mAttributeName = mAttributeName;
    cloned->mScope = mScope;
    cloned->mIsScoped  = mIsScoped;
    cloned->mIsShowSize  = mIsShowSize;
    cloned->mOccurrences = mOccurrences;
    cloned->mSize = mSize; // size of result
    cloned->mUseXQuery = mUseXQuery;
    cloned->mIsWrapAround = mIsWrapAround;
    cloned->mSelection = mSelection ;
    return cloned ;
}

void FindTextParams::setCaseSensitive(bool value)
{
    mIsCaseSensitive = value ;
}

FindTextParams::EFindType FindTextParams::findType()
{
    return mFindType;
}

void FindTextParams::setFindType(const FindTextParams::EFindType newType)
{
    mFindType = newType ;
}

bool FindTextParams::isHiliteAll() const
{
    return (FindAllOccurrences == mFindType);
}

bool FindTextParams::isFindAllOccurrences()
{
    return FindAllOccurrences == mFindType ;
}

bool FindTextParams::isFindNext()
{
    switch(mFindType) {
    default:
        return false;
    case FindNext:
    case ReplaceAndGotoNext:
    case SkipAndGotoNext:
        return true ;
    }
}

bool FindTextParams::isFindPrev()
{
    switch(mFindType) {
    default:
        return false;
    case FindPrevious:
    case ReplaceAndGotoPrev:
    case SkipAndGotoPrev:
        return true ;
    }
}

bool FindTextParams::checkParams(bool &isErrorShown)
{
    isErrorShown = false ;
    if((FIND_ATTRIBUTE_VALUE != mFindTarget) && mIsSearchAttribute) {
        Utils::error(tr("The scope is not legal for a search of an attribute value."));
        isErrorShown = true ;
        return false ;
    }
    switch(mFindType) {
    case FindAllOccurrences:
    case FindNext:
    case FindPrevious:
    case ReplaceAndGotoNext:
    case ReplaceAndGotoPrev:
    case SkipAndGotoNext:
    case SkipAndGotoPrev:
        break;
    default:
        Utils::error(tr("The type of the search is not legal:%1.").arg(mFindType));
        isErrorShown = true ;
        return false;
    }

    return true;
}

bool FindTextParams::isTextMatched(const QString &textToExamine) const
{
    Qt::CaseSensitivity caseSensitivity =  mIsCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ;
    if(mIsMatchExact) {
        return (0 == mTextToFind.compare(textToExamine, caseSensitivity));
    } else {
        return textToExamine.contains(mTextToFind, caseSensitivity);
    }
}

bool FindTextParams::isTextBase64Matched(const QString & textToExamine) const
{
    QString textDecoded = Utils::fromBase64(textToExamine);
    Qt::CaseSensitivity caseSensitivity =  mIsCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ;
    if(mIsMatchExact) {
        return (0 == mTextToFind.compare(textDecoded, caseSensitivity));
    } else {
        return textDecoded.contains(mTextToFind, caseSensitivity);
    }
}

QStringList &FindTextParams::getScopes()
{
    return mScopes;
}


void FindTextParams::loadState()
{
    mTextToFind =  "";
    mIsMatchExact = Config::getBool(Config::KEY_SEARCH_MATCHEXACT, false);
    mIsCaseSensitive = Config::getBool(Config::KEY_SEARCH_CASEUNSITIVE, false);
    mIsOnlyChildren = Config::getBool(Config::KEY_SEARCH_ONLYCHILDREN, false);
    mFindTarget = (EFindTarget)Config::getInt(Config::KEY_SEARCH_FINDTARGET, 0);
    mIsSelToBookmarks = Config::getBool(Config::KEY_SEARCH_SEL2BOOKMARK, false);
    mIsCloseUnrelated =  Config::getBool(Config::KEY_SEARCH_CLOSEUNRELATED, true);
    mIsShowSize =  Config::getBool(Config::KEY_SEARCH_SHOWSIZE, true);
    mIsWrapAround = Config::getBool(Config::KEY_SEARCH_WRAPAROUND, true);
    mUseXQuery = Config::getBool(Config::KEY_SEARCH_USEXQUERY, false);
}

void FindTextParams::saveState() const
{
    Config::saveBool(Config::KEY_SEARCH_MATCHEXACT, mIsMatchExact);
    Config::saveBool(Config::KEY_SEARCH_CASEUNSITIVE, mIsCaseSensitive);
    Config::saveBool(Config::KEY_SEARCH_ONLYCHILDREN, mIsOnlyChildren);
    Config::saveInt(Config::KEY_SEARCH_FINDTARGET, mFindTarget);
    Config::saveBool(Config::KEY_SEARCH_SEL2BOOKMARK, mIsSelToBookmarks);
    Config::saveBool(Config::KEY_SEARCH_CLOSEUNRELATED, mIsCloseUnrelated);
    Config::saveBool(Config::KEY_SEARCH_SHOWSIZE, mIsShowSize);
    Config::saveBool(Config::KEY_SEARCH_WRAPAROUND, mIsWrapAround);
    Config::saveBool(Config::KEY_SEARCH_USEXQUERY, mUseXQuery);
}

Element  *FindTextParams::firstMatch()
{
    return mFirstMatch ;
}

void FindTextParams::start()
{
    mOccurrences = 0 ;
    mSize = 0 ;
    mFirstMatch = NULL ;
}

void FindTextParams::newOccurence(const int size)
{
    mOccurrences ++ ;
    mSize += size ;
}

void FindTextParams::setMatchExact(const bool value)
{
    mIsMatchExact = value ;
}

void FindTextParams::setOnlyChildren(const bool value)
{
    mIsOnlyChildren = value ;
}

void FindTextParams::setCountOnly(const bool value)
{
    mIsCountingOnly = value ;
}

void FindTextParams::setScope(const EFindTarget scope)
{
    mFindTarget = scope ;
}

void FindTextParams::setScopePath(const QString &scopePath)
{
    mScopes = scopePath.split(SPLIT_SCOPE_CHAR, QString::KeepEmptyParts);
    int scopeCount = mScopes.count();
    if((scopeCount > 0) && mScopes.at(scopeCount - 1).startsWith(ATTR_CHAR)) {
        mIsSearchAttribute = true ;
        mAttributeName = mScopes.at(scopeCount - 1);
        mAttributeName = mAttributeName.right(mAttributeName.length() - 1);
        mScopes.removeAt(scopeCount - 1);
    }
    scopeCount = mScopes.count();
    if((scopeCount > 0)) {
        mIsScoped = true;
        int scopeCount = mScopes.count();
        mScope = mScopes.at(scopeCount - 1);
        mScopes.removeAt(scopeCount - 1);
    }
}

void FindTextParams::addSelection(Element *newSelection)
{
    if(NULL != mSelection) {
        mSelection->append(newSelection);
    }
    if(NULL == mFirstMatch) {
        mFirstMatch = newSelection ;
    }
}

void FindTextParams::setSearchText(const QString &search)
{
    mTextToFind = search ;
}

void FindTextParams::startElement(Element * /*currentElement*/)
{

}

void FindTextParams::endElement()
{

}

bool FindTextParams::handleElementTag()
{
    return true;
}

bool FindTextParams::isExploreAllItems()
{
    return false;
}

bool FindTextParams::handleAttributeName(Attribute * /*attribute*/)
{
    return false;
}

bool FindTextParams::handleAttributeValue(Attribute * /*attribute*/)
{
    return false;
}

bool FindTextParams::handleComment()
{
    return false;
}

bool FindTextParams::handleTextElement()
{
    return false;
}

bool FindTextParams::handleTextInline(TextChunk * /*tc*/)
{
    return false;
}

bool FindTextParams::handleProcessingInstruction()
{
    return false;
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------


ReplaceTextParams::ReplaceTextParams(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isMatchExact,
                                     const bool isCaseSensitive, const bool isOnlyChildren, const EFindTarget findTarget,
                                     const bool isSelToBookmarks, const bool isCloseUnrelated, const bool isShowSize,
                                     const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection) :
    FindTextParams(findType, textToFind, isCountingOnly, isMatchExact,
                   isCaseSensitive, isOnlyChildren, findTarget,
                   isSelToBookmarks, isCloseUnrelated, isShowSize,
                   scope, isWrapAround, useXQuery, selection)
{
    initReplace();
}

ReplaceTextParams::ReplaceTextParams() : FindTextParams()
{
    initReplace();
}

ReplaceTextParams::~ReplaceTextParams()
{
}

void ReplaceTextParams::initReplace()
{
    mReplacementErrorsCount = 0 ;
    mReplacementCount = 0 ;
    mCurrentElement = NULL ;
    mUndoGroup = NULL ;
    mReplaceElement = NULL ;
    mCurrentCommand = NULL ;
}

//---

void ReplaceTextParams::setReplaceText(const QString &replace)
{
    mReplacementText = replace;
}

int ReplaceTextParams::replacementCount()
{
    return mReplacementCount;
}

int ReplaceTextParams::replacementErrorsCount()
{
    return mReplacementErrorsCount ;
}

void ReplaceTextParams::startElement(Element *currentElement)
{
    mCurrentElement = currentElement ;
    mCurrentCommand = NULL ;
}

void ReplaceTextParams::endElement()
{
    mCurrentElement = NULL ;
    mReplaceElement = NULL ;
}

QUndoCommand *ReplaceTextParams::currentUndoCommand()
{
    return mCurrentCommand;
}

void ReplaceTextParams::buildOperationElement()
{
    if(NULL != mReplaceElement) {
        return ;
    }
    mAttributes.clear();
    mTexts.clear();
    mReplaceElement = new Element("", "", NULL, NULL);
    mCurrentElement->copyTo(*mReplaceElement);
    ElUpdateInPlaceCommand *updateCommand = new ElUpdateInPlaceCommand(mCurrentElement->getUI()->treeWidget(),
            mCurrentElement->getParentRule(), mCurrentElement,
            mReplaceElement, mCurrentElement->indexPath(), mUndoGroup);
    mCurrentCommand = updateCommand ;
    foreach(Attribute * attrib, mReplaceElement->attributes) {
        mAttributes[attrib->name] = attrib;
    }
    int chunkId = 0 ;
    foreach(TextChunk * chunk, mCurrentElement->getTextChunks()) {
        mTexts[chunk] = mReplaceElement->getTextChunks().at(chunkId);
        chunkId++;
    }
    updateCommand->setHilite(isHiliteAll());

}

bool ReplaceTextParams::handleElementTag()
{
    if(canChangeXmlIdentifier(mCurrentElement->tag())) {
        buildOperationElement();
        changeElementTag();
        mReplacementCount++;
        return true;
    }
    mReplacementErrorsCount++;
    return false;
}

bool ReplaceTextParams::handleAttributeName(Attribute * attribute)
{
    if(canChangeXmlIdentifier(attribute->name)) {
        buildOperationElement();
        changeAttributeName(attribute);
        mReplacementCount++;
        return true;
    }
    mReplacementErrorsCount++;
    return false;
}

bool ReplaceTextParams::handleComment()
{
    if(canChangeComment()) {
        buildOperationElement();
        changeComment();
        mReplacementCount++;
        return true;
    }
    mReplacementErrorsCount++;
    return false;
}
bool ReplaceTextParams::handleAttributeValue(Attribute * attribute)
{
    buildOperationElement();
    changeAttributeValue(attribute);
    mReplacementCount++;
    return true;
}

void ReplaceTextParams::setCommandGroup(QUndoCommand *undoCommandGroup)
{
    mUndoGroup = undoCommandGroup;
}

QString ReplaceTextParams::applyReplacement(const QString & inputString)
{
    if(mIsMatchExact) {
        // the match has already been validated, only apply here
        return mReplacementText ;
    }
    const Qt::CaseSensitivity caseSensitivity = mIsCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ;
    // avoid modifying a shared object in place
    QString newString(inputString.data(), inputString.size());
    newString.replace(mTextToFind, mReplacementText, caseSensitivity);
    return newString;
}

bool ReplaceTextParams::canChangeXmlIdentifier(const QString &tag)
{
    QString newTag = applyReplacement(tag);
    if(Utils::checkXMLName(newTag)) {
        return true ;
    }
    return false;
}

void ReplaceTextParams::changeElementTag()
{
    QString tag = mCurrentElement->tag();
    QString newTag = applyReplacement(tag);
    mReplaceElement->setTag(newTag);
}

void ReplaceTextParams::changeComment()
{
    QString tag = mCurrentElement->getComment();
    QString newComment = applyReplacement(tag);
    mReplaceElement->setComment(newComment);
}

bool ReplaceTextParams::canChangeComment()
{
    QString tag = mCurrentElement->getComment();
    QString newComment = applyReplacement(tag);
    if(newComment.indexOf("--") >= 0) {
        return false;
    }
    return true ;
}

bool ReplaceTextParams::canChangeText(const QString & /*text*/)
{
    return true ;
}

void ReplaceTextParams::changeAttributeName(Attribute *attribute)
{
    QString tag = attribute->name;
    QString newTag = applyReplacement(tag);
    Attribute *destAttrib = mAttributes[attribute->name];
    if(NULL != destAttrib) {
        destAttrib->name = newTag ;
    }
}

void ReplaceTextParams::changeAttributeValue(Attribute *attribute)
{
    QString value = attribute->value;
    QString newValue = applyReplacement(value);
    Attribute *destAttrib = mAttributes[attribute->name];
    if(NULL != destAttrib) {
        destAttrib->value = newValue ;
    }
}

bool ReplaceTextParams::isExploreAllItems()
{
    return true ;
}

bool ReplaceTextParams::handleTextElement()
{
    bool isCData = mCurrentElement->isCDATA() ;
    if(!isCData || (isCData && canChangeText(mCurrentElement->text))) {
        buildOperationElement();
        mReplaceElement->text = applyReplacement(mCurrentElement->text);
        mReplacementCount++;
        return true;
    }
    mReplacementErrorsCount++;
    return false;
}

bool ReplaceTextParams::handleTextInline(TextChunk *tc)
{
    bool isCData = tc->isCDATA ;
    if(!isCData || (isCData && canChangeText(tc->text))) {
        buildOperationElement();
        TextChunk *localChunk = mTexts[tc];
        if(NULL != localChunk) {
            localChunk->text = applyReplacement(tc->text);
            mReplacementCount++;
            return true;
        }
    }
    mReplacementErrorsCount++;
    return false;
}

bool ReplaceTextParams::handleProcessingInstruction()
{
    buildOperationElement();
    mReplaceElement->setPIData(applyReplacement(mCurrentElement->getPIData()));
    mReplaceElement->setPITarget(applyReplacement(mCurrentElement->getPITarget()));
    mReplacementCount++;
    return true;
}
