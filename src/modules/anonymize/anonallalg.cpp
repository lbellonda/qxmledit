/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2022 by Luca Bellonda and individual contributors  *
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

#include "anonallalg.h"
#include "qxmleditconfig.h"

#ifdef  QXML_DEBUG
#define DD(x)    do{x;}while(0)
#else
#define DD(x)
#endif

AnonAllAlg::AnonAllAlg(const bool parmAutodelete, AnonProducer *theProducer) : AnonAlg(parmAutodelete, theProducer)
{
    _useLegacyAlgorithm = Config::getBool(Config::KEY_ANON_TEXT_LEGACY, false) ;
    _algStatRandomProvider = NULL;
}

AnonAllAlg::~AnonAllAlg()
{
    EMPTYPTRLIST(_anonStatData, AnonStatAlgValue);
}

bool AnonAllAlg::isUseLegacy()
{
    return _useLegacyAlgorithm;
}

void AnonAllAlg::setUseLegacy(const bool value)
{
    _useLegacyAlgorithm = value ;
}

QString AnonAllAlg::processText(AnonAlgStatContext &context, const QString &path, const QString &input)
{
    int length = input.length();
    QString result;
    if(_useLegacyAlgorithm || path.isEmpty() || !scanned()) {
        for(int i = 0 ; i < length ; i ++) {
            QChar ch = input.at(i);
            if(ch.isLetter()) {
                QChar chRes = _producer->nextLetter(ch.isUpper(), AnonProducer::ASCII);
                result.append(chRes);
            } else if(ch.isDigit()) {
                QChar chRes = _producer->nextDigit();
                result.append(chRes);
            } else {
                result.append(ch);
            }
        }
    } else {
        AnonStatAlgValue* algStatValue = valueFor(path);
        algStatValue->setRandomProvider(_algStatRandomProvider);
        result = algStatValue->value(context, _producer);
    }
    return result ;
}

bool AnonAllAlg::scanned()
{
    return !_anonStatData.isEmpty();
}

AnonStatAlgValue* AnonAllAlg::valueFor(const QString &path)
{
    DD(qDebug() << QString("AnonAllAlg::valueFor " % path));
    AnonStatAlgValue* algStatValue = _anonStatData[path];
    if(NULL == algStatValue) {
        algStatValue = new AnonStatAlgValue();
        _anonStatData[path] = algStatValue;
    }
    return algStatValue;
}

void AnonAllAlg::scan(AnonAlgStatContext &context, const QString &path, const QString &input)
{
    if(!_useLegacyAlgorithm) {
        AnonStatAlgValue* algStatValue = valueFor(path);
        algStatValue->scan(context, input);
    }
}

bool AnonAllAlg::needScan()
{
    return !_useLegacyAlgorithm;
}

void AnonAllAlg::setAlgStatRandomProvider(AlgStatRandomProvider *newValue)
{
    _algStatRandomProvider = newValue ;
}

QString AnonAllAlg::dumpAsString()
{
    QString result = QString("Count:%1\n").arg(_anonStatData.size());
    foreach(const QString &key, _anonStatData.keys()) {
        result += QString("  Key: '%1', value/total: '%2'\n").arg(key).arg(_anonStatData[key]->total());
    }
    result += "--- end\n";
    return result;
}
