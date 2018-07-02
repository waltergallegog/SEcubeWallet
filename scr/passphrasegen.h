#ifndef PASSPHRASEGEN_H
#define PASSPHRASEGEN_H

#include <QString>

QString PassPhraseGen(
        QString path,           //path to dicts
        QStringList dicts,      //list of dicts
        QStringList dictsLen,   //commulative list of lengths
        int totalLen,           //total length (considering minLeng of words and %)
        int numWords,           //num of words in the password
        bool ppgenMinLenEnab,   //min length of words enabled
        int ppgenMinLen,        //value of min len
        bool capFirst,          //upper case first letter of each word
        bool ppgenLowerEnab,    //lower % part of dictionary enabled
        int ppgenLower          //%value
        );

#endif // PASSPHRASEGEN_H
