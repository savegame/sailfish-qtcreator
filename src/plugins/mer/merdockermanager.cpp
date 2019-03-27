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

#include "merdockermanager.h"

#include "mercommandprocess.h"
#include "mercommandserializer.h"

#include <QDir>
#include <QProcess>
#include <QRegularExpression>


namespace  {
    const char IMAGE[] = "image";
    const char LS[] = "ls";
    const char PS[] = "ps";
    const char FILTER[] = "--filter";
    const char IMAGE_FILTER[] = "label=SFOS_VERSION";
    const char FORMAT[] = "--format";
    const char REPOSITORY_FORMAT[] = "{{.Repository}}";
    const char IMAGE_FORMAT[] = "{{.Image}}";
    const char UNNAMED_IMAGE[] = "<none>";
    const char DOCKER[] = "docker";
    const char RUN[] = "run";
}

namespace Mer {
namespace Internal {

constexpr const char MerDockerManager::TYPE[];

static QString dockerPath()
{
    static QString path;
    if (!path.isNull()) {
        return path;
    }

    QStringList searchPaths = QProcessEnvironment::systemEnvironment()
        .value(QLatin1String("PATH")).split(QLatin1Char(':'));
    searchPaths.append(QLatin1String("/usr/local/bin"));

    foreach (const QString &searchPath, searchPaths) {
        QDir dir(searchPath);
        if (dir.exists(QLatin1String(DOCKER))) {
            path = dir.absoluteFilePath(QLatin1String(DOCKER));
            break;
        }
    }

    return path;
}

MerDockerManager::MerDockerManager(QObject *parent): MerAbstractVMManager(parent)
{
    m_serializer = new MerCommandSerializer(this);
}

QString MerDockerManager::type() const
{
    return QLatin1String(MerDockerManager::TYPE);
}

QStringList listedImages(const QString &output)
{
    QStringList images;
    QStringList lines = output.split(QRegularExpression("[\r\n]"), QString::SkipEmptyParts);
    for(QString line : lines) {
        if (line != QLatin1String(UNNAMED_IMAGE))
            images.append(line);
    }
    return images;
}

QStringList MerDockerManager::fetchImages()
{
    QStringList images;
    QStringList arguments;
    arguments.append(QLatin1String(IMAGE));
    arguments.append(QLatin1String(LS));
    arguments.append(QLatin1String(FILTER));
    arguments.append(QLatin1String(IMAGE_FILTER));
    arguments.append(QLatin1String(FORMAT));
    arguments.append(QLatin1String(REPOSITORY_FORMAT));

    MerCommandProcess process(m_serializer, dockerPath());

    if (!process.runSynchronously(arguments))
        return images;

    return listedImages(QString::fromLocal8Bit(process.readAllStandardOutput()));
}

VirtualMachineInfo MerDockerManager::fetchVirtualMachineInfo(const QString &imageName)
{
    Q_UNUSED(imageName);
    VirtualMachineInfo info;
    info.sshPort = 2222;
    info.wwwPort = 8080;
    info.sharedHome = QDir::home().absolutePath();
    info.sharedTargets = QDir::home().absolutePath() + "/SailfishOS/mersdk/targets";
    info.sharedSsh = QDir::home().absolutePath() + "/SailfishOS/mersdk/ssh";
    info.sharedConfig = QDir::home().absolutePath() + "/SailfishOS/vmshare";
    info.sharedSrc = QDir::home().absolutePath();
    info.headless = true;
    return info;
}

void MerDockerManager::isVirtualMachineRunning(const QString &vmName, QObject *context, std::function<void (bool, bool)> slot)
{
    QStringList images = fetchImages();
    if (!images.contains(vmName)) {
        slot(false, false);
        return;
    }

    QStringList arguments;
    arguments.append(QLatin1String(PS));
    arguments.append(QLatin1String(FORMAT));
    arguments.append(QLatin1String(IMAGE_FORMAT));
    MerCommandProcess *process = new MerCommandProcess(m_serializer, dockerPath(), this);

    void (QProcess::*QProcess_finished)(int, QProcess::ExitStatus) = &QProcess::finished;
    connect(process, QProcess_finished, context,
            [process, vmName, slot](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitCode);
                Q_UNUSED(exitStatus);
                bool isRunning = isContainerRunningFromList(
                            QString::fromLocal8Bit(process->readAllStandardOutput()),
                            vmName);
                slot(isRunning, true);
            });

    process->setDeleteOnFinished();
    process->runAsynchronously(arguments);
}

void MerDockerManager::startVirtualMachine(const QString &vmName, bool headless)
{
    Q_UNUSED(headless);
    VirtualMachineInfo vmInfo = fetchVirtualMachineInfo(vmName);
    QStringList arguments;
    arguments.append(QLatin1String(RUN));
    arguments.append(QLatin1String("-d"));
    arguments.append(QLatin1String("--privileged"));
    arguments.append(QLatin1String("-p"));
    // TODO: use ports from vminfo
    arguments.append(QLatin1String("2222:22"));
    arguments.append(QLatin1String("-p"));
    arguments.append(QLatin1String("8080:9292"));
    arguments.append(QLatin1String("-v"));
    arguments.append(vmInfo.sharedHome + ":/home/mersdk/share");
    arguments.append(QLatin1String("-v"));
    arguments.append(vmInfo.sharedTargets + ":/host_targets");
    arguments.append(QLatin1String("-v"));
    arguments.append(QLatin1String("srvmer:/srv/mer/"));
    arguments.append(QLatin1String("-v"));
    arguments.append(vmInfo.sharedSsh + ":/etc/ssh/authorized_keys");
    arguments.append(vmName);

     MerCommandProcess *process = new MerCommandProcess(m_serializer, dockerPath(), this);
     process->setDeleteOnFinished();
     process->runAsynchronously(arguments);
}

bool MerDockerManager::isContainerRunningFromList(const QString &containerList, const QString &container)
{
    QStringList runningContainers = listedImages(containerList);
    return runningContainers.contains(container);
}

}
}
