/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef ADDMERTARGETOPERATION_H
#define ADDMERTARGETOPERATION_H

#include "operation.h"

class AddMerTargetOperation : public Operation
{
public:
    AddMerTargetOperation();

    QString name() const;
    QString helpText() const;
    QString argumentsHelpText() const;

    bool setArguments(const QStringList &args);

    int execute() const;

#ifdef WITH_TESTS
    //TODO:
    bool test() const { return false; }
#endif

    static QVariantMap initializeTargets();
    static QVariantMap addTarget(const QVariantMap &map, const QString &name, const QString &qmakeFileName,
            const QString &gccDumpMachineFileName, const QString &gccDumpMacrosFileName,
            const QString &gccDumpIncludesFileName, const QString &rpmValidationSuitesFileName);
    static QVariantMap load(const QString &root);
    static bool save(const QVariantMap &map, const QString &root);
private:   

    static QString readCacheFile(const QString &cacheFile);

private:
    QString m_targetsDir;
    QString m_targetName;
    QString m_qmakeQueryFileName;
    QString m_gccDumpmachineFileName;
    QString m_gccDumpmacrosFileName;
    QString m_gccDumpincludesFileName;
    QString m_rpmValidationSuitesFileName;
};

#endif // ADDMERTARGETOPERATION_H
