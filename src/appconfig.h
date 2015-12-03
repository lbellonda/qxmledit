/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "qxmleditconfig.h"

/**
  *\brief configurations specific to the application
  */

class AppConfig /*: public Config*/
{
public:
    static const QString KEY_ENCODING_USEHEXVALUES;
    static const QString KEY_ENCODING_USEDCODECS;
    static const QString KEY_ENCODING_TRADENC1;
    static const QString KEY_ENCODING_TRADENC2;
    static const QString KEY_ENCODING_TRADENC3;
    static const QString KEY_ENCODING_FONTFAMILY;
    static const QString KEY_ENCODING_FONTSTYLE;
    static const QString KEY_ENCODING_FONTWHEIGHT;
    static const QString KEY_ENCODING_FONTPTSIZE;

private:
    AppConfig();
    ~AppConfig();
};

#endif // APPCONFIG_H
