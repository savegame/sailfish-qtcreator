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

#include "mercommandserializer.h"
#include <QProcess>
#include <QTimerEvent>

namespace Mer {
namespace Internal {

MerCommandSerializer::MerCommandSerializer(QObject *parent)
    : QObject(parent)
{
}

MerCommandSerializer::~MerCommandSerializer()
{
}

bool MerCommandSerializer::runSynchronous(QProcess *process)
{
    m_queue.prepend(process);

    // Currently runnning an asynchronous process?
    if (m_current)
        m_current->waitForFinished();

    dequeue();

    return m_current->waitForFinished();
}

void MerCommandSerializer::runAsynchronous(QProcess *process)
{
    m_queue.enqueue(process);
    scheduleDequeue();
}

void MerCommandSerializer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_dequeueTimer.timerId()) {
        m_dequeueTimer.stop();
        dequeue();
    } else  {
        QObject::timerEvent(event);
    }
}

void MerCommandSerializer::scheduleDequeue()
{
    m_dequeueTimer.start(0, this);
}

void MerCommandSerializer::dequeue()
{
    if (m_current)
        return;
    if (m_queue.isEmpty())
        return;

    m_current = m_queue.dequeue();

    connect(m_current, &QProcess::errorOccurred, this, &MerCommandSerializer::finalize);
    void (QProcess::*QProcess_finished)(int, QProcess::ExitStatus) = &QProcess::finished;
    connect(m_current, QProcess_finished, this, &MerCommandSerializer::finalize);

    m_current->start(QIODevice::ReadWrite | QIODevice::Text);
}

void MerCommandSerializer::finalize()
{

    m_current->disconnect(this);
    m_current = nullptr;

    scheduleDequeue();
}
}
}
