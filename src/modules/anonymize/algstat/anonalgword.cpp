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

#include "anonalgword.h"
#include "modules/anonymize/xmlanonutils.h"

AnonStatAlgWord::AnonStatAlgWord()
{
    _type = AnonProducer::ASCII;
}

AnonStatAlgWord::~AnonStatAlgWord()
{
    //
}

AnonAlgFreqTarget* AnonStatAlgWord::newChild()
{
    return NULL; // pure virtual
}

AnonStatAlgPattern *AnonStatAlgWord::findPattern(const QString& pattern)
{
    if(_patterns.contains(pattern)) {
        return _patterns[pattern];
    }
    return NULL ;
}

AnonStatAlgPattern *AnonStatAlgWord::newPattern(const int id, const QString & value)
{
    AnonStatAlgPattern *aNewPattern = new AnonStatAlgPattern(id, value);
    return aNewPattern;
}

AnonStatAlgPattern *AnonStatAlgWord::findCreatePattern(AnonAlgStatContext &context, const QString& pattern)
{
    if(!_patterns.contains(pattern)) {
        AnonStatAlgPattern *aNewPattern = newPattern(_patterns.size(), pattern);
        if(NULL == aNewPattern) {
            context.setError(QObject::tr("Unable to create pattern"), pattern);
            return NULL ;
        }
        _patterns.insert(pattern, aNewPattern);
        insertTarget(aNewPattern->id(), aNewPattern);
    }
    return _patterns[pattern];
}

void AnonStatAlgWord::scan(AnonAlgStatContext &context, const QString &token)
{
    ScanPatternInfo info;
    const QString &patternToken = AnonStatAlgPattern::scanPattern(token, info);
    AnonStatAlgPattern* pattern = findCreatePattern(context, patternToken);
    if(NULL == pattern) {
        context.setError(QObject::tr("Unable to scan a pattern"), QObject::tr("findCreatePattern for %1").arg(patternToken));
    }
    setTypeIfNotSet(info.type);
    AnonAlgFreqCount::newHit(pattern);
}

QString AnonStatAlgWord::value(AnonAlgStatContext &context, AnonProducer *generator, const AnonProducer::ESeqType type)
{
    AnonStatAlgPattern* pattern = (AnonStatAlgPattern*)randomChoice();
    if(NULL == pattern) {
        context.setError(QObject::tr("Unable to find a pattern"), QObject::tr("word: %1").arg(id()));
        return "";
    }
    return pattern->value(generator, type);
}

bool AnonStatAlgWord::setTypeIfNotSet(const AnonProducer::ESeqType aType)
{
    return XmlAnonUtils::setTypeIfNotSet(_type, aType);
}

AnonProducer::ESeqType AnonStatAlgWord::type() const
{
    return _type ;
}

QString AnonStatAlgWord::dumpAsString()
{
    QString result ;
    result += QString("WORD, pattern Count :%1\n").arg(_patterns.size());
    foreach(const QString &key, _patterns.keys()) {
        AnonStatAlgPattern *pattern = _patterns[key];
        result += QString(" key: %1, pattern: %2\n").arg(key).arg(pattern->pattern());
    }
    return result ;
}
