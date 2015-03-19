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
#ifndef XPROPERTIES_H
#define XPROPERTIES_H


#define DECL_QPROP(type,name,setFunc) type name() ; void setFunc( const type new##name) ;

#define DECL_QPROPS(name,setFunc) QString name() ; void setFunc( const QString& new##name) ;

#define IMPL_QPROP(type,clazz,name,setFuncName,propertyName) type clazz::name()\
    {\
        return _##name ;\
    }\
    \
    void clazz::setFuncName( const type new##name)\
    {\
        if( _##name != new##name ) {\
            _##name = new##name ;\
            emit propertyChanged(propertyName);\
        }\
    }

#define IMPL_QPROPS(clazz,name,setFuncName,propertyName) QString clazz::name()\
    {\
        return _##name ;\
    }\
    \
    void clazz::setFuncName( const QString & new##name)\
    {\
        if( _##name != new##name ) {\
            _##name = new##name ;\
            emit propertyChanged(propertyName);\
        }\
    }




#define PROPERTY_ELEMENT_FIXED   "ELEMENT_FIXED"
#define PROPERTY_ELEMENT_MIXED   "ELEMENT_MIXED"
#define PROPERTY_ELEMENT_ABSTRACT  "ELEMENT_ABSTRACT"
#define PROPERTY_ELEMENT_NILLABLE   "ELEMENT_NILLABLE"
#define PROPERTY_ELEMENT_DEFAULTVALUE   "ELEMENT_DEFAULTVALUE"
#define PROPERTY_ELEMENT_REF        "ELEMENT_REF"
#define PROPERTY_ELEMENT_SUBSTITUTIONGROUP  "ELEMENT_SUBSTITUTIONGROUP"
#define PROPERTY_ELEMENT_MAXOCCURS      "ELEMENT_MAXOCCURS"
#define PROPERTY_ELEMENT_MINOCCURS      "ELEMENT_MINOCCURS"
#define PROPERTY_ELEMENT_FINAL   "ELEMENT_FINAL"
#define PROPERTY_ELEMENT_BLOCK   "ELEMENT_BLOCK"
#define PROPERTY_ELEMENT_TYPE   "ELEMENT_TYPE"
#define PROPERTY_ELEMENT_CATEGORY  "ELEMENT_CATEGORY"

#endif // XPROPERTIES_H
