/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#include "xsdeditor/xsddefinitions.h"
#include "xsdeditor/xschema.h"
#include "utils.h"

/*
 * namespace resolution rules
 * element                 | schema has |
 * has ns                  | target ns  | resolution
 * ------------------------+------------+---------------------
 *                         |   yes      | use main schema target ns if equal
 *  yes                    +------------+---------------------
 *                         |   no       | use other ns
 * ------------------------+------------+---------------------
 *       resolve           |   yes      | main schema has targetNamespace? defaultns? yes: use schema data
 *  no   schema            |            |                       no: use other ns
 *       default           +------------+---------------------
 *       namespacedefault  |   no       | use the schema
 * ------------------------+------------+---------------------
 *
 * element
 * prefix+name -> ns+name (if no prefix and default ns, use defaultns)
 *
 */

//-------------------------------------------------------------------------------------
