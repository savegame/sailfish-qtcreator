/****************************************************************************
**
** Copyright (C) 2019 Jolla Ltd.
** Contact: http://jolla.com/
**
** This file is part of Qt Creator.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Digia.
**
****************************************************************************/

#include "mercommandprocess.h"
#include "mercommandserializer.h"
namespace Mer {
namespace Internal {

MerCommandProcess::MerCommandProcess(MerCommandSerializer *serializer, const QString& program, QObject *parent)
    : QProcess(parent),
      m_serializer(serializer)
{
    setProcessChannelMode(QProcess::ForwardedErrorChannel);
    setProgram(program);
}

bool MerCommandProcess::runSynchronously(const QStringList &arguments)
{
    setArguments(arguments);
    return m_serializer->runSynchronous(this);
}

void MerCommandProcess::runAsynchronously(const QStringList &arguments)
{
    setArguments(arguments);
    return m_serializer->runAsynchronous(this);
}

void MerCommandProcess::setDeleteOnFinished()
{
    void (QProcess::*QProcess_finished)(int, QProcess::ExitStatus) = &QProcess::finished;
    connect(this, &QProcess::errorOccurred, this, &QObject::deleteLater);
    connect(this, QProcess_finished, this, &QObject::deleteLater);
}
}
}
