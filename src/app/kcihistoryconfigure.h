/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *  This file is part of Kreogist-Cuties.
 *
 *    Kreogist-Cuties is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist-Cuties is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist-Cuties.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KCIHISTORYCONFIGURE_H
#define KCIHISTORYCONFIGURE_H

#include <QString>
#include <QStringList>

#include "kciconfigure.h"

class kciHistoryConfigure : public kciConfigure
{
public:
    static kciHistoryConfigure* getInstance();

    void readConfigure();
    void writeConfigure();

    void cleanAllHistorys();

    void setTrackUserHistoryEnabled(bool enabled);
    bool isTrackUserHistoryEnabled() const;

    void setHistoryDir(const QString& dirPath);
    QString getHistoryDir() const;

    void clearAllUnClosedFilePaths();
    QList<QString> getAllUnClosedFilePaths() const;
    void addUnClosedFilePath(const QString& path);

private:
    kciHistoryConfigure();
    QString historyDirPath;
    QStringList RecentlyOpenedFiles;
    int RecentlyOpenedFileListSize;
    bool trackUserHistory;
    bool cleanMark;
    QList<QString> unClosedFilePaths;
    static kciHistoryConfigure* instance;
};

#endif // KCIHISTORYCONFIGURE_H
