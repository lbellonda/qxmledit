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

#include "testanonymize.h"
#include "modules/anonymize/algstat/anonalgvalue.h"
#include "modules/anonymize/anonseqproducer.h"
#include "modules/anonymize/anonfixedproducer.h"
#include "modules/anonymize/anoncodealg.h"
#include "modules/anonymize/anoncontext.h"
#include "modules/anonymize/algstat/anoncharutils.h"
#include "modules/anonymize/xmlanonutils.h"
#include "comparexml.h"
#include "qxmleditconfig.h"
#include "utils.h"

#define DATA_FOLDER    "../test/data/anon/"

#define FILE_STAT_T1        DATA_FOLDER "argstat_t1.xml"
#define FILE_STAT_T2        DATA_FOLDER "argstat_t2.xml"
#define FILE_STAT_T2A       DATA_FOLDER "argstat_t2a.xml"
#define FILE_STAT_T3        DATA_FOLDER "argstat_t3.xml"
#define FILE_STAT_TONLY     DATA_FOLDER "argstat_tonly.xml"
#define FILE_STAT_1T        DATA_FOLDER "argstat_t.xml"

#define FILE_STAT_TWESTERN          DATA_FOLDER "argstat_w.xml"
#define FILE_STAT_TEAST             DATA_FOLDER "argstat_e.xml"
#define FILE_STAT_TCYRILLIC         DATA_FOLDER "argstat_c.xml"
#define FILE_STAT_TCYRILLICUPPER    DATA_FOLDER "argstat_cu.xml"
#define FILE_STAT_TMIXED            DATA_FOLDER "argstat_m.xml"
#define FILE_STAT_TMIXED1           DATA_FOLDER "argstat_m1.xml"
#define FILE_STAT_TWESTERNUPPER     DATA_FOLDER "argstat_wu.xml"
#define FILE_STAT_TEASTUPPER        DATA_FOLDER "argstat_eu.xml"

//-----------------

extern bool __qxmledit_test_nextLetterCond ;

static QString dumpFreqTargets(AnonAlgFreqCount *accumulator);

bool TestAnonymize::testUnitAlgStat()
{
    _testName = "testUnitAlgStat";
    if(!testAnonUtils()) {
        return false;
    }
    if(!testAnonStatAlgVaria()) {
        return false;
    }
    if(!testAnonStatFreqTarget()) {
        return false;
    }
    if(!testAnonStatContext()) {
        return false;
    }
    if(!testAnonStatMean()) {
        return false;
    }
    if(!testAnonStatPrepare()) {
        return false;
    }
    if(!testAnonStatChars()) {
        return false;
    }
    if(!testAnonStatRandomChoice()) {
        return false;
    }
    if(!testAnonStatTargetValue()) {
        return false;
    }
    if(!testUnitAnonStatAlgPattern()) {
        return false;
    }
    if(!testUnitAnonStatAlgWord()) {
        return false;
    }
    if(!testUnitAnonStatAlgWordChain()) {
        return false;
    }
    if(!testUnitAlgStatScanValue()) {
        return false;
    }
    if(!testUnitAlgStatValue()) {
        return false;
    }
    return true;
}

