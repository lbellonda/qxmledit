/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef XSLTEXECDIALOG_H
#define XSLTEXECDIALOG_H

#include "xmlEdit.h"
#include "modules/utils/comboutils.h"

class XSLTExecutor;
class ApplicationData;
class XmlEditWidget;

namespace Ui
{
class XSLTExecDialog;
}

class XSLTExecDialog : public QDialog
{
    Q_OBJECT
    static const int NormalView = 0;
    static const int ProgressView = 1;

    bool _running ;
    ComboUtils _utilsInput, _utilsXSL, _utilsOutput ;
    ApplicationData *_data;
    bool _inputDataAsFile;
    QString _inputData;
    bool _inputXSLAsFile;
    QString _inputXSL;
    XmlEditWidget *_inputEditor;
    XmlEditWidget *_xslEditor;
    bool _outputAsFile;
    QString _outputFile;
    QString _outputString;
    QSet<QString> _parameters ;
protected:

    void finishSetup(XmlEditWidget *source, XmlEditWidget *xsl);
    void startOperation();
    void openFolder(const QString &outputFilePath);
    void openStringInEditor(const QString &output);
    void openFileInEditor(const QString &outputFilePath);
    void setInputEditor(XmlEditWidget *editor);
    void setInputDataFile(const QString &fileName);
    void setXSLEditor(XmlEditWidget *editor);
    void setXSLFile(const QString &fileName);
    bool setInput(XSLTExecutor *xsltExecutor);
    void setupRecentFolders(QToolButton *button, ComboUtils *util, const bool isSave, const char *method, const QString &file, XmlEditWidget *editor);
    QString fileForSelection(ComboUtils::ComboItem *item, const QString &message, const bool isOpen);
    bool existsEditor(XmlEditWidget *editor);
    void errorNotEditor(const QString &type);
    void errorEmptyFileString(const QString &type);
    void loadSources(XmlEditWidget *source, XmlEditWidget *xsl);
    void selectDefaults();
    void mergeParameters();
    int addParamRow(const QString &text, const QString &value = "");
    void evaluateParamEnabling();
    QHash<QString, QString> getParameters();
    void resetParameters();
    QString fromEditorToFile(const bool useFile, XmlEditWidget *source);
    XmlEditWidget *fromEditorToEditor(const bool useFile, XmlEditWidget *source);

public:
    explicit XSLTExecDialog(QWidget *parent, ApplicationData *data, XmlEditWidget *source = NULL, XmlEditWidget *xsl = NULL);
    virtual ~XSLTExecDialog();

    static bool execDialog(QWidget *parent, ApplicationData *data, XmlEditWidget *source = NULL, XmlEditWidget *xsl = NULL);

private:
    Ui::XSLTExecDialog *ui;
private slots:
    void onChooseInput();
    void onChooseOutput();
    void onChooseXSL();
    void accept();
    void onBBOXlicked(QAbstractButton* button);
    void on_params_itemSelectionChanged();
    void on_cmdInsParam_clicked();
    void on_cmdModParam_clicked();
    void on_cmdDelParam_clicked();
    void on_cmdResetParam_clicked();
#ifdef  QXMLEDIT_TEST
    friend class TestExecXSLT;
#endif
};

#endif // XSLTEXECDIALOG_H
