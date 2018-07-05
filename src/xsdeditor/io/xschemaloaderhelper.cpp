/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

#include "xmlEdit.h"
#include "xschemaloaderhelper.h"
#include "utils.h"

XSchemaLoaderHelper::XSchemaLoaderHelper(QObject *parent) :
    QObject(parent)
{
    _isError = false;
    _networkReply = NULL ;
    _errorCode = QNetworkReply::UnknownNetworkError;
    _isAsynch = false;

}

XSchemaLoaderHelper::~XSchemaLoaderHelper()
{
}


void XSchemaLoaderHelper::abort()
{
    if(NULL != _networkReply) {
        Utils::TODO_THIS_RELEASE("corretto?");
        disconnectReply();
        _networkReply->abort();
        _networkReply->deleteLater();
        _networkReply = NULL ;
    }
}

QNetworkReply* XSchemaLoaderHelper::loadSchemaUsingUrl(QUrl &url, QNetworkAccessManager *networkAccessManager, const bool asynch)
{
    _isAsynch = asynch ;
    _isError = false;
    _request.setUrl(url);
    Utils::TODO_THIS_RELEASE("");
    _networkReply = networkAccessManager->get(_request);
    if(NULL == _networkReply) {
        return NULL ;
    }
    connect(_networkReply, SIGNAL(finished()), this, SLOT(finished()));
    connect(_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onNetworkAccessError(QNetworkReply::NetworkError)));
    if(!asynch) {
        int code = _eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
        disconnectReply();
        if(0 == code) {
            _networkReply->deleteLater();
            _networkReply = NULL ;
        }
    }
    return _networkReply;
}

void XSchemaLoaderHelper::disconnectReply()
{
    if(NULL != _networkReply) {
        disconnect(_networkReply, SIGNAL(finished()), this, SLOT(finished()));
        disconnect(_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onNetworkAccessError(QNetworkReply::NetworkError)));
    }
}

void XSchemaLoaderHelper::finished()
{
    disconnectReply();
    if(NULL != _networkReply) {
        if(_networkReply->error() != QNetworkReply::NoError) {
            _isError = true ;
            _errorCode = _networkReply->error();
            _errorString = _networkReply->errorString();
        }
    } else {
        _isError = true ;
    }
    end();
}

void XSchemaLoaderHelper::onNetworkAccessError(QNetworkReply::NetworkError code)
{
    disconnectReply();
    _isError = true ;
    if(QNetworkReply::NoError == code) {
        _errorCode = code ;
    }
    end();
}

void XSchemaLoaderHelper::end()
{
    if(_isAsynch) {
        if(_isError) {
            emit error(_errorCode, _errorString);
            if(NULL != _networkReply) {
                _networkReply->deleteLater();
                _networkReply = NULL ;
            }
        } else {
            emit finished(_networkReply);
        }

    } else {
        if(_isError) {
            _eventLoop.exit(0);
        } else {
            _eventLoop.exit(1);
        }
    }
}