bool TestAnonymize::testUnitAlgStatScanValue()
{
    _subTestName = "testUnitAlgStatScanValue";
    if(!testAlgStatScanValueNull()) {
        return false;
    }
    if(!testAlgStatScanValueNull()) {
        return false;
    }
    if(!testAlgStatScanValueEmpty()) {
        return false;
    }
    if(!testAlgStatScanValueOneWord()) {
        return false;
    }
    if(!testAlgStatScanValueDoubleWord()) {
        return false;
    }
    if(!testAlgStatScanValue3Word()) {
        return false;
    }
    if(!testAlgStatScanValueW1()) {
        return false;
    }
    if(!testAlgStatScanValueW2()) {
        return false;
    }
    if(!testAlgStatScanValueW3()) {
        return false;
    }
    if(!testAlgStatScanValueW4()) {
        return false;
    }
    if(!testAlgStatScanValueW5()) {
        return false;
    }
    if(!testAlgStatScanValueW6()) {
        return false;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueCase(QStringList input, const int expectedTotal, QList<QPair<int,int> > &expectedItems )
{
    AnonStatAlgValue anonAlgValue;
    AnonAlgStatContext context;
    foreach( QString singleInput, input) {
        anonAlgValue.scan(context, singleInput);
        if(context.isError()) {
            return error(QString("For input '%1' error: '%2'  detail: '%3'").arg(singleInput).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    if(anonAlgValue.total()!=expectedTotal) {
        return error(QString("Expected total %1, was %2").arg(expectedTotal).arg(anonAlgValue.total()));
    }
    QHash<int, AnonAlgFreqTarget*> children = anonAlgValue.children();
    QPair<int,int> expectedItem;
    foreach( expectedItem, expectedItems) {
        if(!children.contains(expectedItem.first)) {
            QString keys ;
            foreach( int aKey, children.keys()) {
                keys += "'";
                keys += QString::number(aKey);
                keys += "', ";
            }
            return error(QString("No item for key %1, keys %2").arg(expectedItem.first).arg(keys));
        }
        AnonAlgFreqTarget* item = children[expectedItem.first];
        if(item->count()!=expectedItem.second) {
            QString keys ;
            foreach( int key, children.keys()) {
                keys += ",";
                keys += QString::number(key);
            }
            return error(QString("Item for key %1, expected %2, found %3 for keys %4")
                         .arg(expectedItem.first).arg(expectedItem.second).arg(item->count())
                         .arg(keys));
        }
    }
    return true;
}

static QList<QPair<int,int> > itemsForArray( const int count, const int occurrences1[], const int occurrences2[]) {
    QList<QPair<int,int> > result ;
    FORINT(i, count) {
        QPair<int,int> newPair(occurrences1[i], occurrences2[i]);
        result.append(newPair);
    }
    return result;
}

bool TestAnonymize::testAlgStatScanValueNull()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueNull";
    QList<QPair<int,int> > expected ;
    if(!testAlgStatScanValueCase(QStringList(), 0, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueEmpty()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueEmpty";
    QList<QPair<int,int> > expected = itemsForArray(0, NULL, NULL);
    if(!testAlgStatScanValueCase(QStringList()<<"", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueOneWord()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueOneWord";
    const int expectedKey[] = {1};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueDoubleWord()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueDoubleWord";
    const int expectedKey[] = {2};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc def", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValue3Word()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValue3Word";
    const int expectedKey[] = {3};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc def k", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueW1()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueW1";
    const int expectedKey[] = {3, 1};
    const int expectedCount[] = {1, 1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc def k"<<"abc", 2, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueW2()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueW2";
    const int expectedKey[] = {1, 2, 3};
    const int expectedCount[] = {2, 1, 1};
    QList<QPair<int,int> > expected = itemsForArray(3, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc def k"<<"abc"<<"abc"<<"abc e", 4, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueW3()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueW3";
    const int expectedKey[] = {1};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc\u00C4\00C8\u00E0def", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueW4()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueW4";
    const int expectedKey[] = {1};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc\u00C4\u00C8\u00E0def,4", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueW5()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueW5";
    const int expectedKey[] = {2};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc\u00C4\t\u00C8\u00E0def,4", 1, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgStatScanValueW6()
{
    _subTestName = "testAlgStatScanValue/testAlgStatScanValueW6";
    const int expectedKey[] = {7};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expected = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanValueCase(QStringList()<<"abc\u00C4\t\u00C8 \u00E0def,\n45\r6  7\t\t\n\r///", 1, expected)) {
        return false ;
    }
    return true;
}


//--------------------------

bool TestAnonymize::testAlgStatWordChainCase(QList<QStringList> input, QStringList &expectedPatterns )
{
    AnonStatAlgWordChain anonStatAlgWordChain;
    AnonAlgStatContext context;
    foreach( QStringList singleInput, input) {
        anonStatAlgWordChain.scan(context, singleInput);
        if(context.isError()) {
            return error(QString("Ctx error at input: %1").arg(singleInput.join(",")));
        }
    }
    if(anonStatAlgWordChain.wordsCount()!=expectedPatterns.size()) {
        return error(QString("Expected total %1, was %2")
                     .arg(expectedPatterns.size())
                     .arg(anonStatAlgWordChain.wordsCount()));
    }
    QList<AnonStatAlgWord*> words = anonStatAlgWordChain.words();
    FORINT(index, expectedPatterns.size()) {
        const QString &expected = expectedPatterns.at(index);
        if(NULL == words.at(index)->findPattern(expected) ) {
            return error(QString("Error at index %1 expected '%2' found NULL").arg(index).arg(expected));
        }
    }
    return true;
}

bool TestAnonymize::testAnonStatWordChain3Word()
{
    _subTestName = "testUnitAnonAlgStatWordChain/testAnonStatWordChain3Word";
    QStringList p1, p2, p3;
    p1 <<"ab2"<<"fgv"<<"12#";
    p2 <<"ab2"<<"fgv"<<"12#";
    p3 <<"ab2"<<"fgv"<<"12#";
    if(!testAlgStatWordChainCase(QList<QStringList>() <<p1<<p2<<p3, QStringList()<<"aa0"<<"aaa"<<"00#")) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAnonStatWordChainDoubleWord()
{
    _subTestName = "testUnitAnonAlgStatWordChain/testAnonStatWordChainDoubleWord";
    QStringList p1, p2, p3;
    p1 <<"ab2"<<"fgv";
    p2 <<"ab2"<<"fgv";
    p3 <<"ab2"<<"fgv";
    if(!testAlgStatWordChainCase(QList<QStringList>() <<p1<<p2<<p3, QStringList()<<"aa0"<<"aaa")) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAnonStatWordChainOneWord()
{
    _subTestName = "testUnitAnonAlgStatWordChain/testAnonStatWordChainOneWord";
    QStringList p1, p2, p3;
    p1 <<"ab2";
    p2 <<"ab2";
    p3 <<"ab2";
    if(!testAlgStatWordChainCase(QList<QStringList>() <<p1<<p2<<p3, QStringList()<<"aa0")) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAnonStatWordChainEmpty()
{
    _subTestName = "testUnitAnonAlgStatWordChain/testAnonStatWordChainEmpty";
    QStringList p1, p2, p3, expected;
    if(!testAlgStatWordChainCase(QList<QStringList>() <<p1<<p2<<p3, expected)) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testIsInInterval(AnonAlgFreqTarget *target, const double input, const bool expected )
{
    const bool result = target->isInInterval0To1(input);
    if(result != expected) {
        return error( QString("Is in interval error for %1 expected %2 (%3, %4)")
                      .arg(input).arg(expected)
                      .arg(target->minInterval0To1Value())
                      .arg(target->maxInterval0To1Value()));
    }
    return true;
}

bool TestAnonymize::testAnonStatForTargetValue(AnonAlgFreqTarget *target)
{
    if(target->count()!=0) {
        return error(QString("AnonAlgFreqTarget count start is %1").arg(target->count()));
    }
    if(target->maxInterval0To1Value()!=0) {
        return error(QString("AnonAlgFreqTarget maxInterval0To1Value start is %1").arg(target->maxInterval0To1Value()));
    }
    if(target->minInterval0To1Value()!=0) {
        return error(QString("AnonAlgFreqTarget minInterval0To1Value start is %1").arg(target->minInterval0To1Value()));
    }
    target->setLastInInterval0To1();
    if(target->maxInterval0To1Value()!=1) {
        return error(QString("AnonAlgFreqTarget maxInterval0To1Value set is %1").arg(target->maxInterval0To1Value()));
    }
    if(target->minInterval0To1Value()!=0) {
        return error(QString("AnonAlgFreqTarget minInterval0To1Value set is %1").arg(target->minInterval0To1Value()));
    }
    return true ;
}

class TestAnonStatAlgValue : public AnonStatAlgValue {
protected:
    inline virtual AnonAlgFreqTarget* newChild() {
        return NULL;
    }

};

bool TestAnonymize::testAnonStatTargetValue()
{
    _subTestName = "testAnonStatTargetValue";
    AnonStatAlgWordChain chain;
    if(!testAnonStatForTargetValue(&chain)) {
        return false;
    }
    chain.setRandomValueInterval0To1(0.5, 0.7);
    if(!(
            testIsInInterval(&chain, 0, false)
        &&  testIsInInterval(&chain, 0.495, false)
        &&  testIsInInterval(&chain, 0.5, true)
        &&  testIsInInterval(&chain, 0.6, true)
        &&  testIsInInterval(&chain, 0.7, true)
        &&  testIsInInterval(&chain, 0.71, false)
        &&  testIsInInterval(&chain, 0.8, false)
        &&  testIsInInterval(&chain, 0.9, false)
        &&  testIsInInterval(&chain, 1, false) )) {
        return false ;
    }
    {
        // scan
        TestAnonStatAlgValue    anonStatAlgValue;
        AnonAlgStatContext context;
        const bool result = anonStatAlgValue.scan(context, "input");
        if(result) {
            return error("E 1");
        }
        if(!context.isError()) {
            return error("E 2");
        }
    }
    return true;
}

bool TestAnonymize::testUnitAnonStatAlgWordChain()
{
    _subTestName = "testUnitAnonStatAlgWordChain";
    if(!testAnonStatWordChainEmpty()) {
        return false;
    }
    if(!testAnonStatWordChainOneWord()) {
        return false;
    }
    if(!testAnonStatWordChainDoubleWord()) {
        return false;
    }
    if(!testAnonStatWordChain3Word()) {
        return false;
    }
    return true;
}

//--------------------------

bool TestAnonymize::testUnitAnonStatAlgSinglePattern(const QString &aPattern, const QString &expected, AnonProducer::ESeqType expectedType)
{
    ScanPatternInfo info;
    const QString result = AnonStatAlgPattern::scanPattern(aPattern, info) ;
    if(expected != result ) {
        return error(QString("Single pattern for input '%1' expected '%2' found '%3' ")
                     .arg(aPattern).arg(expected).arg(result));
    }
    if(expectedType!=info.type) {
        return error(QString("Single pattern for input '%1' expected type '%2' found '%3' ")
                     .arg(aPattern).arg(expectedType).arg(info.type));
    }
    return true;
}

bool TestAnonymize::testUnitAnonStatAlgPatterns()
{
    _subTestName = "testUnitAnonStatAlgPatterns";
    if(!(true
         &&testUnitAnonStatAlgSinglePattern("", "", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern(" ", " ", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("c", "a", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("Z", "A", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("aBc", "aAa", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("8", "0", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("0123456789", "0000000000", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("asd&.$@,%#w6JJr/", "aaa&.$@,%#a0AAa/", AnonProducer::ASCII)
         &&testUnitAnonStatAlgSinglePattern("\u04000.\u0402\u0403\u04E1", "A0.AAa", AnonProducer::CYRILLIC)
         &&testUnitAnonStatAlgSinglePattern("\u00E8 abc", "a aaa", AnonProducer::WESTERN)
         &&testUnitAnonStatAlgSinglePattern("\u0106 abc", "A aaa", AnonProducer::EASTEUROPE)
         ) ) {
        return false;
    }
    return true;
}

bool TestAnonymize::checkPatternCount(AnonStatAlgPattern *anonStatAlgPattern, const int expected)
{
    if( expected != anonStatAlgPattern->count()) {
        return error(QString("Pattern count expected %1 found %2").arg(expected).arg(anonStatAlgPattern->count()));
    }
    return true;
}

bool TestAnonymize::testAnonStatAlgPattern()
{
    _subTestName = "testAnonStatAlgPattern";
    AnonStatAlgPattern anonStatAlgPattern(1, "abc");
    if(!checkPatternCount(&anonStatAlgPattern, 0)) {
        return false;
    }
    anonStatAlgPattern.newHit();
    if(!checkPatternCount(&anonStatAlgPattern, 1)) {
        return false;
    }
    anonStatAlgPattern.newHit();
    if(!checkPatternCount(&anonStatAlgPattern, 2)) {
        return false;
    }
    if( anonStatAlgPattern.pattern() != "abc") {
        return error(QString("Pattern expected '%1' found '%2'").arg("abc").arg(anonStatAlgPattern.pattern()));
    }
    return true ;
}

bool TestAnonymize::testUnitAnonStatAlgPatternsBase()
{
    AnonStatAlgPattern pattern(12, "abc");
    if(pattern.id()!=12) {
        return error(QString("Pattern create id '%1' found '%2'").arg(12).arg(pattern.id()));
    }
    if(pattern.pattern()!="abc") {
        return error(QString("Pattern create pattern '%1' found '%2'").arg("abc").arg(pattern.pattern()));
    }
    return true;
}


bool TestAnonymize::testAnonStatAlgPatternLoop()
{
    _subTestName = "testAnonStatAlgPatternLoop";
    ScanPatternInfo info;
    const QString decoded = AnonStatAlgPattern::scanPattern("bBbb b5", info);
    AnonStatAlgPattern pattern(12, decoded);
    AnonFixedProducer producer;
    const QString result = pattern.value(&producer, AnonProducer::ASCII);
    const QString decoded2 = AnonStatAlgPattern::scanPattern(result, info);
    if("aAaa a0" != decoded2) {
        return error(QString(" redecode pattern expected: '%1', found: '%2' ").arg("aAaa a0").arg(decoded2));
    }
    return true ;
}

bool TestAnonymize::testUnitAnonStatAlgPattern()
{
    _subTestName = "testUnitAnonStatAlgPattern";
    if(!testUnitAnonStatAlgPatternsBase()) {
        return false;
    }
    if(!testUnitAnonStatAlgPatterns()) {
        return false;
    }
    if(!testAnonStatAlgPattern()) {
        return false;
    }
    if(!testAnonStatAlgPatternLoop()) {
        return false;
    }
    return true;
}

//--------------------------

bool TestAnonymize::testAlgStatScanWordCase(QStringList input, const int expectedTotal, QList<QPair<int,int> > &expectedItems, QStringList expectedPatterns )
{
    AnonStatAlgWord anonStatAlgWord;
    AnonAlgStatContext context;
    AnonAlgFreqCount *freq = dynamic_cast<AnonAlgFreqCount*>(&anonStatAlgWord);
    foreach( QString singleInput, input) {
        anonStatAlgWord.scan(context, singleInput);
        if(context.isError()) {
            return error(QString("Context: For input '%1' error: '%2'  detail: '%3'").arg(singleInput).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    if(anonStatAlgWord.total()!=expectedTotal) {
        return error(QString("Expected total %1, was %2 for:%3").arg(expectedTotal).arg(anonStatAlgWord.total()).arg(dumpFreqTargets(freq)));
    }
    // test direct call id,count,pattern
    QHash<int, AnonAlgFreqTarget*> children = anonStatAlgWord.children();
    QPair<int,int> expectedItem;
    int expectedItemIndex = 0 ;
    foreach( expectedItem, expectedItems) {
        if(!children.contains(expectedItem.first)) {
            QString keys ;
            foreach( int aKey, children.keys()) {
                keys += "'";
                keys += QString::number(aKey);
                keys += "', ";
            }
            return error(QString("No item for key %1, keys %2").arg(expectedItem.first).arg(keys));
        }
        AnonAlgFreqTarget* item = children[expectedItem.first];
        if(item->count()!=expectedItem.second) {
            QString keys ;
            foreach( int key, children.keys()) {
                keys += ",";
                keys += QString::number(key);
            }
            return error(QString("Item for key %1, expected %2, found %3 for keys %4")
                         .arg(expectedItem.first).arg(expectedItem.second).arg(item->count())
                         .arg(keys));
        }
        AnonStatAlgPattern *anonStatAlgPattern = (AnonStatAlgPattern*)item;
        if(anonStatAlgPattern->pattern()!=expectedPatterns.at(expectedItemIndex)) {
            return error(QString("For index %1 expected pattern '%2' found '%3'")
                         .arg(expectedItemIndex).arg(expectedPatterns.at(expectedItemIndex)).arg(anonStatAlgPattern->pattern()));
        }
        expectedItemIndex++;
    }
    // test indirect pattern -> id
    int indexPattern = 0;
    foreach( const QString &expectedPattern, expectedPatterns) {
         AnonStatAlgPattern *anonStatAlgPattern = anonStatAlgWord.findPattern(expectedPattern);
         if(NULL == anonStatAlgPattern) {
             return error(QString("For reverse index %1 searched pattern '%2' but got NULL")
                          .arg(indexPattern).arg(expectedPattern));
         }
         if(indexPattern != anonStatAlgPattern->id()) {
             return error(QString("For reverse index %1 searched pattern '%2' but got id %3")
                          .arg(indexPattern).arg(expectedPattern).arg(anonStatAlgPattern->id()));
         }
         indexPattern ++;
    }

    return true;
}

class TestAnonStatAlgWord : public AnonStatAlgWord {
protected:
    inline virtual AnonStatAlgPattern *newPattern(const int /*id*/, const QString & /*value*/) {
        return NULL;
    }
};

bool TestAnonymize::testUnitAnonStatAlgWordErrors()
{
    _subTestName = "testUnitAnonStatAlgWordErrors";
    {
        AnonStatAlgWord anonStatAlgWord;
        AnonAlgStatContext context;
        const AnonStatAlgPattern *pattern = anonStatAlgWord.findCreatePattern(context, "aaa");
        if(NULL==pattern) {
            return error("EP 1");
        }
        if(context.isError()) {
            return error("E 2");
        }
    }
    {
        TestAnonStatAlgWord anonStatAlgWord;
        AnonAlgStatContext context;
        const AnonStatAlgPattern *pattern = anonStatAlgWord.findCreatePattern(context, "aaa");
        if(NULL!=pattern) {
            return error("EP 1");
        }
        if(!context.isError()) {
            return error("EP 1");
        }
    }
    return true;
}

bool TestAnonymize::testUnitAnonStatAlgWordHitPatterns1()
{
    _subTestName = "testUnitAnonStatAlgWordHitPatterns1";
    const int expectedKey[] = {0};
    const int expectedCount[] = {1};
    QList<QPair<int,int> > expectedPairs = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanWordCase(QStringList()<<"abc", 1, expectedPairs, QStringList()<<"aaa")) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testUnitAnonStatAlgWordHitPatternsNsame()
{
    _subTestName = "testUnitAnonStatAlgWordHitPatternsNsame";
    const int expectedKey[] = {0};
    const int expectedCount[] = {3};
    QList<QPair<int,int> > expectedPairs = itemsForArray(1, expectedKey, expectedCount);
    if(!testAlgStatScanWordCase(QStringList()<<"abc"<<"abc"<<"abc", 3, expectedPairs, QStringList()<<"aaa")) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testUnitAnonStatAlgWordHitPatternsN()
{
    _subTestName = "testUnitAnonStatAlgWordHitPatternsN";
    const int expectedKey[] = {0,1,2};
    const int expectedCount[] = {1,2,1};
    QList<QPair<int,int> > expectedPairs = itemsForArray(3, expectedKey, expectedCount);
    if(!testAlgStatScanWordCase(QStringList()<<"abc"<<"789"<<"012"<<"Nf/1", 4, expectedPairs, QStringList()<<"aaa"<<"000"<<"Aa/0")) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testUnitAnonStatAlgWord()
{
    _subTestName = "testUnitAnonStatAlgWord";
    if(!testUnitAnonStatAlgWordErrors()) {
        return false;
    }
    if(!testUnitAnonStatAlgWordHitPatterns1()) {
        return false;
    }
    if(!testUnitAnonStatAlgWordHitPatternsNsame()) {
        return false;
    }
    if(!testUnitAnonStatAlgWordHitPatternsN()) {
        return false;
    }
    return true ;
}

bool TestAnonymize::testStatValueForInputs( const QStringList &inputs, const QString expectedOutput)
{
    AnonStatAlgValue anonAlgValue;
    AnonAlgStatContext context;
    foreach( QString singleInput, inputs) {
        anonAlgValue.scan(context, singleInput);
        if(context.isError()) {
            return error(QString("Context: For input '%1' error: '%2'  detail: '%3'").arg(singleInput).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    AnonFlatSeqProducer seqProducer;
    const QString result = anonAlgValue.value(context, &seqProducer);

    if(context.isError()) {
        return error(QString("Value error: '%1'  detail: '%2'").arg(context.errorMessage()).arg(context.errorDetail()));
    }
    if( result != expectedOutput ) {
        return error(QString(" Expected '%1' got '%2' for %3").arg(expectedOutput).arg(result).arg(inputs.join(",")));
    }
    return true ;
}

bool TestAnonymize::testStatValueForInputsCoded( const QStringList &inputs, const QString expectedOutput, const AnonProducer::ESeqType type1, AnonProducer::ESeqType type2)
{
    AnonStatAlgValue anonAlgValue;
    AnonAlgStatContext context;
    foreach( QString singleInput, inputs) {
        anonAlgValue.scan(context, singleInput);
        if(context.isError()) {
            return error(QString("Context: For input '%1' error: '%2'  detail: '%3'").arg(singleInput).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    int times = 100;
    bool found1 = false;
    bool found2 = false;
    do {
        times --;
        AnonSeqProducer seqProducer;
        const QString result = anonAlgValue.value(context, &seqProducer);
        if(context.isError()) {
            return error(QString("Value error: '%1'  detail: '%2'").arg(context.errorMessage()).arg(context.errorDetail()));
        }
        if(result.length() != expectedOutput.length()) {
            return error(QString(" Expected len '%1' got %2 '%3' for %4").arg(expectedOutput.length()).arg(result.length()).arg(result).arg(inputs.join(",")));
        }
        FORINT(i,result.length()) {
            QChar chResult = result.at(i);
            QChar chExpected = expectedOutput.at(i);
            if(chExpected.isLetter()) {
                bool ok = true ;
                if(!chResult.isLetter()) {
                    ok = false ;
                } else {
                    if(chExpected.isLower()!=chResult.isLower()) {
                        ok = false;
                    }
                    const AnonProducer::ESeqType type = AnonCharUtils::type(chExpected);
                    if(type==type1) {
                        found1 = true;
                    }
                    if(type==type2) {
                        found2 = true;
                    }
                    if((type!=type1)&&(type!=type2)) {
                            ok = false;
                    }
                }
                if(!ok) {
                    return error(QString(" Expected '%1' got '%2' for %3, fault on '%4' @ %5")
                                 .arg(expectedOutput).arg(result).arg(inputs.join(","))
                                 .arg(i).arg(chResult));
                }
            }
        }
    } while(!(found1&&found2)||(times>=0));
    if(!(found1&&found2)) {
        return error(QString("Encoding: expected type '%2' and '%3'").arg(type1).arg(type2));
    }
    return true ;
}

static QString buildString( int start, int end)
{
    QString result ;
    for( int i = start ; i <= end ; i++ ) {
        result += QChar(i);
    }
    return result;
}

static QString buildStringArray( const int array [], const int len )
{
    QString result ;
    for( int i = 0 ; i < len ; i++ ) {
        result += QChar(array[i]);
    }
    return result;
}

static QString buildStringTimes( int theChar, int len)
{
    QString result ;
    for( int i = 0 ; i < len ; i++ ) {
        result += QChar(theChar);
    }
    return result;
}

// I want one blank value in N tries
bool TestAnonymize::testStatValueBlankValue()
{
    _subTestName = "testStatValueBlankValue";
    QStringList inputs ;
    AnonStatAlgValue anonAlgValue;
    AnonAlgStatContext context;
    inputs << "" << "" <<"" <<"" <<""<< "" << "a";
    foreach( QString singleInput, inputs) {
        anonAlgValue.scan(context, singleInput);
        if(context.isError()) {
            return error(QString("Context: For input '%1' error: '%2'  detail: '%3'").arg(singleInput).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    AnonSeqProducer seqProducer;
    bool found = false;
    FORINT(i, 100) {
        const QString result = anonAlgValue.value(context, &seqProducer);
        if(context.isError()) {
            return error(QString("Value error: '%1'  detail: '%2'").arg(context.errorMessage()).arg(context.errorDetail()));
        }
        if(result.isEmpty()) {
            found = true;
            break;
        }
    }
    if( !found ) {
        return error(QString(" Expected at least on blank value in '%1' tries.").arg(100));
    }
    return true ;
}

bool TestAnonymize::testStatErrorProducer()
{
    _subTestName = "testStatErrorProducer";
    QStringList inputs ;
    AnonStatAlgValue anonAlgValue;
    AnonAlgStatContext context;
    const QString Input = "aa a";
    anonAlgValue.scan(context, Input);
    if(context.isError()) {
        return error(QString("Context: for '%1' unexpected error: '%2'  detail: '%3'").arg(Input).arg(context.errorMessage()).arg(context.errorDetail()));
    }
    AnonSeqProducer seqProducer;
    seqProducer._errorCharSet = true ;
    anonAlgValue.value(context, &seqProducer);
    if(!context.isError()) {
        return error(QString("Context: For input '%1' expected error: '%2'  detail: '%3'").arg(Input).arg(context.errorMessage()).arg(context.errorDetail()));
    }
    return true ;
}


bool TestAnonymize::testUnitAlgStatValue()
{
    _testName = "testUnitAlgStatValue";
    if(!testStatValueForInputs(QStringList()<<"", "")) {
        return false;
    }
    if(!testStatValueForInputs(QStringList()<<"k", "a")) {
        return false;
    }
    if(!testStatValueForInputs(QStringList()<<"kggG", "abcD")) {
        return false;
    }
    if(!testStatValueForInputs(QStringList()<<"9", "0")) {
        return false;
    }
    if(!testStatValueForInputs(QStringList()<<"97639", "01234")) {
        return false;
    }
    if(!testStatValueForInputs(QStringList()<<"GHB", "ABC")) {
        return false;
    }
    if(!testStatValueForInputs(QStringList()<<"Bcd, 12/14/2032 UjUi,/sf234/.#$@", "Abc, 01/23/4567 DeFg,/hi890/.#$@")) {
        return false;
    }
    {
        QString input= buildString('a', 'z');
        QString expected= buildString('a', 'z');
        if(!testStatValueForInputsCoded(QStringList()<<input, expected, AnonProducer::ASCII, AnonProducer::ASCII)) {
            return false;
        }
    }
    {
        QString input= buildString('A', 'Z');
        QString expected= buildString('A', 'Z');
        if(!testStatValueForInputsCoded(QStringList()<<input, expected, AnonProducer::ASCII, AnonProducer::ASCII)) {
            return false;
        }
    }
    // LETTERE ACCENTATE INPUT
    {
        QString input= buildString('a', 'z');
        input += buildStringTimes(0x00E0, AnonCharUtils::WesternSize);
        QString expected= buildString('a', 'z');
        expected+= buildStringArray(AnonCharUtils::WesternLower, AnonCharUtils::WesternSize);
        if(!testStatValueForInputsCoded(QStringList()<<input, expected, AnonProducer::ASCII, AnonProducer::WESTERN)) {
            return false;
        }
    }
    {
        QString input= buildString('A', 'Z');
        input += buildStringTimes(0x00C0, AnonCharUtils::WesternSize);
        QString expected= buildString('A', 'Z');
        expected+= buildStringArray(AnonCharUtils::WesternUpper, AnonCharUtils::WesternSize);
        if(!testStatValueForInputsCoded(QStringList()<<input, expected, AnonProducer::ASCII, AnonProducer::WESTERN)) {
            return false;
        }
    }
    {
        QString input= buildString('A', 'Z');
        input += buildStringTimes(0x0106, AnonCharUtils::EastEuropeSize);
        QString expected= buildString('A', 'Z');
        expected+= buildStringArray(AnonCharUtils::EastEuropeUpper, AnonCharUtils::EastEuropeSize);
        if(!testStatValueForInputsCoded(QStringList()<<input, expected, AnonProducer::ASCII, AnonProducer::EASTEUROPE)) {
            return false;
        }
    }
    {
        QString input= buildString('a', 'z');
        input += buildStringTimes(0x0107, AnonCharUtils::EastEuropeSize);
        QString expected= buildString('a', 'z');
        expected+= buildStringArray(AnonCharUtils::EastEuropeLower, AnonCharUtils::EastEuropeSize);
        if(!testStatValueForInputsCoded(QStringList()<<input, expected, AnonProducer::ASCII, AnonProducer::EASTEUROPE)) {
            return false;
        }
    }
    {
        QString input= buildString('A', 'B');
        input += buildStringTimes(0x0410, 1);
        QString expected= "\u0410\u0411\u0412";
        if(!testStatValueForInputs(QStringList()<<input, expected)) {
            return false;
        }
    }
    {
        QString input= buildString('a', 'b');
        input += buildStringTimes(0x0430, 1);
        QString expected= "\u0430\u0431\u0432";
        if(!testStatValueForInputs(QStringList()<<input, expected)) {
            return false;
        }
    }
    if(!testStatValueBlankValue()) {
        return false;
    }
    if(!testStatErrorProducer()) {
        return false;
    }

    return true ;
}

static QString dumpFreqTargets(AnonAlgFreqCount *accumulator) {
    QString result;
    result += QString("\n**Total %1\n").arg(accumulator->children().size() );
    foreach(AnonAlgFreqTarget* target, accumulator->children().values()) {
        result += QString("id: %1 key:%5 Interval (%2 / %3) Total: %4\n")
                .arg(target->id())
                .arg(target->minInterval0To1Value()).arg(target->maxInterval0To1Value())
                .arg(target->count())
                .arg(target->key());
    }
    result += QString("keys:");
    foreach(const int key, accumulator->children().keys()) {
        result += QString("%1 ").arg(key);
    }
    result += "\n";
    return result;
}

bool TestAnonymize::checkBaseStatIntervals(AnonAlgFreqCount *accumulator, const QList<AnonAlgFreqTarget*> &expectedTargets )
{
    accumulator->prepare();
    if(accumulator->children().values().size()!=expectedTargets.size()) {
        return error(QString("accumulator size %1, expected: %2, dump: %3")
                     .arg(accumulator->children().values().size())
                     .arg(expectedTargets.size()).arg(dumpFreqTargets(accumulator)));
    }

    foreach(AnonAlgFreqTarget* expectedTarget, expectedTargets) {
        bool found = false;
        foreach(AnonAlgFreqTarget* foundTarget, accumulator->children().values()) {
            if(expectedTarget->id() == foundTarget->id()) {
                found = true ;
                if(fabs(expectedTarget->minInterval0To1Value()-foundTarget->minInterval0To1Value())>0.1) {
                    return  error(QString("target for id %1 expected min %2 found :%3 %4")
                                  .arg(expectedTarget->id())
                                  .arg(expectedTarget->minInterval0To1Value())
                                  .arg(foundTarget->minInterval0To1Value())
                                  .arg(dumpFreqTargets(accumulator)));
                }
                if(fabs(expectedTarget->maxInterval0To1Value()-foundTarget->maxInterval0To1Value())>0.1) {
                    return  error(QString("target for id %1 expected max %2 found :%3, %4")
                                  .arg(expectedTarget->id())
                                  .arg(expectedTarget->maxInterval0To1Value())
                                  .arg(foundTarget->maxInterval0To1Value())
                                  .arg(dumpFreqTargets(accumulator)));
                }
                break;
            }
        }
        if(!found) {
            return  error(QString("target not found for is:%1, %2").arg(expectedTarget->id()).arg(dumpFreqTargets(accumulator)));
        }
    }
    return true;
}

bool TestAnonymize::checkStatIntervals(const QStringList &inputs, const QList<QPair<double, double> > &expected )
{
    AnonStatAlgValue chain;
    AnonAlgStatContext context;
    foreach(const QString &input, inputs ) {
        chain.scan(context, input);
        if(context.isError()) {
            return error(QString("For input '%1' error: '%2'  detail: '%3'").arg(input).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    //-----
    QList<AnonAlgFreqTarget*> expectedTargets;
    int id = 1;
    QPair<double, double> pair;
    foreach(pair, expected) {
        AnonAlgFreqTarget* newTarget = new AnonAlgFreqTarget();
        newTarget->setId(id);
        id++;
        newTarget->_minInterval0To1Value = pair.first;
        newTarget->_maxInterval0To1Value = pair.second;
        expectedTargets.append(newTarget);
    }
    bool result = checkBaseStatIntervals(&chain, expectedTargets);
    EMPTYPTRLIST(expectedTargets, AnonAlgFreqTarget);
    if(!result) {
        return false;
    }
    return true;
}

bool TestAnonymize::testAnonStatRandom()
{
    AnonStatAlgValue chain;
    for( int i = 0 ; i < 10 ; i ++ ) {
        double value = chain.chooseValue();
        if((value <0)||(value>1)) {
            return error(QString("chooseValue was %1").arg(value));
        }
    }
    return true;
}

bool TestAnonymize::testAnonStatContext()
{
    _testName = "testAnonStatContext";

    {
        AnonFixedProducer producer;
        AnonAlgStatContext context(&producer);
        if(context.producer()!= &producer) {
            return error("e1");
        }
        if(context.isError()) {
            return error("e2");
        }
    }
    {
        AnonAlgStatContext context;
        if(context.producer()!= NULL) {
            return error("e3");
        }
        if(context.isError()) {
            return error("e4");
        }
    }
    {
        AnonAlgStatContext context;
        if(context.producer()!= NULL) {
            return error("e5");
        }
        if(context.setError("msg", "detail")) {
            return error("e6");
        }
        if(!context.isError()) {
            return error("e7");
        }
        if(context.errorMessage()!="msg") {
            return error("e8");
        }
        if(context.errorDetail()!="detail") {
            return error("e9");
        }
    }
    return true;
}

bool TestAnonymize::testAnonStatMean()
{
    _testName = "testAnonStatMean";

    if(!testAnonStatRandom()) {
        return false;
    }
    {
        QList<QPair<double, double> > expected ;
        QPair<double, double> d1(0, 1);
        expected.append(d1);
        if(!checkStatIntervals(QStringList()<<"1", expected)) {
            return false;
        }
    }
    {
        QList<QPair<double, double> > expected ;
        QPair<double, double> d1(0, 1);
        expected.append(d1);
        if(!checkStatIntervals(QStringList()<<"1"<<"1", expected)) {
            return false;
        }
    }
    {
        QList<QPair<double, double> > expected ;
        QPair<double, double> d1(0, 0.75);
        QPair<double, double> d2(0.75, 1);
        expected.append(d1);
        expected.append(d2);
        if(!checkStatIntervals(QStringList()<<"1"<<"1"<<"2 2"<<"1", expected)) {
            return false;
        }
    }
    {
        QList<QPair<double, double> > expected ;
        QPair<double, double> d1(0.75, 1);
        QPair<double, double> d2(0, 0.5);
        QPair<double, double> d3(0.5, 0.75);
        expected.append(d1);
        expected.append(d2);
        expected.append(d3);
        if(!checkStatIntervals(QStringList()<<"3 3 3"<<"1"<<"1"<<"2 2", expected)) {
            return false;
        }
    }
    return true;
}

class TestStatAlgStatRandomProvider : public AlgStatRandomProvider
{
public:
    double _value;
    TestStatAlgStatRandomProvider(){}
    virtual ~TestStatAlgStatRandomProvider(){}

    // Between 0 and 1
    double chooseValue() { return _value;}
};

bool TestAnonymize::checkStatOneRandomChoice(AnonAlgFreqCount *accumulator, const double randomChoice, const int expectedId)
{
    TestStatAlgStatRandomProvider testStatAlgStatRandomProvider;
    accumulator->setRandomProvider(&testStatAlgStatRandomProvider);
    testStatAlgStatRandomProvider._value = randomChoice;
    AnonAlgFreqTarget* target = accumulator->randomChoice();
    if(target->id()!=expectedId) {
        return error(QString("Random choice for: %1 expected: %2, found: %3 for:%4")
                     .arg(randomChoice).arg(expectedId).arg(target->id())
                     .arg(dumpFreqTargets(accumulator)));
    }
    return true ;
}

bool TestAnonymize::checkStatRandomChoice(const QStringList &inputs, const double randomChoice, const int expectedId)
{
    AnonStatAlgValue chain;
    AnonAlgStatContext context;
    foreach(const QString &input, inputs ) {
        chain.scan(context, input);
        if(context.isError()) {
            return error(QString("For input '%1' error: '%2'  detail: '%3'").arg(input).arg(context.errorMessage()).arg(context.errorDetail()));
        }
    }
    //-----
    if(!checkStatOneRandomChoice(&chain, randomChoice, expectedId)) {
        return false;
    }
    return true;
}

bool TestAnonymize::testAnonStatRandomChoice()
{
    _testName = "testAnonStatRandomChoice";
    QStringList inputs;
    inputs << "1" << "1"<< "2 2"<< "3 3 3";
    if(     checkStatRandomChoice(inputs, 0, 1)
        &&  checkStatRandomChoice(inputs, 0.15, 1)
        &&  checkStatRandomChoice(inputs, 0.25, 1)
        &&  checkStatRandomChoice(inputs, 0.5, 1)
        &&  checkStatRandomChoice(inputs, 0.52, 2)
        &&  checkStatRandomChoice(inputs, 0.70, 2)
        &&  checkStatRandomChoice(inputs, 0.749, 2)
        &&  checkStatRandomChoice(inputs, 0.752, 3)
        &&  checkStatRandomChoice(inputs, 0.90, 3)
        &&  checkStatRandomChoice(inputs, 1, 3) ) {
        return true ;
    }
    return false;
}

bool TestAnonymize::testAnonStatPrepare()
{
    _testName = "testAnonStatPrepare";
    AnonStatAlgValue chain;
    if(chain.isReady()) {
        return error("Ready at start");
    }
    QStringList inputs ;
    inputs << "1" << "2" << "3" ;
    AnonAlgStatContext context;
    foreach(const QString &input, inputs ) {
        chain.scan(context, input);
        if(context.isError()) {
            return error(QString("For input '%1' error: '%2'  detail: '%3'").arg(input).arg(context.errorMessage()).arg(context.errorDetail()));
        }
        if(chain.isReady()) {
            return error("Ready first");
        }
        chain.prepare();
        if(!chain.isReady()) {
            return error("Not ready");
        }
    }
    if(!chain.isReady()) {
        return error("Not ready at end");
    }
    return true;
}

class CheckCodeContext {
public:
    AnonProducer::ESeqType codes;
    CheckCodeContext()
    {
        codes = AnonProducer::ASCII;
    }
};

static void checkCharCodes(CheckCodeContext &context, const QString &input)
{
    foreach(const QChar ch, input) {
        if(AnonCharUtils::isCyrillic(ch)) {
            context.codes = AnonProducer::CYRILLIC ;
        } else if(AnonCharUtils::isWestern(ch)) {
            context.codes = AnonProducer::WESTERN ;
        } else if(AnonCharUtils::isEastEurope(ch)) {
            context.codes = AnonProducer::EASTEUROPE;
        }
    }
}

static QPair<bool,QString> validateAttribute(const QString &name, const QString &value, const QString &path)
{
    QStringList parts= name.split("_");
    if(parts.length()<3) {
        return QPair<bool,QString>(true,"");
    }
    bool isOk=false;
    int minLen = parts.at(1).toInt(&isOk);
    if(!isOk) {
        return QPair<bool,QString>(false,QString("Bad minlen num path:%1/@%2").arg(path).arg(name));
    }
    int maxLen = parts.at(2).toInt(&isOk);
    if(!isOk) {
        return QPair<bool,QString>(false,QString("Bad minlen num path:%1/@%2").arg(path).arg(name));
    }
    QString charsType ;
    if(parts.length() > 3) {
        charsType= parts.at(3);
    }
    if(value.length()<minLen) {
        return QPair<bool,QString>(false,QString("Attr invalid min len path:%1/@%2 %3 '%4'").arg(path).arg(name).arg(value.length()).arg(value));
    }
    if(value.length()>maxLen) {
        return QPair<bool,QString>(false,QString("Attr invalid max len path:%1/@%2 %3 '%4'").arg(path).arg(name).arg(value.length()).arg(value));
    }
    // Cyrillic only
    if(charsType=="C") {
        foreach( const QChar ch, value ) {
            if(!AnonCharUtils::isCyrillic(ch)) {
                return QPair<bool,QString>(false,QString("Attr invalid encoding path:%1/@%2 '%3'").arg(path).arg(name).arg(value));
            }
        }
    }
    return QPair<bool,QString>(true,"") ;
}

static QPair<bool,QString> compareAttributesDiff(CheckCodeContext &context, QDomElement element1, QDomElement element2, const QString path)
{
    QDomNamedNodeMap attrList1 = element1.attributes();
    QDomNamedNodeMap attrList2 = element2.attributes();
    int attrMax1 = attrList1.count();
    int attrMax2 = attrList2.count();
    if(attrMax1 != attrMax2) {
        return QPair<bool,QString>(false,QString("Attributes count 1:%1 2:%2 for %3/%4").arg(attrMax1).arg(attrMax2).arg(path).arg(element1.tagName())) ;
    }
    QMap<QString, QString> am1, am2;
    for(int a = 0 ; a < attrMax1 ; a ++) {
        QDomAttr attr1 = attrList1.item(a).toAttr();
        QDomAttr attr2 = attrList2.item(a).toAttr();
        am1[attr1.name()] = attr1.value();
        am2[attr2.name()] = attr2.value();
    }
    foreach(QString key, am1.keys()) {
        if(!am2.contains(key)) {
            return QPair<bool,QString>(false,QString("2 does not contain attribute '%1' at %2").arg(key).arg(path)) ;
        }
        if(am1[key] == am2[key]) {
            if(am1[key].length()>AnonCodeAlg::Threshold) {
                return QPair<bool,QString>(false,QString("attributes value is the same element: path '%1/@%2', val:'%3'")
                                       .arg(path).arg(key).arg(am1[key])) ;
            }
        }
        QPair<bool,QString> result = validateAttribute(key, am1[key], path);
        if(!result.first) {
            return result;
        }
        checkCharCodes(context, am1[key]);
    }
    return QPair<bool,QString>(true,"");
}

static QPair<bool,QString> comparaFigliStrutt(CheckCodeContext &context, QDomNode &node1, QDomNode &node2, const QString sourcePath)
{
    QString path = sourcePath ;
    int nodi1Count = node1.childNodes().count();
    int nodi2Count = node2.childNodes().count();

    if(nodi1Count != nodi2Count) {
        return QPair<bool,QString>(false, QString("nodes count a:%1, b:%2 at %3").arg(nodi1Count).arg(nodi2Count).arg(path) );
    }
    QDomNodeList childNodes1 = node1.childNodes();
    QDomNodeList childNodes2 = node2.childNodes();

    for(int i = 0 ; i < nodi1Count ; i ++) {
        QDomNode childNode1 = childNodes1.item(i) ;
        QDomNode childNode2 = childNodes2.item(i) ;

        if(childNode1.isElement()) {
            if(!childNode2.isElement()) {
                return QPair<bool,QString>(false, QString("1 is element, 2 no: '%1', '%2' path %3").arg(childNode1.nodeName()).arg(childNode2.nodeName()).arg(path));
            }
            QDomElement element1 = childNode1.toElement();
            QDomElement element2 = childNode2.toElement();
            if(element1.tagName() != element2.tagName()) {
                return QPair<bool,QString>(false, QString("elements: '%1' ' %2' path %3").arg(element1.tagName()).arg(element2.tagName()).arg(path));
            }
            QPair<bool,QString> result = compareAttributesDiff(context, element1, element2, path+"/"+element1.tagName());
            if(!result.first) {
                return result;
            }
            result = comparaFigliStrutt(context, childNode1, childNode2, path+"/"+element1.tagName());
            if(!result.first) {
                return result;
            }
        } else if(childNode1.isCDATASection()) {
            if(!childNode2.isCDATASection()) {
                return QPair<bool,QString>(false, QString("1 is CData, 2 no: '%1', '%2' path %3").arg(childNode1.nodeName()).arg(childNode2.nodeName()).arg(path));
            }
            QDomCDATASection text1 = childNode1.toCDATASection();
            QDomCDATASection text2 = childNode2.toCDATASection();
            QString s1 = text1.data() ;
            QString s2 = text2.data() ;
            s1.replace("\r\n", "\n");
            s2.replace("\r\n", "\n");
            if(s1 == s2) {
                if(s1.length()>AnonCodeAlg::Threshold) {
                    return QPair<bool,QString>(false, QString("CData equals: '%1', '%2' path %3").arg(text1.data()).arg(text2.data()).arg(path));
                }
            }
            checkCharCodes(context, s1);
        } else if(childNode1.isText()) {
            if(!childNode2.isText()) {
                return QPair<bool,QString>(false, QString("1 is text, 2 no: '%1', '%2' path %3").arg(childNode1.nodeName()).arg(childNode2.nodeName()).arg(path));
            }
            QDomText text1 = childNode1.toText();
            QDomText text2 = childNode2.toText();
            QString s1 = text1.data() ;
            QString s2 = text2.data() ;
            s1.replace("\r\n", "\n");
            s2.replace("\r\n", "\n");
            s1.replace("\r", "\n");
            s2.replace("\r", "\n");
            s1 = s1.trimmed();
            s2 = s2.trimmed();
            if(s1 == s2) {
                if(s1.length()>AnonCodeAlg::Threshold) {
                    return QPair<bool,QString>(false, QString("text equals: on element '%3' %1', '%2' path %4").arg(text1.data()).arg(text2.data()).arg(node1.nodeName()).arg(path));
                }
            }
            checkCharCodes(context, s1);
        } else if(childNode1.isProcessingInstruction()) {
            if(!childNode2.isProcessingInstruction()) {
                return QPair<bool,QString>(false, QString("1 is isProcessingInstruction, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomProcessingInstruction procInstrNode1 = childNode1.toProcessingInstruction();
            QDomProcessingInstruction procInstrNode2 = childNode2.toProcessingInstruction();
            if(procInstrNode1.data() != procInstrNode2.data()) {
                return QPair<bool,QString>(false, QString("PI differ: '%1', '%2'").arg(procInstrNode1.data()).arg(procInstrNode2.data()));
            }
            if(procInstrNode1.target() != procInstrNode2.target()) {
                return QPair<bool,QString>(false, QString("PI target differ: '%1', '%2'").arg(procInstrNode1.target()).arg(procInstrNode2.target()));
            }
        } else if(childNode1.isComment()) {
            if(!childNode2.isComment()) {
                return QPair<bool,QString>(false, QString("1 is comment, 2 no: '%1', '%2'").arg(childNode1.nodeName()).arg(childNode2.nodeName()));
            }
            QDomComment commentNode1 = childNode1.toComment();
            QDomComment commentNode2 = childNode2.toComment();
            QString s1 = commentNode1.data() ;
            QString s2 = commentNode2.data() ;
            s1.replace("\r\n", "\n");
            s2.replace("\r\n", "\n");

            if(s1 != s2) {
                return QPair<bool,QString>(false, QString("comments differ: '%1', '%2'").arg(commentNode1.data()).arg(commentNode2.data()));
            }
        } else if(childNode1.isEntity()) {
            //TODO
        } else if(childNode1.isEntityReference()) {
            //TODO
        } else if(childNode1.isDocumentType()) {
            QDomDocumentType dtype1 = childNode1.toDocumentType();
            QDomDocumentType dtype2 = childNode2.toDocumentType();
            if( dtype1.name() != dtype2.name() ) {
                return QPair<bool,QString>(false, "elem14");
            }
            if( dtype1.systemId() != dtype2.systemId() ) {
                return QPair<bool,QString>(false, "elem15");
            }
            if( dtype1.publicId() != dtype2.publicId() ) {
                return QPair<bool,QString>(false, "elem16");
            }
        } //if
    }//for
    return QPair<bool,QString>(true, "");
}//

bool TestAnonymize::cfrStatMem(QBuffer *resultData, const QString &step, const QString &fileResult, const bool isCheckCodes, const AnonProducer::ESeqType expectedType)
{
    QDomDocument documentExpected;
    QDomDocument documentCandidate;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, documentExpected)) {
        return error(QString("step: %1, load file result %2").arg(step).arg(fileResult));
    }
    if(!compare.loadFileIntoDocument(resultData, documentCandidate)) {
        return error(QString("step %1 load modified data").arg(step));
    }
    CheckCodeContext codeContext;
    QPair<bool,QString> result = comparaFigliStrutt(codeContext, documentCandidate, documentExpected, "");
    if( !result.first ) {
        return error(QString("Step: %1 comparing file with doc: %2").arg(step).arg(result.second));
    }
    if(isCheckCodes) {
        if(expectedType != codeContext.codes) {
            return error(QString("Step: %1 expected char codes %2, found %3").arg(step).arg(expectedType).arg(codeContext.codes));
        }
    }
    return true ;
}

bool TestAnonymize::testBatchBaseStatSkeleton(const QString &code, const QString &sourceFilePath, AnonContext *context, const bool checkCodes, const AnonProducer::ESeqType type)
{
    //----- first part: editor
    AnonOperationBatch tester;
    bool isError = false;
    QByteArray resultData;
    QBuffer buffer(&resultData);
    QFile sourceFile(sourceFilePath);
    if(!buffer.open(QIODevice::ReadWrite)) {
        return error(QString("Batch: unable to open out buffer %1").arg(code));
    }
    if( sourceFile.open(QFile::ReadOnly) ) {
        const AnonOperationResult *res ;
        __qxmledit_test_nextLetterCond = true ;
        res = tester.scanAndExecute(&sourceFile, &buffer, sourceFilePath, context);
        __qxmledit_test_nextLetterCond = false;
        if( NULL != res ) {
            if( res->isError() ) {
                error(QString("Batch: code %3 error writing was: '%1' '%2'").arg(res->code()).arg(res->message()).arg(code));
            }
        } else {
            error(QString("Batch: code %1 returned null result").arg(code));
        }
        buffer.close();
        sourceFile.close();
    } else {
        return error(QString("Batch: code %2 unable to open file: '%1'").arg(sourceFilePath).arg(code));
    }
    if( isError ) {
        return false;
    }
    Utils::TEST_ME("a");
    writeToFile("/tmp/1.xml", QString(buffer.data()));
    if(!cfrStatMem(&buffer, code, sourceFilePath, checkCodes, type)) {
        return false;
    }
    return true;
}

// to have a deterministic choice
// this class will be useful in a future
/*class AlgStatFixedRandomProvider : public AlgStatRandomProvider {

public:
    AlgStatFixedRandomProvider() {
        //
    }
    virtual ~AlgStatFixedRandomProvider() {
        //
    }

    // Between 0 and 1
    double chooseValue() {
        return 1;
    }

};*/

bool TestAnonymize::testBatchBaseStatAFile(const QString &code, const QString &dataFile)
{
    return testBatchBaseStatAFile(code, dataFile, false, AnonProducer::ASCII);
}

bool TestAnonymize::testBatchBaseStatAFile(const QString &code, const QString &dataFile, const bool checkCodes, const AnonProducer::ESeqType type)
{
    QMap<QString, QVariant> emptyConfig;
    Config::setBackend(&emptyConfig);
    AnonContext context(NULL, "");
    AnonSeqProducer *producer = new AnonSeqProducer();
    AnonCodeAlg *anonCodeAlg = new AnonCodeAlg(false, producer);
    context.setAlg(anonCodeAlg);
    bool result = false ;
    result = testBatchBaseStatSkeleton(code, dataFile, &context, checkCodes, type);
    Config::setBackend(NULL);
    if(result) {
        return true ;
    }
    return false;
}


bool TestAnonymize::testBatchBaseStat()
{
    _testName = "testBatchBaseStat/qualified";
    if(!testBatchBaseStatAFile("1", FILE_STAT_1T)) {
        return false;
    }
    if(!testBatchBaseStatAFile("2", FILE_STAT_TONLY)) {
        return false;
    }
    if(!testBatchBaseStatAFile("3", FILE_STAT_T1)) {
        return false;
    }
    if(!testBatchBaseStatAFile("4", FILE_STAT_T2)) {
        return false;
    }
    if(!testBatchBaseStatAFile("6", FILE_STAT_TWESTERN, true, AnonProducer::WESTERN)) {
        return false;
    }
    if(!testBatchBaseStatAFile("6a", FILE_STAT_TWESTERNUPPER, true, AnonProducer::WESTERN)) {
        return false;
    }
    if(!testBatchBaseStatAFile("7", FILE_STAT_TEAST, true, AnonProducer::EASTEUROPE)) {
        return false;
    }
    if(!testBatchBaseStatAFile("7a", FILE_STAT_TEASTUPPER, true, AnonProducer::EASTEUROPE)) {
        return false;
    }
    if(!testBatchBaseStatAFile("8", FILE_STAT_TCYRILLIC, true, AnonProducer::CYRILLIC)) {
        return false;
    }
    if(!testBatchBaseStatAFile("8a", FILE_STAT_TCYRILLICUPPER, true, AnonProducer::CYRILLIC)) {
        return false;
    }
    if(!testBatchBaseStatAFile("9", FILE_STAT_TMIXED, true, AnonProducer::WESTERN)) {
        return false;
    }
    if(!testBatchBaseStatAFile("9a", FILE_STAT_TMIXED1, true, AnonProducer::EASTEUROPE)) {
        return false;
    }
    return true;
}

bool TestAnonymize::testAnonStatChars()
{
    _testName = "testAnonStatChars";
    for( int i = 0 ; i <= 0x007F; i++ ) {
        if(AnonCharUtils::isWestern(i)) {
            return error(QString("isWestern for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(AnonCharUtils::isEastEurope(i)) {
            return error(QString("isEastEurope for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(AnonCharUtils::isCyrillic(i)) {
            return error(QString("isCyrillic for:%1 %2").arg(QChar(i)).arg(i));
        }
    }
    for( int i = 0x80 ; i <= 0x00FF; i++ ) {
        if(!AnonCharUtils::isWestern(i)) {
            return error(QString("isWestern for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(AnonCharUtils::isEastEurope(i)) {
            return error(QString("isEastEurope for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(AnonCharUtils::isCyrillic(i)) {
            return error(QString("isCyrillic for:%1 %2").arg(QChar(i)).arg(i));
        }
    }
    //--
    for( int i = 0x100 ; i <= 0x017F; i++ ) {
        if(AnonCharUtils::isWestern(i)) {
            return error(QString("isWestern for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(!AnonCharUtils::isEastEurope(i)) {
            return error(QString("isEastEurope for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(AnonCharUtils::isCyrillic(i)) {
            return error(QString("isCyrillic for:%1 %2").arg(QChar(i)).arg(i));
        }
    }
    //--
    for( int i = 0x400 ; i <= 0x04FF; i++ ) {
        if(AnonCharUtils::isWestern(i)) {
            return error(QString("isWestern for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(AnonCharUtils::isEastEurope(i)) {
            return error(QString("isEastEurope for:%1 %2").arg(QChar(i)).arg(i));
        }
        if(!AnonCharUtils::isCyrillic(i)) {
            return error(QString("isCyrillic for:%1 %2").arg(QChar(i)).arg(i));
        }
    }
    return true ;
}

bool TestAnonymize::testAnonUtilsOne(const int code, const AnonProducer::ESeqType expected, const bool expectedRetValue)
{
    AnonProducer::ESeqType type = AnonProducer::ASCII;
    const bool resultBool = XmlAnonUtils::setTypeIfNotSet(type, expected);
    if(resultBool!=expectedRetValue) {
        return error( QString("Result: %3 expected %1 found %2").arg(expectedRetValue).arg(resultBool).arg(code));
    }
    if(type!=expected) {
        return error( QString("Type: %3 expected %1 found %2").arg(expected).arg(type).arg(code));
    }
    XmlAnonUtils::setTypeIfNotSet(type, AnonProducer::ASCII);
    if(type!=expected) {
        return error( QString("Type: %3 expected PASS 2 %1 found %2").arg(expected).arg(type).arg(code));
    }
    return true;
}

bool TestAnonymize::testAnonUtils()
{
    _testName = "testAnonUtils";
    if(!testAnonUtilsOne(1, AnonProducer::ASCII, false)) {
        return false;
    }
    if(!testAnonUtilsOne(2, AnonProducer::WESTERN, true)) {
        return false;
    }
    if(!testAnonUtilsOne(3, AnonProducer::EASTEUROPE, true)) {
        return false;
    }
    if(!testAnonUtilsOne(4, AnonProducer::CYRILLIC, true)) {
        return false;
    }
    return true ;
}

bool TestAnonymize::testAnonStatFreqTarget()
{
    _testName = "testAnonStatFreqTarget";
    {
        AnonStatAlgWordChain chain;
        if(0!=chain.id()) {
            return error(QString("Id 0 expected %1 found %2")
                    .arg(0).arg(chain.id()));
        }
        chain.setId(101);
        if(101!=chain.id()) {
            return error(QString("Id 1 expected %1 found %2")
                    .arg(101).arg(chain.id()));
        }
    }
    return true;
}

bool TestAnonymize::testAnonStatAlgVaria()
{
    _testName = "testAnonStatAlgVaria";
    {
        AnonAllAlg alg(true, NULL);
        if(alg.scanned()) {
            return error("AnonAllAlg 1");
        }
        if(alg.isUseLegacy()) {
            return error("AnonAllAlg 2");
        }
        AnonAlgStatContext context;
        alg.scan(context, "aa", "") ;
        if(!alg.scanned()) {
            return error("AnonAllAlg 3");
        }
    }
    {
        AnonStatAlgWord word;
        AnonAlgStatContext context;
        QString value = word.value(context, NULL, AnonProducer::ASCII);
        if(!value.isEmpty()) {
            return error("AnonStatAlgWord 1");
        }
        if(!context.isError()) {
            return error("AnonStatAlgWord 2");
        }
    }
    return true;
}
