#include "xmlerrormanagerdialog.h"
#include "ui_xmlerrormanagerdialog.h"
#include "modules/xml/xmlloadcontext.h"
#include <QDesktopServices>
#include "utils.h"

XMLErrorManagerDialog::XMLErrorManagerDialog(QWidget *parent, XMLLoadContext *newContext, QXmlStreamReader *xmlReader) :
    QDialog(parent),
    ui(new Ui::XMLErrorManagerDialog)
{
    _xmlReader = xmlReader ;
    _error = false;
    _context = newContext ;
    ui->setupUi(this);
    Utils::showWaitCursor();
    readErrorData();
    Utils::restoreCursor();
    QPushButton *noButton = ui->buttonBox->button(QDialogButtonBox::No);
    if(NULL != noButton) {
        noButton->setDefault(true);
    }
    if(NULL != xmlReader) {
        QFile *file = qobject_cast<QFile*>(xmlReader->device());
        if(NULL != file) {
            _filePath = file->fileName();
            QFileInfo info(*file);
            QDir dir = info.dir();
            _dirPath = dir.absolutePath();
        }
    }
    if(_filePath.isEmpty()) {
        ui->copyFilePath->setEnabled(false);
    }
    if(_dirPath.isEmpty()) {
        ui->showContainingFolder->setEnabled(false);
    }
}

XMLErrorManagerDialog::~XMLErrorManagerDialog()
{
    delete ui;
}

static qint64 clip0(const qint64 input)
{
    if(input < 0) {
        return 0;
    }
    return input;
}

/*
n = 2
n-4 = -2
-2|-1| 0| 1| 2|
..|..|..|..| X|
 4| 3| 2| 1| 0|
*/
int XMLErrorManagerDialog::charsBeforeError(const qint64 charOffset, const int bufferSize)
{
    qint64 charErrorPosition = 0;
    const qint64 firstCharacterToLoad = charOffset - bufferSize ;
    if(firstCharacterToLoad < 0) {
        charErrorPosition = charOffset - 1;
    } else {
        charErrorPosition = bufferSize ;
    }
    return static_cast<int>(charErrorPosition) ;
}

void XMLErrorManagerDialog::readErrorData()
{
    readData(_xmlReader->device());
    if(_error) {
        ui->textAfter->setVisible(false);
        ui->textError->setVisible(false);
        ui->textBefore->setVisible(false);
        ui->errorAfter->setVisible(false);
        ui->errorError->setVisible(false);
        ui->errorBefore->setVisible(false);
    }
    ui->errorMessage->setText(_context->errorMessage());

    ui->textAfter->setText(_afterString);
    ui->textError->setText(_errorPosString);
    ui->textBefore->setText(_beforeString);
}

bool XMLErrorManagerDialog::readData(QIODevice *device)
{
    _error = true ;
    if(!device->seek(0)) {
        return false ;
    }
    QTextStream stream(device);
    stream.setCodec(QTextCodec::codecForName(_context->encoding().toLatin1().data()));
    return readData(&stream, _context->characterOffset());
}

bool XMLErrorManagerDialog::readData(QTextStream *stream, const qint64 charOffset)
{
    _error = false;
    // this is the first character to load
    const qint64 firstCharacterToLoad = clip0(charOffset - CharsBefore) ;
    const qint64 blocksToSkip = firstCharacterToLoad / BlockSize ;
    const qint64 lastFractionSize = clip0(firstCharacterToLoad - (blocksToSkip * BlockSize)) ;
    const int beforeErrorChars = charsBeforeError(charOffset - firstCharacterToLoad, CharsBefore);
    for(qint64 index = 0; index < blocksToSkip ; index ++) {
        stream->read(BlockSize) ;
        if(stream->status() != QTextStream::Ok) {
            _error = true ;
            return false ;
        }
    }
    if(lastFractionSize > 0) {
        stream->read(lastFractionSize) ;
        if(stream->status() != QTextStream::Ok) {
            _error = true ;
            return false ;
        }
    }
    // read the effective data
    const QString &result = stream->read(CharsBefore * 2) ;
    if(stream->status() != QTextStream::Ok) {
        _error = true ;
        return false ;
    }
    if(beforeErrorChars > 0) {
        _beforeString = escapeString(result.left(beforeErrorChars));
    }
    _errorPosString = escapeString(result.mid(beforeErrorChars, 1));
    _afterString = escapeString(result.mid(beforeErrorChars + 1));
    return true;
}

QString XMLErrorManagerDialog::escapeString(const QString &str)
{
    QString result = str ;
    result = result.replace("\r\n", "\\r\\n");
    result = result.replace("\n", "\\n");
    result = result.replace("\r", "\\r");
    return result;
}

void XMLErrorManagerDialog::on_buttonBox_accepted()
{
    accept();
}

void XMLErrorManagerDialog::on_buttonBox_rejected()
{
    reject();
}

void XMLErrorManagerDialog::on_showContainingFolder_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(_dirPath));
}

void XMLErrorManagerDialog::on_copyFilePath_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    if(NULL != clipboard) {
        clipboard->setText(_filePath);
    }
}
