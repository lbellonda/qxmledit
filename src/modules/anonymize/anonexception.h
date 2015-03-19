/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef ANONEXCEPTION_H
#define ANONEXCEPTION_H

#include <xmlEdit.h>
#include "libQXmlEdit_global.h"
#include <QDomElement>
#include "anonexception.h"

/*
strategia:
    di default si include
    le ecceszioni srvono per escludere
    escludi oggetto
    escludi con figli
    includi (di oggetto escluso)
  includi con figli
   sostituisci con valore fisso
  sostituisci con contatore
*/

class LIBQXMLEDITSHARED_EXPORT AnonInclusionCriteria
{
public:
    enum Criteria {
        /*!
          \brief : include this object (attribute, text, element with all attributes using the namespace)
          */
        Exclude,
        /*!
          *\brief : include this object and all the children (attribute, text, element with all attributes using the namespace)
          */
        ExcludeWithChildren,
        /*!
          *\brief : include this object (attribute, text, element with all attributes using the namespace)
          */
        Include,
        /*!
          \brief : include this object and all the children (attribute, text, element with all attributes using the namespace)
          */
        IncludeWithChildren,
        EndValue,
        StartValue = Exclude
    };
};

class LIBQXMLEDITSHARED_EXPORT AnonType
{
public:
    enum Type {
        /*!
         \brief: use whatever algorithm
         */
        UseDefault,
        /*!
          \brief: use a fixed value
          */
        FixedValue,
        /**
          \brief: use a specific formula for this object
          */
        UseFormula,
        EndValue,
        StartValue = UseDefault
    };
};

class LIBQXMLEDITSHARED_EXPORT AnonException
{
public:

private:
    /*!
     * \brief the inclusion type of the exception
     */
    AnonInclusionCriteria::Criteria _criteria;
    /*!
     * \brief whether the exception should include or exclude the object
     */
    AnonType::Type _anonType;
    /*!
     * \brief the path relative to the exception.
     */
    QString _path;
    bool _useNamespace;
    QString _fixedValue;

public:
    AnonException();
    virtual ~AnonException();
    AnonException *clone();
    QString fixedValue();

    AnonInclusionCriteria::Criteria criteria() ;
    void setCriteria(const AnonInclusionCriteria::Criteria parmCriteria) ;
    AnonType::Type anonType();
    void setAnonType(const AnonType::Type newType);
    QString path() const;
    void setPath(const QString &path);
    QString fixedValue() const;
    void setFixedValue(const QString &value);

    bool readFromDom(const QDomElement &element);
    bool saveToDom(QDomElement &element);
    bool isUseNamespace() const;
    void setUseNamespace(bool useNamespace);

    bool isInclude();

    bool compare(AnonException *other);
};

/*
input: the context

per applicare
per ogni elemento: trova path,
                   controlla se sto operando una eccezione.
                   Se eccezione usa namespace, trova ns

devo usare un qname:

ho name,
       localname e ns

cerca con i nomi semplici
                   cerca tra le eccezioni qname
                per ogni attributo, ripeti, usando solo il nome


                   se elemento cerca per nome nelle eccezioni per nome
fullpath, normal path piu attributo

dal contest applica
*/

#endif // ANONEXCEPTION_H
