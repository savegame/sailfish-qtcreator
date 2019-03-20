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
    const char FILTER[] = "--filter";
    const char IMAGE_FILTER[] = "label=SFOS_VERSION";
    const char FORMAT[] = "--format";
    const char IMAGE_FORMAT[] = "{{.Repository}}";
    const char UNNAMED_IMAGE[] = "<none>";
    const char DOCKER[] = "docker";
}

namespace Mer {
namespace Internal {

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

MerDockerManager::MerDockerManager(QObject *parent): QObject(parent)
{
    m_serializer = new MerCommandSerializer(this);
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
    arguments.append(QLatin1String(IMAGE_FORMAT));

    MerCommandProcess process(m_serializer, dockerPath());

    if (!process.runSynchronously(arguments))
        return images;

    return listedImages(QString::fromLocal8Bit(process.readAllStandardOutput()));
}
}
}
