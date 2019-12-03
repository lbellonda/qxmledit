/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2019 by Luca Bellonda and individual contributors       *
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

#include "testsplit.h"
#ifdef  QXMLEDIT_JS_SCRIPT
#include "utils.h"
#include "extraction/scripting/model/extractionscriptfiltermodel.h"

bool TestSplit::unitTestFilterInterface()
{
    if(!unitTestFilterInterfaceProperties()) {
        return false;
    }
    if(!unitTestFilterEventChain()) {
        return false;
    }
    if(!unitTestFilterInterfaceAccessibility()) {
        return false;
    }
    return true ;
}

bool TestSplit::unitTestFilterEventChain()
{
    if(!unitAllEventsChainOnScriptManager()) {
        return false;
    }
    return true ;
}

bool TestSplit::unitTestFilterInterfaceProperties()
{
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterInterfaceProperties/empty";
        ExtractionOperationScriptContext a, b;
        if(!a.compareTo(b)) {
            return error("a not empty 1");
        }
        TestWritableExtractionOperationScriptContext c;
        if(!a.compareTo(c)) {
            return error("a not empty 2");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterInterfaceProperties/path";
        ExtractionOperationScriptContext a, b;
        TestWritableExtractionOperationScriptContext c;
        a.setPath("a");
        if(a.compareTo(b)) {
            return error("1");
        }
        b.setPath("a");
        if(!a.compareTo(b)) {
            return error("2");
        }
        c.setPath("a");
        if(!a.compareTo(c)) {
            return error("3");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterInterfaceProperties/level";
        ExtractionOperationScriptContext a, b;
        TestWritableExtractionOperationScriptContext c;
        a.setLevel(34);
        if(a.compareTo(b)) {
            return error("1");
        }
        b.setLevel(34);
        if(!a.compareTo(b)) {
            return error("2");
        }
        c.setLevel(34);
        if(!a.compareTo(c)) {
            return error("3");
        }
    }
    return true ;
}

bool TestSplit::unitTestFilterInterfaceAccessibility()
{
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterInterfaceAccessibility/empty";
        TestExtractionExecutorHelper helper ;
        if(!helper.execute("var a =1;")) {
            return error("1");
        }
        if(!helper.reference->compareTo(*helper.compare)) {
            return error("2");
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterInterfaceAccessibility/one";
        TestExtractionExecutorHelper helper ;
        const QString value("12xyz");
        helper.reference->setPath(value);
        if(!helper.execute("contextDouble.path = context.path")) {
            return error("0");
        }
        if(!helper.reference->compareTo(*helper.compare)) {
            return error("1");
            if(helper.reference->path() != value) {
                return error("2");
            }
            if(helper.compare->path() != value) {
                return error("3");
            }
        }
    }
    {
        _subTestName = "unitTestScriptingBase/unitTestFilterInterfaceAccessibility/two";
        TestExtractionExecutorHelper helper ;
        const int value = 12345;
        helper.reference->setLevel(value);
        if(!helper.execute("contextDouble.level = context.level")) {
            return error("0");
        }
        if(!helper.reference->compareTo(*helper.compare)) {
            return error("1");
            if(helper.reference->level() != value) {
                return error("2");
            }
            if(helper.compare->level() != value) {
                return error("3");
            }
        }
    }
    return true ;
}

#endif
