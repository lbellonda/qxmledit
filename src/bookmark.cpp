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
#include <QtXml/QDomDocument>
#include "xmlEdit.h"
#include "bookmark.h"
#include "element.h"
#include "regola.h"

Bookmark::Bookmark(Bookmarks *newParent, Element *newElement)
{
    parent = newParent;
    element = newElement;
    calculateRow();
    //connect(element->getParentRule(), SIGNAL(uiDeleted(Element*)), this, SLOT(onUiDeleted(Element*)));
}

Bookmark::~Bookmark()
{
    remove();
}

void Bookmark::onUiDeleted(Element *elementImplied)
{
    if(elementImplied == element) {
        remove();
    }
}

void Bookmark::remove()
{
    if(NULL != element) {
        //disconnect(element->getParentRule(), SIGNAL(uiDeleted(Element*)), this, SLOT(onUiDeleted(Element*)));
        element = NULL ;
    }
}

bool Bookmark::owns(Element *elementToCompare)
{
    if((elementToCompare == element) && (NULL != element)) {
        return true;
    }
    return false;
}

void Bookmark::calculateRow()
{
    if(NULL != element) {
        _row = element->getRow();
    } else {
        _row = -1 ;
    }
}

QString Bookmark::row()
{
    return _row;
}

Element *Bookmark::getElement()
{
    return element;
}

D(
    void Bookmark::dump(const int index)
{
    printf("%d: %s\n", index, _row.toAscii().data());
}
)
//-----------------------------------------------------------------------------------------

Bookmarks::Bookmarks()
{
    previousIndex = -1 ;
    nextIndex = -1;
    isModified = false ;
}

Bookmarks::~Bookmarks()
{
    clear();
}

void Bookmarks::clear()
{
    foreach(Bookmark * item, bookmarks) {
        //disconnect(bookmark->getElement(), SIGNAL(uiDeleted()), this, SLOT(uiDeleted()));
        item->remove();
        delete item;
    }
    bookmarks.clear();
    elements.clear();
}

bool Bookmarks::add(Element *element)
{
    bool isInserted = false;
    if(!elements.contains(element)) {
        Bookmark *newBookmark = new Bookmark(this, element);
        QString newRow = newBookmark->row();
        recalculateRows();
        int len = bookmarks.size();
        int insertedIndex = -1;
        for(int i = 0 ; i < len ; i ++) {
            Bookmark *actual = bookmarks.at(i);
            if(actual->row() >= newRow) {
                bookmarks.insert(i, newBookmark);
                insertedIndex = i;
                break;
            }
        }
        if(-1 == insertedIndex) {
            bookmarks.append(newBookmark);
            //insertedIndex = bookmarks.size() - 1;
        }
        recalculatePositions(newRow);
        elements[element] = newBookmark ;
        element->refreshUI();
        // can sort by ui row?
        isInserted = true;
    }
#ifdef  DEBUG
    dump();
#endif
    return isInserted ;
}


void Bookmarks::remove(Element *element, const bool dontUpdate)
{
    if(!elements.contains(element)) {
        return ;
    }
    QString row;
    int index = findBookmark(element) ;
    if(index >= 0) {
        Bookmark *bookmark = bookmarks.at(index);
        if(!dontUpdate) {
            recalculateRows();
            row = bookmark->row();
        }
        bookmark->remove();
        bookmarks.remove(index);
        if(NULL != bookmark) {
            delete bookmark;
            nextIndex = -1 ;
            previousIndex = -1 ;
        }
    }
    elements.remove(element);
    if(!dontUpdate) {
        element->refreshUI();
        if(row.length() > 0) {
            recalculatePositions(row);
        }
    }
    D(dump());
}

void Bookmarks::toggle(Element *element)
{
    if(!elements.contains(element)) {
        add(element);
    } else {
        remove(element, false);
    }
}

Element* Bookmarks::gotoNext()
{
    if(nextIndex == -1) {
        return NULL ;
    }
    Bookmark* selected = bookmarks[nextIndex];
    D(dump());
    return selected->getElement();
}

Element* Bookmarks::gotoPrevious()
{
    if(previousIndex == -1) {
        return NULL ;
    }
    Bookmark* selected = bookmarks[previousIndex];
    D(dump());
    return selected->getElement();
}

Element* Bookmarks::gotoItem(const int itemIndex)
{
    if((itemIndex >= 0) && (itemIndex < bookmarks.size())) {
        D(dump());
        return bookmarks.at(itemIndex)->getElement();
    }
    return NULL;
}

int Bookmarks::findBookmark(Element *element)
{
    int len = bookmarks.size();
    for(int i = 0 ; i < len ; i ++) {
        Bookmark *bookmark = bookmarks.at(i);
        if(bookmark->owns(element)) {
            return i;
        }
    }
    return -1;
}

bool Bookmarks::isBookmarked(Element* element)
{
    return  elements.contains(element);
}

D(
    void Bookmarks::dump()
{
    printf("--Bookmarks state\n");
    int len = bookmarks.size();
    printf(" %d bookmarks", len);
    for(int i = 0 ; i < len ; i ++) {
        Bookmark *bkm = bookmarks.at(i);
        bkm->dump(i);
    }
    printf(" previous:%d, next:%d\n", previousIndex, nextIndex);
    printf("------------\n");
}
)

int Bookmarks::bookmarkSize()
{
    return bookmarks.size();
}

int Bookmarks::nextBookmark()
{
    return nextIndex;
}

int Bookmarks::previousBookmark()
{
    return previousIndex;
}

void Bookmarks::setModified()
{
    isModified = true ;
}

void Bookmarks::recalculateRows()
{
    if(isModified) {
        foreach(Bookmark * item, bookmarks) {
            item->calculateRow();
        }
        isModified = false ;
    }
}

void Bookmarks::recalculatePositions(const QString &actualPosition)
{
    int prev = -1 ;
    int next = -1 ;
    recalculateRows();
    int bookmarksSize = bookmarks.size();
    for(int item = 0 ; item < bookmarksSize ; item ++) {
        const QString position = bookmarks[item]->row();
        if(actualPosition < position) {
            prev = item - 1 ;
            next = item;
            break;
        } else if(actualPosition == position) {
            prev = item - 1;
            next = item + 1;
            break;
        } else {
            prev = item;
            next = item + 1;
        }
    }
    if(prev >= bookmarksSize) {
        prev = -1 ;
    }
    if(next >= bookmarksSize) {
        next = -1 ;
    }
    if(prev > next) {
        // TODO: come fa?
    }
    previousIndex = prev;
    nextIndex = next;
}

void Bookmarks::newSelection(Element *element)
{
    QString position = "" ;
    if(NULL != element) {
        position = element->getRow();
    }
    recalculatePositions(position);
}

/*
algorithm to mavigate bookmarks:
bkm are ordered by row number of relative elements

if(!current) get first

goto next:
 find next
 if not found, restart
 if found and is current stop
 if not found stop

 the sort must be edit (insert/delete) aware

 Each edit operation mark a flag in bkm structure to recalculate positions
 nota che i bkm sono smpre ordinati inserendo o eliminando, non (cotnrolla coreente) cambia l-ordinamento
 degli oggetti esistenti
 Devo calcolrlo solo al momento dell'inserimento
 quando uso current devo controllare quando viene eliminato e settarlo all'inserimento
*/

QVector<Bookmark*> &Bookmarks::getBookmarks()
{
    return bookmarks;
}
