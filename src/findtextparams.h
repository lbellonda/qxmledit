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

#ifndef QXMLEDITWIDGET_FINDTEXTPARAMS_H
#define QXMLEDITWIDGET_FINDTEXTPARAMS_H

#include "libQXmlEdit_global.h"
#include <QApplication>
#include <QUndoCommand>
#include "xmlEdit.h"

class Element;
class Attribute;

class LIBQXMLEDITSHARED_EXPORT FindTextParams
{
    Q_DECLARE_TR_FUNCTIONS(FindTextParams)

public:
    enum EFindTarget {
        FIND_ALL,
        FIND_TAG,
        FIND_ATTRIBUTE_NAME,
        FIND_ATTRIBUTE_VALUE,
        FIND_TEXT,
        FIND_TEXT_BASE64,
        FIND_COMMENT

    };

public:
    enum EFindType {
        FindAllOccurrences,
        FindNext,
        FindPrevious,
        ReplaceAndGotoNext,
        ReplaceAndGotoPrev,
        SkipAndGotoNext,
        SkipAndGotoPrev
    };


protected:

    EFindType   mFindType;
    bool        mIsCountingOnly;
    QString   mTextToFind;
    bool      mIsMatchExact;
    bool      mIsCaseSensitive;
    bool      mIsOnlyChildren;
    EFindTarget mFindTarget;
    bool      mIsSelToBookmarks;
    bool      mIsCloseUnrelated;
    QStringList mScopes;
    bool      mIsSearchAttribute;
    QString   mAttributeName;
    QString   mScope;
    bool      mIsScoped ;
    bool      mIsShowSize ;
    int       mOccurrences;
    int       mSize; // size of result
    bool      mUseXQuery;
    bool        mIsWrapAround;
    QList<Element*> *mSelection;

public:
    FindTextParams();
    FindTextParams(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isMatchExact, const bool isCaseSensitive,
                   const bool isOnlyChildren, const EFindTarget findTarget, const bool isSelToBookmarks,
                   const bool isCloseUnrelated, const bool isShowSize, const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection = NULL);
    virtual ~FindTextParams();

    void init(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isMatchExact,
              const bool isCaseSensitive, const bool isOnlyChildren, const EFindTarget findTarget,
              const bool isSelToBookmarks, const bool isCloseUnrelated, const bool isShowSize,
              const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection = NULL);


    EFindType findType();
    void setFindType(const EFindType newType);
    bool isFindAllOccurrences();
    bool isFindNext();
    bool isFindPrev();

    bool checkParams(bool &isErrorShown);

    void saveState() const;
    void loadState();

    const QString &getTextToFind() const {
        return mTextToFind;
    }

    bool isCountingOnly() const {
        return mIsCountingOnly ;
    }

    bool isTextMatched(const QString &textToExamine) const ;
    bool isTextBase64Matched(const QString & textToExamine) const;

    bool isLookOnlyChildren() const {
        return mIsOnlyChildren ;
    }
    bool isHiliteAll() const ;

    EFindTarget getFindTarget() const {
        return mFindTarget;
    }
    bool isSelToBookmarks() const {
        return mIsSelToBookmarks;
    }
    bool isCloseUnrelated() const {
        return mIsCloseUnrelated;
    }
    bool isCaseSensitive() const {
        return mIsCaseSensitive;
    }
    bool isIsMatchExact() const {
        return mIsMatchExact;
    }
    bool isSearchInAttribute() const {
        return mIsSearchAttribute;
    }
    bool isSearchWithScope() const {
        return mIsScoped ;
    }
    const QString &attributeName() const {
        return mAttributeName;
    }

    const QString &mainScope() const {
        return mScope;
    }

    int getOccurrences() const {
        return mOccurrences;
    }

    bool isShowSize() const {
        return mIsShowSize;
    }

    bool isWrapAround() const {
        return mIsWrapAround;
    }

    int size() const {
        return mSize;
    }

    bool useXQuery() const {
        return mUseXQuery;
    }

    QStringList &getScopes();

    void start();

    void newOccurence(const int size);

    void setMatchExact(const bool value);
    void setOnlyChildren(const bool value);
    void setCountOnly(const bool value);
    void setScope(const EFindTarget scope);
    void setScopePath(const QString &scopePath);

    void addSelection(Element *newSelection);

    void setSearchText(const QString &search);

    virtual void startElement(Element *currentElement);
    virtual void endElement();
    virtual bool handleElementTag();
    virtual bool handleAttributeName(Attribute *attribute);
    virtual bool handleAttributeValue(Attribute *attribute);
    virtual bool handleComment();
    virtual bool handleTextElement();
    virtual bool handleTextInline(TextChunk *tc);
    virtual bool handleProcessingInstruction();
    virtual bool isExploreAllItems();

    void setCaseSensitive(bool value);

    FindTextParams *cloneFind();
};

class TextChunk;

class LIBQXMLEDITSHARED_EXPORT ReplaceTextParams : public FindTextParams
{

    QString mReplacementText;
    int mReplacementErrorsCount;
    int mReplacementCount;
    Element *mCurrentElement;
    Element *mReplaceElement;
    QUndoCommand *mUndoGroup;
    QUndoCommand *mCurrentCommand;
    QHash<QString, Attribute*> mAttributes;
    QHash<TextChunk*, TextChunk*> mTexts;

    void initReplace();
    void changeElementTag();
    void changeComment();
    bool canChangeComment();
    bool canChangeText(const QString & text);
    void changeAttributeName(Attribute *attribute);
    void changeAttributeValue(Attribute *attribute);
    bool canChangeXmlIdentifier(const QString &tag);
    void buildOperationElement();

public:
    ReplaceTextParams();
    ReplaceTextParams(const EFindType findType, const QString &textToFind, const bool isCountingOnly, const bool isMatchExact, const bool isCaseSensitive,
                      const bool isOnlyChildren, const EFindTarget findTarget, const bool isSelToBookmarks,
                      const bool isCloseUnrelated, const bool isShowSize, const QString &scope, const bool isWrapAround, const bool useXQuery, QList<Element*> *selection = NULL);
    virtual ~ReplaceTextParams();
    void setReplaceText(const QString &replace);

    QString applyReplacement(const QString &inputString);

    int replacementCount();
    int replacementErrorsCount();
    QUndoCommand *currentUndoCommand();

    virtual void startElement(Element *currentElement);
    virtual void endElement();
    virtual bool handleElementTag();
    virtual bool handleAttributeName(Attribute *attribute);
    virtual bool handleAttributeValue(Attribute *attribute);
    virtual bool handleComment();
    virtual bool handleTextElement();
    virtual bool handleTextInline(TextChunk *tc);
    virtual bool handleProcessingInstruction();
    virtual bool isExploreAllItems();

    void setCommandGroup(QUndoCommand *undoCommandGroup);
};

#endif // QXMLEDITWIDGET_FINDTEXTPARAMS_H
