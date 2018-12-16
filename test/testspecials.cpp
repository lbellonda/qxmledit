/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#include "testspecials.h"
#include "app.h"
#include "modules/specialized/xinclude/xincludeeditormanager.h"
#include "modules/specialized/xinclude/xincludedialog.h"


#define BASE_PATH "../test/data/xml/special/"

TestSpecials::TestSpecials()
{

}

TestSpecials::~TestSpecials()
{

}

bool TestSpecials::testFast()
{
    _testName = "testFast" ;
    return testXInclude();
}

class TestXIncludeSetInternal :public NamespaceEditorInsertChoiceProvider, XIncludeEditorProvider
{
    public:

    TestBase *base;
    int test ;

    TestXIncludeSetInternal()
    {
        base = NULL ;
        test = 0 ;
    }

    HandlerForInsert *handleInsertElementForSpecialized(QWidget * /*parent*/, QList<HandlerForInsert*> *handlers)
    {
        foreach( HandlerForInsert* handler, *handlers ) {
            if( handler->nameSpace == NamespaceManager::XIncludeNamespace ) {
                if( ( 4 == test ) || ( 5 == test ) ) {
                    handler->outputSelectedCode = XIncludeEditorManager::XIncludeFallbackItemCode ;
                } else {
                    handler->outputSelectedCode = XIncludeEditorManager::XIncludeItemCode;
                }
                return handler;
            }
        }
        return NULL ;
    }

    bool provideXIncludeEditor(QWidget *parent, Element *element)
    {
        XIncludeDialog dlg(parent, element);
        switch(test) {
        default:
            return false ;
        case 1:
            {
                if(!base->setTextWidget(&dlg, "href", "disclaimer.xml")) {
                    return false;
                }
                dlg.accept();
                return true;
            }
        case 2:
            {
                if(!base->setTextWidget(&dlg, "href", "count.txt")) {
                    return false;
                }
                if(!base->setComboWidget(&dlg, "parse", "xml")) {
                    return false;
                }
                dlg.accept();
                return true;
            }
        case 3:
            {
                if(!base->setTextWidget(&dlg, "href", "a.xml")) {
                    return false;
                }
                if(!base->setTextWidget(&dlg, "xpointer", "aaa-bbb")) {
                    return false;
                }
                dlg.accept();
                return true;
            }
        }
    }
    //-------------
    static bool idoSetTestBase(TestBase * aptr, MainWindow * window, Element * /*selection*/, const int testCode)
    {
        TestXIncludeSetInternal internal;
        internal.base = aptr ;
        internal.test = testCode ;
        NamespaceManager *nsManager = window->getEditor()->getRegola()->namespaceManager();
        if(NULL == nsManager) {
            return false;
        }
        nsManager->setProviderForInsert(&internal);
        XIncludeEditorManager *xIncludeEditorManager = nsManager->xIncludeEditorManager();
        if( NULL == xIncludeEditorManager ) {
            return false;
        }
        xIncludeEditorManager->setProviderXIncludeEditor(&internal);
        window->getEditor()->insertSpecial();
        return true;
    }
    static bool idoSetTest1(TestBase * aptr, MainWindow * window, Element * selection)
    {
        return idoSetTestBase(aptr, window, selection, 1);
    }
    // fragment
    static bool idoSetTest2(TestBase * aptr, MainWindow * window, Element * selection)
    {
        return idoSetTestBase(aptr, window, selection, 2);
    }
    // fragment
    static bool idoSetTest3(TestBase * aptr, MainWindow * window, Element * selection)
    {
        return idoSetTestBase(aptr, window, selection, 3);
    }
    static bool idoSetTest4(TestBase * aptr, MainWindow * window, Element * selection)
    {
        return idoSetTestBase(aptr, window, selection, 4);
    }
    static bool idoSetTest5(TestBase * aptr, MainWindow * window, Element * selection)
    {
        return idoSetTestBase(aptr, window, selection, 5);
    }
};

#define FILE_XINCLUDE_1_START   BASE_PATH "xinclude/00_start.xml"
#define FILE_XINCLUDE_1_END   BASE_PATH "xinclude/00_basic.xml"
#define FILE_XINCLUDE_XML_END  BASE_PATH "xinclude/01_xml.xml"
#define FILE_XINCLUDE_FRAGMENT_END  BASE_PATH "xinclude/02_fragment.xml"
#define FILE_XINCLUDE_FALLBACK_START BASE_PATH "xinclude/03_fallback_start.xml"
#define FILE_XINCLUDE_FALLBACK_END BASE_PATH "xinclude/03_fallback.xml"
#define FILE_XINCLUDE_FALLBACK2_START BASE_PATH "xinclude/04_fallback_start.xml"
#define FILE_XINCLUDE_FALLBACK2_END BASE_PATH "xinclude/04_fallback.xml"

bool TestSpecials::testXInclude()
{
    _testName = "testXInclude" ;
    QList<int> selPath ;
    //---
    selPath << 1 ;
    // as is
    if(!testSkeletonBase(   "1", FILE_XINCLUDE_1_START, FILE_XINCLUDE_1_END, TestXIncludeSetInternal::idoSetTest1, selPath) ) {
        return false;
    }
    selPath.clear() ;
    selPath << 1 ;
    if(!testSkeletonBase(   "2", FILE_XINCLUDE_1_START, FILE_XINCLUDE_XML_END, TestXIncludeSetInternal::idoSetTest2, selPath) ) {
        return false;
    }
    selPath.clear() ;
    selPath << 1 ;
    if(!testSkeletonBase(   "3", FILE_XINCLUDE_1_START, FILE_XINCLUDE_FRAGMENT_END, TestXIncludeSetInternal::idoSetTest3, selPath) ) {
        return false;
    }
    selPath.clear() ;
    selPath << 1 << 0;
    if(!testSkeletonBase(   "4", FILE_XINCLUDE_FALLBACK_START, FILE_XINCLUDE_FALLBACK_END, TestXIncludeSetInternal::idoSetTest4, selPath) ) {
        return false;
    }
    selPath.clear() ;
    selPath << 1 << 0;
    if(!testSkeletonBase(   "5", FILE_XINCLUDE_FALLBACK2_START, FILE_XINCLUDE_FALLBACK2_END, TestXIncludeSetInternal::idoSetTest5, selPath) ) {
        return false;
    }
    //
    return true;
}
