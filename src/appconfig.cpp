/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#include "appconfig.h"

//-- encoding
const QString AppConfig::KEY_ENCODING_USEHEXVALUES("encoding/useHexValues");
const QString AppConfig::KEY_ENCODING_USEDCODECS("encoding/usedCodecs");
const QString AppConfig::KEY_ENCODING_TRADENC1("encoding/tradEnc1");
const QString AppConfig::KEY_ENCODING_TRADENC2("encoding/tradEnc2");
const QString AppConfig::KEY_ENCODING_TRADENC3("encoding/tradEnc3");
const QString AppConfig::KEY_ENCODING_FONTFAMILY("encoding/fontFamily");
const QString AppConfig::KEY_ENCODING_FONTSTYLE("encoding/fontStyle");
const QString AppConfig::KEY_ENCODING_FONTWHEIGHT("encoding/fontWeight");
const QString AppConfig::KEY_ENCODING_FONTPTSIZE("encoding/fontPtSize");


AppConfig::AppConfig()
{
}

AppConfig::~AppConfig()
{
}
