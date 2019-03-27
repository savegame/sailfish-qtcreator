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

#ifndef MERABSRACTVMMANAGER_H
#define MERABSRACTVMMANAGER_H

#include <QObject>

#include <functional>


namespace Mer {
namespace Internal {

class VirtualMachineInfo
{
public:
    VirtualMachineInfo() : sshPort(0), wwwPort(0), headless(false) {}
    QString sharedHome;
    QString sharedTargets;
    QString sharedConfig;
    QString sharedSrc;
    QString sharedSsh;
    quint16 sshPort;
    quint16 wwwPort;
    QList<quint16> freePorts;
    QList<quint16> qmlLivePorts;
    QStringList macs;
    bool headless;
};

class MerAbstractVMManager : public QObject
{
    Q_OBJECT
public:
    explicit MerAbstractVMManager(QObject *parent = nullptr);

    virtual QString type() const = 0;

    virtual void isVirtualMachineRunning(const QString &vmName, QObject *context,
                                         std::function<void(bool,bool)> slot) = 0;
    virtual void startVirtualMachine(const QString &vmName, bool headless) = 0;

signals:

public slots:
};

}
}

#endif // MERABSRACTVMMANAGER_H
