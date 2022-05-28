/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2022 by Luca Bellonda and individual contributors       *
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

#include "testabout.h"
#include "licensedialog.h"

TestAbout::TestAbout()
{
    Config::setBackend(&_configBackend);
}

TestAbout::~TestAbout()
{
    Config::setBackend(NULL);
}

bool TestAbout::testFast()
{
    return testLicense();
}

bool TestAbout::testUnit() {
    _testName = "testUnit";
    if(!testLicense()) {
        return false;
    }
    return true ;
}

bool TestAbout::testLicense()
{
    _subTestName = "testLicense";
    if(!testLicenseStart()) {
        return false;
    }
    if(!testLicenseStarted()) {
        return false;
    }
    return true;
}

bool TestAbout::testLicenseStart() {
    _subTestName = "testLicense/Start";
    Config::setBackend(&_configBackend);
    LicenseDialog::testLicenseValid = false;
    Config::saveBool(Config::KEY_GENERAL_LICENSE_AGREED, false);
    //
    LicenseDialog::licenseAgreement();
    //
    if(!Config::getBool(Config::KEY_GENERAL_LICENSE_AGREED, false)) {
        return error("After dialog, no settings");
    }
    if(!LicenseDialog::testLicenseValid) {
        return error("After dialog, no settings");
    }
    return true;
}

bool TestAbout::testLicenseStarted() {
    _subTestName = "testLicense/Started";
    Config::setBackend(&_configBackend);
    LicenseDialog::testLicenseValid = false;
    Config::saveBool(Config::KEY_GENERAL_LICENSE_AGREED, true);
    //
    LicenseDialog::licenseAgreement();
    //
    if(!Config::getBool(Config::KEY_GENERAL_LICENSE_AGREED, false)) {
        return error("After dialog, no settings");
    }
    if(LicenseDialog::testLicenseValid) {
        return error("After dialog, no settings");
    }
    return true;
}
