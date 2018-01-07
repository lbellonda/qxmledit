/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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


#ifndef EXTRACTIONFRONTEND_H
#define EXTRACTIONFRONTEND_H

#include "xmlEdit.h"
#include "extractionoperation.h"
#include "modules/services/uidservices.h"

class ExtractionOperation;

namespace Ui
{
class ExtractionFrontEnd;
}

class ExtractionFrontEnd : public QDialog
{
    Q_OBJECT
    ExtractionOperation *_operation;
    bool    _running ;
    QFuture<void> _future ;
    UIDesktopServices _uiServices;
    bool _pbDeterminate ;

    void searchInFileWorking();
    void extractFragmentsWorkThread();
    void endOfOperation();
    void setPBIndeterminate();
    void setPBDeterminate();

public:
    explicit ExtractionFrontEnd(ExtractionOperation *operation, QWidget *parent, QWidget *mainWidget);
    ~ExtractionFrontEnd();

private:
    Ui::ExtractionFrontEnd *ui;

private slots:
    void on_cmdCancel_clicked();
    void checkIfDone();
};

#endif // EXTRACTIONFRONTEND_H
