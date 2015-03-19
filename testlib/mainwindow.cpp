#include "mainwindow.h"

const char *APP_TITLE = QT_TR_NOOP("TestLib");

#include "xmleditwidget.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QXmlEditGlobals::setAppTitle(APP_TITLE);
    XmlEditWidget* e = new XmlEditWidget(this);
    QFile f("../testlib/svg.xml");
    //QFile f("./consolidation_tree_schema.xml");
    f.open(QIODevice::ReadOnly);
    QTextStream ts(&f);
    bool ischange = true, isask = true;
    QString xml = ts.readAll();
    qDebug("loaded an xml of size %d ...", xml.size());
    e->loadText(xml, ischange, isask);
    //e->on_closeSearchPanel_clicked();
    e->setMoveButtonsVisible(false);
    e->setViewAsXSDButtonVisible(false);
    e->setEncodingLabelVisible(false);
    e->resizeTreeColumns();
    e->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum); // Maximum
    e->setMinimumSize(e->maximumSize()); //e->sizeHint()
    e->resize(e->maximumSize()); // e->sizeHint()
    setCentralWidget(e);
    //setLayout(new QVBoxLayout());
    //layout()->addWidget(e);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum); // Maximum // Preferred MinimumExpanding
    resize(sizeHint());  //minimumSize() // maximumSize(): fullscreen ! // sizeHint() maximumSize()
    resize(e->sizeHint());
}

MainWindow::~MainWindow()
{

}
