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

#include "sessionfiltermodel.h"

SessionFilterModel::SessionFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

SessionFilterModel::~SessionFilterModel()
{
}


void SessionFilterModel::setFilter(const QString &newFilter)
{
    _filter = newFilter ;
}

bool SessionFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(_filter.isEmpty()) {
        return true;
    }
    if(!sourceParent.isValid()) {
        return true ;
    }
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if(sourceModel()->data(index).toString().contains(_filter)) {
        return true ;
    }
    return false;
}
