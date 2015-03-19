/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#ifndef METADATAINFO_H
#define METADATAINFO_H
#include "modules/metadata/basecomplexvariable.h"
#include "modules/metadata/metadataparser.h"
#include <QList>
#include <QMap>

class MetadataUserProvider
{
public:
    virtual ~MetadataUserProvider() {}
    virtual QString user() = 0 ;
};

class MetadataDateProvider
{
public:
    virtual ~MetadataDateProvider() {}
    virtual QString date() = 0 ;
};

class UpdatableMetadata
{
private:
    PseudoAttribute _creationUser;
    PseudoAttribute _creationDate;
    PseudoAttribute _updateUser;
    PseudoAttribute _updateDate;
    PseudoAttribute _revision;
    PseudoAttribute _metaVersion;
    QList<PseudoAttribute*> _other;
    MetadataUserProvider *_userProvider;
    MetadataDateProvider *_dateProvider;

    bool readAttribute(PseudoAttribute *attr, PseudoAttribute *target);
    void writeAnAttribute(QString &result, PseudoAttribute *attr, const QString &defaultValue = "");

public:
    UpdatableMetadata();
    ~UpdatableMetadata();
    void read(MetadataParsedResult *input);
    QString toProcessingInstruction();
    void update(const bool insertOnlyIfsNotExisting);
    void markNewRecord();

    static QString CreationUserName;
    static QString CreationDateName;
    static QString UpdateUserName;
    static QString UpdateDateName;
    static QString RevisionName;
    static QString MetaVersionName;
    //
    static QString MetaDataModelVersion;
    static QString MetaDataStartVersion;
    //--
    MetadataUserProvider *userProvider() const;
    void setUserProvider(MetadataUserProvider *userProvider);
    MetadataDateProvider *dateProvider() const;
    void setDateProvider(MetadataDateProvider *dateProvider);
    PseudoAttribute *metaVersion();
    PseudoAttribute *revision();
    PseudoAttribute *updateDate();
    PseudoAttribute *updateUser();
    PseudoAttribute *creationDate();
    PseudoAttribute *creationUser();
};

class MetadataInfo : public MetadataUserProvider, public MetadataDateProvider
{
private:
    UpdatableMetadata _updatable;
    //-
public:
    PseudoAttribute project;
    PseudoAttribute copyright;
    PseudoAttribute version;
    PseudoAttribute domain;
    PseudoAttribute name;
private:
    QList<PseudoAttribute*> userDefined;
    MetadataUserProvider *_userProvider;
    MetadataDateProvider *_dateProvider;
    QMap<QString, PseudoAttribute*> _attributes;

    void setupVariable(PseudoAttribute *attr, const QString &name);
    bool lookForOneAttribute(MetadataParsedResult *attributes, PseudoAttribute *inAttr, PseudoAttribute *targetAttr, const QString &name);

public:
    MetadataInfo();
    ~MetadataInfo();

    bool parseUpdatable(const QString &input, const int row);
    static const QString QXMLEDIT_TARGET_PI;
    static const QString UPDATABLE_ATTR;
    static const QString TYPE_ATTR;
    static const QString VALUE_ATTR;
    //------
    static const QString ProjectMetaType;
    static const QString CopyrightMetaType;
    static const QString VersionMetaType;
    static const QString DomainMetaType;
    static const QString NameMetaType;
    //------

    MetadataUserProvider *userProvider() const;
    void setUserProvider(MetadataUserProvider *userProvider);
    MetadataDateProvider *dateProvider() const;
    void setDateProvider(MetadataDateProvider *dateProvider);
    UpdatableMetadata *updatable() ;
    void markNewRecord();
    bool parseOtherMetadata(const QString &inputData, const int row);
    QList<PseudoAttribute*> manualAttributeListByCondition(const bool readOnes, const bool writeOnes);
    PseudoAttribute *attribute(const QString &name);
    //--
    virtual QString user();
    virtual QString date();
};

class MetadataInfoFactory
{
    MetadataUserProvider *_userProvider;
    MetadataDateProvider *_dateProvider;

    void _initInstance(MetadataInfo* newInstance);

public:
    static void initInstance(MetadataInfo* newInstance);
    MetadataInfoFactory();
    ~MetadataInfoFactory();

    static MetadataUserProvider *userProvider() ;
    static void setUserProvider(MetadataUserProvider *userProvider);
    static MetadataDateProvider *dateProvider() ;
    static void setDateProvider(MetadataDateProvider *dateProvider);
};

#endif // METADATAINFO_H
