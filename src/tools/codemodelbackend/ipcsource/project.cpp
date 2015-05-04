/****************************************************************************
**
** Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "project.h"

#include <cstring>

#include <utf8stringvector.h>

#include <projectcontainer.h>

namespace CodeModelBackEnd {

class ProjectData {
public:
    ProjectData(const Utf8String &projectFilePath);

public:
    time_point lastChangeTimePoint;
    std::vector<const char*> arguments;
    Utf8String projectFilePath;
};

ProjectData::ProjectData(const Utf8String &projectFilePath)
    : lastChangeTimePoint(std::chrono::high_resolution_clock::now()),
      projectFilePath(projectFilePath)
{
}

Project::Project(const Utf8String &projectFilePath)
    : d(std::make_shared<ProjectData>(projectFilePath))
{
}

Project::Project(const ProjectContainer &projectContainer)
    : d(std::make_shared<ProjectData>(projectContainer.filePath()))
{
    setArguments(projectContainer.arguments());
}

Project::~Project() = default;

Project::Project(const Project &) = default;
Project &Project::operator =(const Project &) = default;

Project::Project(Project &&other)
    : d(std::move(other.d))
{
}

Project &Project::operator =(Project &&other)
{
    d = std::move(other.d);

    return *this;
}

void Project::clearProjectFilePath()
{
    d->projectFilePath.clear();
}

const Utf8String &Project::projectFilePath() const
{
    return d->projectFilePath;
}

static const char *strdup(const Utf8String &utf8String)
{
    char *cxArgument = new char[utf8String.byteSize() + 1];
    std::memcpy(cxArgument, utf8String.constData(), utf8String.byteSize() + 1);

    return cxArgument;
}

void Project::setArguments(const Utf8StringVector &arguments)
{
    d->arguments.resize(arguments.size());
    std::transform(arguments.cbegin(), arguments.cend(), d->arguments.begin(), strdup);
    updateLastChangeTimePoint();
}

const std::vector<const char*> &Project::arguments() const
{
    return d->arguments;
}

int Project::argumentCount() const
{
    return d->arguments.size();
}

const char * const *Project::cxArguments() const
{
    return arguments().data();
}

const time_point &Project::lastChangeTimePoint() const
{
    return d->lastChangeTimePoint;
}

void Project::updateLastChangeTimePoint()
{
    d->lastChangeTimePoint = std::chrono::high_resolution_clock::now();
}

bool operator ==(const Project &first, const Project &second)
{
    return first.projectFilePath() == second.projectFilePath();
}

} // namespace CodeModelBackEnd

