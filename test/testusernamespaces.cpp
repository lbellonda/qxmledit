/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "testusernamespaces.h"
#include "modules/namespace/usernamespace.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/namespace/usernamespaceloader.h"
#include "comparexml.h"
#include "app.h"
#include "testsqlaccess.h"

TestUserNamespaces::TestUserNamespaces()
{
}

TestUserNamespaces::~TestUserNamespaces()
{
}

bool TestUserNamespaces::testSerialization()
{
    _testName = "testSerialization";
    if(!serializeUserNs()) {
        return false;
    }
    if(!crudUserNs()) {
        return false;
    }
    return true;
}

bool TestUserNamespaces::testFast()
{
    return testSerialization();
}

bool TestUserNamespaces::serializeUserNs()
{
    _testName = "serializeUserNs";
    if(!readNSFromDom()) {
        return false;
    }
    if(!saveNSToDom()) {
        return false;
    }
    return true;
}

const QString TestUserNamespaces::SerializedForm1 = ""
     "<?xml version='1.0' encoding='UTF-8' ?>"
     "<namespace uri=\"uri1\" defaultPrefix='' >"
     "  <prefix value=\"\"/>"
     "   <prefix value=\"v1\"/>"
     "   <prefix value='dd'/>"
     " </namespace>";

const QString TestUserNamespaces::SerializedForm2 = ""
        "<?xml version='1.0' encoding='UTF-8' ?>"
 "<namespace uri=\"uri2\" defaultPrefix='a' >"
 " </namespace>";



#define TEST(obj,prop,cfr) if(obj.prop != cfr) { return error(QString("Property %1 differs, expected: '%2' but found:'%3'").arg(#prop).arg(cfr).arg(obj.prop));}
bool TestUserNamespaces::readNSFromDom()
{
    _testName = "readNSFromDom";

    {
        UserNamespace ns;
        if( !ns.readFromSerializedXmlString(SerializedForm1)) {
            return error("read from string 1");
        }
        // test values
        TEST(ns,uri(),"uri1");
        TEST(ns,preferredPrefix(),"");
        QStringList expected;
        expected << "" << "v1" << "dd" ;
        if(!compareStringList("prefixes 1", ns.prefixes(), expected)) {
            return false;
        }
    }
    {
        UserNamespace ns;
        if( !ns.readFromSerializedXmlString(SerializedForm2)) {
            return error("read from string 2");
        }
        // test values
        TEST(ns,uri(),"uri2");
        TEST(ns,preferredPrefix(),"a");
        QStringList expected;
        if(!compareStringList("prefixes 2", ns.prefixes(), expected)) {
            return false;
        }
    }
    // negative test
    {
        const QString SerializedForm3 = ""
         "<namespace uri=\"uri2\" defaultPrefix='a' >"
         " /namespace>";
        UserNamespace ns;
        if( ns.readFromSerializedXmlString(SerializedForm3)) {
            return error("false reading from string 3");
        }
    }
    return true;
}


bool TestUserNamespaces::saveNSToDom()
{
    _testName = "saveNSToDom";

    {
        UserNamespace ns;
        ns.setUri("uri1");
        ns.setPreferredPrefix("");
        QStringList prefixes;
        prefixes << "" << "v1" << "dd" ;
        ns.setPrefixes(prefixes);
        QString s1 = ns.toXMLSerializedString();
        CompareXML compare;
        if(!compare.compareXMLAsStringAsUtf8(s1, SerializedForm1)) {
            return error(QString("Ser 1:%1").arg(compare.errorString()));
        }
    }
    {
        UserNamespace ns;
        ns.setUri("uri2");
        ns.setPreferredPrefix("a");
        QString s1 = ns.toXMLSerializedString();
        CompareXML compare;
        if(!compare.compareXMLAsStringAsUtf8(s1, SerializedForm2)) {
            return error(QString("Ser 2:%1").arg(compare.errorString()));
        }
    }

    return true;
}

static void cleanNs(QList<UserNamespace*> userNamespaces)
{
    foreach(UserNamespace *ns, userNamespaces) {
        delete ns;
    }
    userNamespaces.clear();
}

static void setNs( UserNamespace *un, const QString &name, const QString  &uri, const QString &prefix)
{
    un->setName(name);
    un->setUri(uri);
    un->setPreferredPrefix(prefix);
}

