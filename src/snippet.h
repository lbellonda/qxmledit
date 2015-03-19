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
#ifndef SNIPPET_H
#define SNIPPET_H

#include <QStringList>
#include <QDateTime>
#include <QMap>
#include <QDomNode>
#include <QApplication>

class Snippet
{
    Q_DECLARE_TR_FUNCTIONS(FindTextParams)

    QString _id;
    QString _name;
    QString _description;
    QStringList _tags;
    QString _xmlPayload;
    QDateTime _creationDate;
    QDateTime _updateDate;
    QString _author;
    QString _fileName;
    bool _isReadOnly;

public:
    Snippet();
    ~Snippet();

    QString id();
    void setId(const QString &newId);
    // ---
    QString name();
    void setName(const QString &newName);
    // ---
    QString description();
    void setDescription(const QString &newDescription);
    // ---
    void setPayload(const QString &newPayload);
    QString payload();

    QStringList tags();
    void setTags(const QStringList &newTags);

    //void setUpdateUser( const QString &user );
    void setUpdateDate(const QDateTime &date);
    //void setCreationUser( const QString &user );
    void setCreationDate(const QDateTime &date);

    //QString updateUser();
    QDateTime updateDate();
    //QString creationUser();
    QDateTime creationDate();

    bool isReadOnly();
    void setReadOnly(const bool newReadOnly);

    static void loadMockSnippet(QMap<QString, Snippet*> &snippetArray);
    static void resetSnippetContainer(QMap<QString, Snippet*> &snippets);

    bool save(const QString &filePath);
    bool read(const QString &filePath);
    bool remove(const QString &filePath);

private:
    bool scanData(QDomNode &rootNode);

};

#endif // SNIPPET_H
