#ifndef CHOOSEXSDVIEWROOTITEMDIALOG_H
#define CHOOSEXSDVIEWROOTITEMDIALOG_H

#include "xmlEdit.h"
#include <QListWidgetItem>
#include "libQXmlEdit_global.h"

class XSchemaElement;

namespace Ui
{
class ChooseXSDViewRootItemDialog;
}

class LIBQXMLEDITSHARED_EXPORT ChooseXSDViewRootItemDialog : public QDialog
{
    Q_OBJECT
    QString _selection ;
protected:
    void enableOK();
    void setupData(QList<XSchemaElement*> elements);
    virtual void accept();
public:
    explicit ChooseXSDViewRootItemDialog(QWidget *parent, QList<XSchemaElement*> elements);
    virtual ~ChooseXSDViewRootItemDialog();
    QString selection() ;

private:
    Ui::ChooseXSDViewRootItemDialog *ui;
private slots:
    void on_list_itemDoubleClicked(QListWidgetItem *item);
    void on_list_itemSelectionChanged();
};

#endif // CHOOSEXSDVIEWROOTITEMDIALOG_H
