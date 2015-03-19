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


#ifndef REGOLADEFINITIONS_H
#define REGOLADEFINITIONS_H

// this file is meant to be included from regola

#include <QtGui>
#include <QtXml/qdom.h>
#include <QtXml/QDomAttr>
#include <QtXml/QXmlAttributes>
#include <QInputDialog>
#include "xmlEdit.h"
#include "utils.h"
#include "element.h"
#include "findtextparams.h"
#include "regola.h"
#include "editelement.h"
#include "config.h"
#include "edittextnode.h"
#include "xsaxhandler.h"
#include "xmlutils.h"
#include "documenttype.h"
#include "editprocessinginstruction.h"

#include "undo/undomoveupcommand.h"
#include "undo/undomovedowncommand.h"
#include "undo/undodeletecommand.h"
#include "undo/undoinsertcommand.h"
#include "undo/undoeditcommand.h"
#include "undo/undopasteattributescommand.h"
#include "modules/copyattr/copiedattribute.h"
#include "modules/search/searchxquery.h"
#include "modules/metadata/metadatainfo.h"
#include "qxmleditdata.h"


#define XMLNS_PREFIX    "xmlns"
#define ATTRIBUTE_SCHEMA_LOCATION    "xsi:schemaLocation"
#define SCHEMA_LOCATION    "schemaLocation"
#define ATTRIBUTE_SCHEMA_NONS_LOCATION    "xsi:noNamespaceSchemaLocation"
#define SCHEMA_NONS_LOCATION    "noNamespaceSchemaLocation"
#define SCHEMA_URI "http://www.w3.org/2001/XMLSchema-instance"
#define XSI_NAMESPACE   "xsi"
#define XSI_NAMESPACE_COMPLETE   "xmlns:xsi"
#define XML_TARGET  "xml"

#endif // REGOLADEFINITIONS_H
