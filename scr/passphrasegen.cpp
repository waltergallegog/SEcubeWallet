#include "passphrasegen.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QDebug>
#include <QMessageBox>

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
        )
{

    QString genPass;        //the generated pass to return
    QList <int> random;     //list of random numbers to extract random words
    QList<int> whichDict;   //in which dictiona is each random number
    QList<int> whichLine;   //in which line inside the dictiona
    QFile dictionary;
    QTextStream in;         //to read line by line
    int currentLine = 0;
    QString line;           //temp to read the line


    // generate numWords random numbers, bounded to totalLen
    for(int i=0; i<numWords; i++){
        random<<QRandomGenerator::global()->bounded(totalLen)+1;//+1 to exclude 0 and include totalLen
    }
    std::sort(random.begin(), random.end());//sort the numbers. As the file have to be read sequentially, only read each file once by extracting the words in order
    qDebug() << "TotalLen "<<totalLen;
    qDebug() << "Random: " << random;
    qDebug() << "DictsLen: " << dictsLen;


    //first prepare the list whichDict and whichLine to make the reading loops easier.
    dicts.prepend("start");
    dictsLen.prepend("0");
    if(!ppgenLowerEnab){//regular case, no lower part enabled
        for(int r=0; r<random.length(); r++){//loop the random list
            for (int d=1; d<dictsLen.length(); d++){//loop the len list. The list is commulative
                if (random.at(r)<=dictsLen.at(d).toInt()){//if the random is in this dict
                    whichDict<<d;//save index of dict
                    whichLine<<random.at(r)-dictsLen.at(d-1).toInt();//save line inside the dict
                    break;
                }
            }
        }
    }else{//only use lower part of dict
        for(int r=0; r<random.length(); r++){//loop the random list
            for (int d=1; d<dictsLen.length(); d++){//loop the len list. The list is commulative
                if (random.at(r) <= qRound(0.01*ppgenLower*dictsLen.at(d).toInt())){//the random numbers are scaled to %low, but dict len list is not
                    whichDict<<d;//save index of dict
                    whichLine<<random.at(r)-dictsLen.at(d-1).toInt()+qRound(0.01*(100-ppgenLower)*dictsLen.at(d).toInt());//save line inside the dict
                    break;
                }
            }
        }
    }
    qDebug() << "whichDir "<< whichDict;
    qDebug() << "wichLine "<< whichLine;

    whichDict.prepend(whichDict.first());//duplicate first dic index because in next loop we use index-1, and cannot have index neg
    whichLine.prepend(0);//wont be used, just to keep indexes in sync
    random.prepend(0);//

    dictionary.setFileName(path+"/"+dicts.at(whichDict.at(1)));//open first dictionary outside loop
    if (!dictionary.open(QIODevice::ReadOnly| QIODevice::Text)){
        QMessageBox::information(
                    NULL,
                    "SEcubeWallet",
                    "Passphrase generator: Cant open one of the dictionaries, please try selecting again");
        return "";//in case of error opening, return empty
    }

    in.setDevice(&dictionary);// the textstream
    currentLine = 0;
    for (int d=1; d<whichDict.length();d++){//loop whichDict
        if(random.at(d)==random.at(d-1))//if for some weird reason two random num are the same, skip the second.
            continue;
        if (whichDict.at(d)!=whichDict.at(d-1)){//change of dictionary, close and open
            dictionary.close();
            dictionary.setFileName(path+"/"+dicts.at(whichDict.at(d)));//open first dictionary
            if (!dictionary.open(QIODevice::ReadOnly| QIODevice::Text)){
                QMessageBox::information(
                            NULL,
                            "SEcubeWallet",
                            "Passphrase generator: Cant open one of the dictionaries, please try selecting again");
                return "";
            }
            in.setDevice(&dictionary);
            currentLine = 0;//new dictionary, restart line
        }
        while (!in.atEnd()){
            line = in.readLine().split(QRegExp("\\s+"), QString::SkipEmptyParts).at(0);//read line and only take the first word. Dictionaries usually have info about freq of the word in the same line
            if(!ppgenMinLenEnab)//if no minumum len consideration, all lines are counted
                currentLine++;
            else if (line.length()>=ppgenMinLen)//if min len, then check len is enough to be counted
                currentLine++;
            if (currentLine==whichLine.at(d)){//found the random line
                if(capFirst)//if uppercase first letter
                    line[0] = line.at(0).toUpper();

                if(QRandomGenerator::global()->bounded(2))//just to shuffle the final pass a bit
                    genPass.append(line);//build password to return
                else
                    genPass.prepend(line);//build password to return
                qDebug() << line;
                break;//out of the line by line loop
            }
        }
    }
    return genPass;

}
