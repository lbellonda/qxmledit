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

#ifndef BOOKMARK_H
#define BOOKMARK_H

#include "xmlEdit.h"

#include "element.h"
class Bookmarks;

class Bookmark : public QObject
{
    Q_OBJECT

    Bookmarks *parent;
    Element *element;
    QString    _row;

public:
    Bookmark(Bookmarks *newParent, Element *point);
    ~Bookmark();

    void remove();
    bool owns(Element *elementToCompare);
    void calculateRow();
    QString row();
    Element *getElement();
    D(void dump(const int index));

private slots:
    void onUiDeleted(Element *elementImplied);

};

class Bookmarks : public QObject
{
    Q_OBJECT

    QVector<Bookmark*>   bookmarks;
    QHash<Element*, Bookmark*> elements;
    int nextIndex;
    int previousIndex;
    bool isModified;

public:
    Bookmarks();
    ~Bookmarks();

    void clear();
    void remove(Element *element, const bool dontUpdate);
    bool add(Element *element);
    void toggle(Element *element);
    bool isBookmarked(Element* element);
    Element* gotoNext();
    Element* gotoPrevious();
    Element* gotoItem(const int itemIndex);

    int bookmarkSize();
    int nextBookmark();
    int previousBookmark();

    void setModified();
    void newSelection(Element *element);
    QVector<Bookmark*> &getBookmarks();

private:
    int findBookmark(Element *element);
    void recalculateRows();
    void recalculatePositions(const QString &actualPosition);
    D(void dump());
};

#endif
