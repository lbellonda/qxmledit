#ifndef XMLERRORMANAGERDIALOG_H
#define XMLERRORMANAGERDIALOG_H

#include "xmlEdit.h"
#include "regola.h"

namespace Ui
{
class XMLErrorManagerDialog;
}

class XMLErrorManagerDialog : public QDialog
{
    Q_OBJECT
    bool _error;
    QString _beforeString ;
    QString _errorPosString ;
    QString _afterString ;
    XMLLoadContext *_context;
    QXmlStreamReader *_xmlReader ;
    QString _filePath ;
    QString _dirPath ;

    static const qint64 CharsBefore = 20 ;
    static const qint64 BlockSize = 10000 ;

public:
    explicit XMLErrorManagerDialog(QWidget *parent, XMLLoadContext *newContext, QXmlStreamReader *xmlReader);
    ~XMLErrorManagerDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_copyFilePath_clicked();
    void on_showContainingFolder_clicked();

private:
    Ui::XMLErrorManagerDialog *ui;

    bool readData(QTextStream *stream, const qint64 charOffset);
    void readErrorData();
    bool readData(QIODevice *device);
    QString escapeString(const QString &str);
    static int charsBeforeError(const qint64 charOffset, const int bufferSize);

#if defined(QXMLEDIT_TEST)
    friend class TestLoadFile;
#endif
};

#endif // XMLERRORMANAGERDIALOG_H
