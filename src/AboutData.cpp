/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2021 by Luca Bellonda and individual contributors  *
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

#include "authorinfo.h"
#include "xmlEdit.h"

QList<AuthorInfo*> authorsInfo()
{
    QList<AuthorInfo*> authors ;
    AuthorInfo *newAuthor;
    newAuthor = new AuthorInfo("Luca Bellonda", "lbellonda@gmail.com", AR_DEVELOPER, "");
    authors.append(newAuthor);
    newAuthor = new AuthorInfo("GalaxySemi", "", AR_CONTRIBUTOR, "");
    authors.append(newAuthor);
    newAuthor = new AuthorInfo("Michael Palimaka", "kensington@gentoo.org", AR_CONTRIBUTOR, "");
    authors.append(newAuthor);
    newAuthor = new AuthorInfo("Pavel Fric", "fripohled.blogspot.com", AR_TRANSLATOR, "Czech");
    authors.append(newAuthor);
    newAuthor = new AuthorInfo("Konstantin Blashkin", "<kostet@etersoft.ru>", AR_TRANSLATOR, "Russian");
    authors.append(newAuthor);
    newAuthor = new AuthorInfo("Vitaly Lipatov", "<lav@etersoft.ru>", AR_TRANSLATOR, "Russian");
    authors.append(newAuthor);

    return authors ;
}

void deleteAuthorsInfo(QList<AuthorInfo*> &authors)
{
    foreach(AuthorInfo * author,  authors) {
        delete author;
    }
    authors.clear();
}

const QString AuthorInfo::appName(APPLICATION_NAME) ;
const QString AuthorInfo::version(VERSION_IDENTIFIER) ;
const QString AuthorInfo::copyright = QString::fromUtf8("\u00A9 2008-2018 Luca Bellonda");
const QString AuthorInfo::license("GNU Library Public License Version 2");
const QString AuthorInfo::other("Icons by Oxygen (www.oxygen-icons.org), covered by LGPLv3 license (see README document)."
#ifdef  QWT_PLOT3D
                                "\nUses QwtPlot3D, copyright (C) 2003-2005 Michael Bieber with some modifications."
#endif
                               );


