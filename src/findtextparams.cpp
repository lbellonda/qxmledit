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

#include <QtGui>
#include "xmlEdit.h"
#include "findtextparams.h"
#include "utils.h"
#include "regola.h"
#include "config.h"

#define SPLIT_SCOPE_CHAR '/'
#define ATTR_CHAR        '@'

FindTextParams::FindTextParams(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isHiliteAll, const bool isMatchExact,
                               const bool isCaseSensitive, const bool isOnlyChildren, const EFindTarget findTarget,
                               const bool isSelToBookmarks, const bool isCloseUnrelated, const bool isShowSize,
                               const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection)
{
    init(findType, textToFind, isCountingOnly, isHiliteAll, isMatchExact,
         isCaseSensitive, isOnlyChildren, findTarget,
         isSelToBookmarks, isCloseUnrelated, isShowSize,
         scope, isWrapAround, useXQuery, selection);

}

void FindTextParams::init(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isHiliteAll, const bool isMatchExact,
                          const bool isCaseSensitive, const bool isOnlyChildren, const EFindTarget findTarget,
                          const bool isSelToBookmarks, const bool isCloseUnrelated, const bool isShowSize,
                          const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection)
{
    mFindType = findType ;
    mSize = 0 ;
    mSelection = selection ;
    mIsCountingOnly = isCountingOnly ;
    mTextToFind = textToFind ;
    mIsHiliteAll = isHiliteAll ;
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
}

FindTextParams::FindTextParams()
{
    mFindType = FindAllOccurrences;
    mSize = 0 ;
    mTextToFind = "" ;
    mIsHiliteAll = false ;
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
    mUseXQuery = false;
    mIsWrapAround = true ;
    mSelection = NULL ;
}

FindTextParams::~FindTextParams()
{
}

FindTextParams::EFindType FindTextParams::findType()
{
    return mFindType;
}

bool FindTextParams::isFindAllOccurrences()
{
    return FindAllOccurrences == mFindType ;
}

bool FindTextParams::isFindNext()
{
    return FindNext == mFindType ;
}

bool FindTextParams::isFindPrev()
{
    return FindPrevious == mFindType ;
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
    mIsHiliteAll =  Config::getBool(Config::KEY_SEARCH_HIGHLIGHALL, true);
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
    Config::saveBool(Config::KEY_SEARCH_HIGHLIGHALL, mIsHiliteAll);
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

void FindTextParams::start()
{
    mOccurrences = 0 ;
    mSize = 0 ;
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
}


