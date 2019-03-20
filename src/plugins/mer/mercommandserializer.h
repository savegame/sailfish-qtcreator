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

#ifndef MERCOMMANDSERIALIZER_H
#define MERCOMMANDSERIALIZER_H

#include <QObject>
#include <QBasicTimer>
#include <QQueue>

class QProcess;

namespace Mer {
namespace Internal {

class MerCommandSerializer : public QObject
{
    Q_OBJECT

public:
    explicit MerCommandSerializer(QObject *parent);

    ~MerCommandSerializer() override;

    bool runSynchronous(QProcess *process);

    void runAsynchronous(QProcess *process);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void scheduleDequeue();

    void dequeue();

private slots:
    void finalize();

private:
    QQueue<QProcess *> m_queue;
    QProcess *m_current{};
    QBasicTimer m_dequeueTimer;
};


}
}

#endif // MERCOMMANDSERIALIZER_H