bool TestUserNamespaces::crudUserNs()
{
    _testName = "crudUserNs";
    App app;

    SQLLiteTestAccess access;
    if(!access.init()) {
        return error("init access");
    }
    app.sessionDBPath = access.dbFileName;
    if(!app.init1()) {
        return error("init app");
    }
    DataInterface *dataInterface = app.data()->storageManager();
    // load:0
    {
        UserNamespaceLoader loader;
        bool isOk = false;
        QList<UserNamespace*> userNamespaces = loader.loadUserNamespaces(dataInterface, isOk);
        if(!isOk) {
            return error("loading data");
        } else {
            if(!userNamespaces.isEmpty()) {
                return error(QString("Init, expecting no data, found:%1").arg(userNamespaces.size()));
            }
        }
        cleanNs(userNamespaces);
    }
    // insert 2
    {
        UserNamespaceLoader loader;
        UserNamespace *ns = loader.createUserNamespace(dataInterface);
        setNs(ns, "name1", "uri1", "prefix1");
        UserNamespace *ns1 = loader.createUserNamespace(dataInterface);
        setNs(ns1, "name2", "uri2", "prefix2");

        if(!loader.saveUserNamespace(dataInterface, ns) ) {
            return error("inserting ns 1");
        }
        if(!loader.saveUserNamespace(dataInterface, ns1) ) {
            return error("inserting ns 2");
        }
    }
    // reload:2
    {
        UserNamespaceLoader loader;
        bool isOk = false;
        QList<UserNamespace*> userNamespaces = loader.loadUserNamespaces(dataInterface, isOk);
        UserNamespace *ans1 = NULL;
        if(!isOk) {
            return error("loading data");
        } else {
            if(userNamespaces.size()!=2) {
                return error(QString("Reload, expecting 2 data, found:%1").arg(userNamespaces.size()));
            }
            UserNamespace *ns1 = userNamespaces.at(0);
            UserNamespace *ns2 = userNamespaces.at(1);
            if( ns1->name() == "name1") {
                ans1 = ns1 ;
                if( !((ns1->uri() == "uri1") && (ns2->name() == "name2") && (ns2->uri()=="uri2") ) ) {
                    return error("Reload 1.1, ns differ.");
                }
            } else if( ns1->name() == "name2") {
                ans1 = ns2 ;
                if(!( (ns1->uri() == "uri2") && (ns2->name() == "name1") && (ns2->uri()=="uri1") ) ) {
                    return error("Reload 1.2, ns differ.");
                }
            } else {
                return error(QString("1 After reload, expecting name1 or name2 but found:'%1'").arg(ns1->name()));
            }
        }
        ans1->setName("x");
        ans1->setUri("y");
        if(!loader.saveUserNamespace(dataInterface, ans1)) {
            return error("saving first");
        }
        cleanNs(userNamespaces);
        userNamespaces = loader.loadUserNamespaces(dataInterface, isOk);
        if(!isOk) {
            return error("loading data 3 ");
        } else {
            if(userNamespaces.size()!=2) {
                return error(QString("Reload 3, expecting 2 data, found:%1").arg(userNamespaces.size()));
            }
            UserNamespace *ns1 = userNamespaces.at(0);
            UserNamespace *ns2 = userNamespaces.at(1);
            if( ns1->name() == "x") {
                ans1 = ns1 ;
                if( !((ns1->uri() == "y") && (ns2->name() == "name2") && (ns2->uri()=="uri2") ) ) {
                    return error("Reload 3.1, ns differ.");
                }
            } else if( ns1->name() == "name2") {
                ans1 = ns2 ;
                if( !((ns1->uri() == "uri2") && (ns2->name() == "x") && (ns2->uri()=="y") ) ) {
                    return error("Reload 3.2, ns differ.");
                }
            } else {
                return error(QString("3 After reload, expecting name1 or name2 but found:'%1'").arg(ns1->name()));
            }
            if(!loader.deleteUserNamespace(dataInterface, ans1)) {
                return error("Error deleting ns");
            }

            userNamespaces = loader.loadUserNamespaces(dataInterface, isOk);
            if(1 != userNamespaces.size()) {
                return error(QString("after delete, expecting 1, found:%1").arg(userNamespaces.size()));
            }
            foreach(UserNamespace* userNamespace, userNamespaces) {
                if(!loader.deleteUserNamespace(dataInterface, userNamespace)) {
                    return error("Error deleting all ns");
                }
            }
        }
        cleanNs(userNamespaces);
    }
    return true;
}

#define NS_FILE_EMPTY       "../test/data/namespace/t_empty.xml"
#define NS_FILE_COMPLEX     "../test/data/namespace/t_complex.xml"
#define NS_FILE_SHADOWED    "../test/data/namespace/t_shadowed.xml"

bool TestUserNamespaces::testVis(const QString &fileStart, QHash<QString,QString> expected, QList<int> &sel )
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Element *selectedElement = app.mainWindow()->getRegola()->findElementByArray(sel);
    if(NULL == selectedElement) {
        return error("no selected element");
    }
    QHash<QString,QString> found = selectedElement->findVisibleNamespaces();
    if(!testHash(expected, found)) {
        return false;
    }
    return true;
}

// testa visibilita ns in elemento
bool TestUserNamespaces::testVisibility()
{
    _testName = "testVisibility/None";
    QList<int> sel;
    sel << 1 << 0 << 0 ;
    {
        QHash<QString,QString> expected;
        if(!testVis(NS_FILE_EMPTY, expected, sel ) ) {
            return false;
        }
    }
    _testName = "testVisibility/Complex";
    {
        QHash<QString,QString> expected;
        expected.insert("", "abc");
        expected.insert("a", "xyx");
        expected.insert("b", "123");
        if(!testVis(NS_FILE_COMPLEX, expected, sel ) ) {
            return false;
        }
    }
    _testName = "testVisibility/Shadowed";
    {
        QHash<QString,QString> expected;
        expected.insert("", "abc");
        expected.insert("a", "bbb");
        expected.insert("b", "123");
        if(!testVis(NS_FILE_SHADOWED, expected, sel ) ) {
            return false;
        }
    }

    return true;
}
