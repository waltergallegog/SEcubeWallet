#ifndef ADDENTRY_H
#define ADDENTRY_H

#include <QDialog>
#include <QString>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QStandardItemModel>
#include <QLibrary>


//#include "zxcvbn.h"

//password generators
#define PWGEN 0
#define PWQGEN 1
#define CUSTOM 2
#define NONE 3

/* Enum for the types of match returned in the Info arg to ZxcvbnMatch */
typedef enum
{
    NON_MATCH,          /* 0 */
    BRUTE_MATCH,        /* 1 */
    DICTIONARY_MATCH,   /* 2 */
    DICT_LEET_MATCH,    /* 3 */
    USER_MATCH,         /* 4 */
    USER_LEET_MATCH,    /* 5 */
    REPEATS_MATCH,      /* 6 */
    SEQUENCE_MATCH,     /* 7 */
    SPATIAL_MATCH,      /* 8 */
    DATE_MATCH,         /* 9 */
    YEAR_MATCH,         /* 10 */
    MULTIPLE_MATCH = 32 /* Added to above to indicate matching part has been repeated */
} ZxcTypeMatch_t;

/* Linked list of information returned in the Info arg to ZxcvbnMatch */
struct ZxcMatch
{
    int             Begin;   /* Char position of begining of match */
    int             Length;  /* Number of chars in the match */
    double          Entrpy;  /* The entropy of the match */
    double          MltEnpy; /* Entropy with additional allowance for multipart password */
    ZxcTypeMatch_t  Type;    /* Type of match (Spatial/Dictionary/Order/Repeat) */
    struct ZxcMatch *Next;
};
typedef struct ZxcMatch ZxcMatch_t;


namespace Ui {
class AddEntry;
}

class AddEntry : public QDialog
{
    Q_OBJECT

public:
     // Constructor when calling from add
    explicit AddEntry(QWidget *parent = 0);

    //Constructor when calling from edit.
    explicit AddEntry(QWidget *parent, QString EditUserIn, QString EditPassIn, QString EditDomIn, QString EditDescIn);

    ~AddEntry();//Destructor

    //to clean model and shared memory
    void clean();

    //Methods to access data from main.
    QString getUser();
    QString getDomain();
    QString getPassword();
    QString getDescription();



private slots:
    void on_InUser_textChanged(const QString &arg1);
    void on_InDomain_textChanged(const QString &arg1);
    void on_InPass_textChanged(const QString &text);
    void on_InPass2_textChanged(const QString &text);
    void on_sh_pass_toggled(bool checked);
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_gen_pass_clicked();
    void on_pb_confgen_clicked();
    void on_pb_secInfo_clicked();
    void build_info_model(const char *Pwd);

private:
    Ui::AddEntry *ui;

    void EnableOkButton();
    void PasswordWarning();
    void load_zxcvbn_dicts();

    bool EqPass;

    double e, elog;
    ZxcMatch_t *Info, *p;
    QStandardItemModel *model=0;      //model for the subpass
    QStandardItemModel *model_whole=0;//model for first pass only
    QStandardItemModel *model_multi=0;//model for multi bits
    QStandardItemModel *model_times=0;//model for crack times
    QList<QStandardItem *> items;
    QString currentPass;

    char **userDict;
//    const char ** userDictConst;

    const QStringList attacksType = QStringList() << "Throttled online attack"
                                                  << "Unthrottled online attack"
                                                  << "Offline attack, slow hash, many cores"
                                                  << "Offline attack, fast hash, many cores";

    const QStringList attacksTime_s = QStringList() << "100 / hour"
                                                    << "10  / sec"
                                                    << "10K / sec"
                                                    << "10B / sec";


    const double attacksTime[4] = {-1.55623,1,4,10};

    QLibrary * zxcvbnLib=0;
    typedef double (*ZxcvbnMatch_type)(const char *Passwd, const char *UserDict[], ZxcMatch_t **Info);
    ZxcvbnMatch_type ZxcvbnMatch=0;

    typedef void (*ZxcvbnFreeInfo_type)(ZxcMatch_t *Info);
    ZxcvbnFreeInfo_type ZxcvbnFreeInfo=0;


};

#endif // ADDENTRY_H

