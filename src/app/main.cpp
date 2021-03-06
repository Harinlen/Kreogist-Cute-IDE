/*
 *  Copyright 2013 Kreogist Dev Team
 *
 *  This file is part of Kreogist Cuties.
 *
 *    Kreogist Cuties is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *    Kreogist Cuties is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kreogist Cuties.  If not, see <http://www.gnu.org/licenses/>. *
 */

/*!
 *  \copyright GNU Public License
 *  \details
 *    This file contains the main() of this program. Many initialization functions are defined here.
 */

#include <QApplication>
#include <QString>
#include <QDebug>

#include <iostream>

#include "mainwindow.h"
#include "kcsplashscreen.h"
#include "kreogistcutestyle.h"
#include "kcstatusrecorder.h"
#include "kcdocumentrecorder.h"
#include "kclanguageconfigure.h"

/*!
 * \brief setApplicationInfo function is used to set basic information of Cuties
 *        and window icon.
 *
 */
static inline void loadApplicationInfo()
{
    //Set application details.
    QApplication::setApplicationName(QString("Cuties"));
    QApplication::setApplicationVersion(QString("0.3.0.39"));
    QApplication::setOrganizationName("Kreogist Dev Team");
    QApplication::setOrganizationDomain("https://kreogist.github.io/Cuties");

    //Set window icon
    QApplication::setWindowIcon(QIcon(":/mainicon/image/Cuties.png"));

    //Set application effects
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, true);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu, true);
    QApplication::setEffectEnabled(Qt::UI_AnimateToolBox, true);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip, true);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip, true);
}

/*!
 * \brief initApplicationSettings function is used to load settings
 */
static inline void loadApplicationSettings()
{
    KCConfigure::setCfgFileName(qApp->applicationDirPath()+
                                "/Settings/"+
                                qApp->applicationName()+
                                ".ini");
    KCStatusRecorder::getInstance()->readRecord();
    KCDocumentRecorder::getInstance()->readSettings();
    KCLanguageConfigure::getInstance()->readConfigure();
}

static inline void printVersion()
{
    printf("Kreogist %s %s\n",qApp->applicationName().toLocal8Bit().constData(),
           qApp->applicationVersion().toLocal8Bit().constData());
    printf("Copyright (C) 2013 %s. <%s>\n", qApp->organizationName().toLocal8Bit().constData(),
           qApp->organizationDomain().toLocal8Bit().constData());
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}

static inline void printHelp()
{
    QByteArray appName=qApp->applicationName().toLocal8Bit();
    printf("Usage:\n");
    printf("  %s -h,--help              Print this message\n",
           appName.constData());
    printf("  %s -v,--version           Print %s version\n",
           appName.constData(),
           appName.constData());
    printf("  %s [FILES]                Open [FILES] in %s\n",
           appName.constData(),
           appName.constData());
    fflush(stdout);
}

static inline void processArg()
{
    QStringList argList=qApp->arguments();

    //remove the application name
    argList.removeFirst();

    for(int i=0,l=argList.size(); i<l; i++)
    {
        const QString &arg=argList.at(i);
        if(arg == "--version" || arg == "-v")
        {
            printVersion();
            std::exit(0);
        }
        else if(arg == "--help" || arg == "-h")
        {
            printHelp();
            std::exit(0);
        }
        else if(QFileInfo(arg).exists())
        {
            /*
             * We didn't have a normal way to open a file before the MainWindow be created.
             * But MainWindow will open the files which didn't be closed when quit Cuties.
             * So we add it as a unclosed file so that when the mainwindow show,it will be opened.
             */
            KCDocumentRecorder::getInstance()->appendRecord(QFileInfo(arg).absoluteFilePath(),0,0);
        }
    }

}

/*!
 * \brief This is a custom message handler to deal with Qt's message(for example, qDebug).
 *
 *  KCMessageHandler outputs message which was sended by calling qDebug()(and so on) to both file
 *  (named log.txt) and stderr. This function will be called by Qt.
 * \param type describes the messages that be sent to a message handler
 * \param context This provides additional information about a log message.
 * \param msg message
 */
void KCMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message;

    switch(type)
    {
    case QtDebugMsg:
        message.sprintf("Debug: (%s:%u\n%s)\n",
                        context.file,
                        context.line,
                        context.function);
        break;
    case QtWarningMsg:
        message.sprintf("Warning: (%s:%u\n%s)\n",
                        context.file,
                        context.line,
                        context.function);
        break;
    case QtCriticalMsg:
        message.sprintf("Critical: (%s:%u\n%s)\n",
                        context.file,
                        context.line,
                        context.function);
        break;
    case QtFatalMsg:
        message.sprintf("Fatal: (%s:%u\n%s)\n",
                        context.file,
                        context.line,
                        context.function);
    default:
        message.sprintf("Unknow Message Type: (%s:%u\n%s)\n",
                        context.file,
                        context.line,
                        context.function);
    }

    std::cerr<<message.toStdString()<<msg.toStdString()<<std::endl<<std::endl;

    QFile file(QApplication::applicationDirPath()+"/log.txt");
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out<<QTime::currentTime().toString().toUtf8()<<endl;
        out<<message;

        if(type==QtFatalMsg)
        {
            abort();
        }
        out<<msg<<endl<<endl;
    }
}

/*!
 * \brief main() initial application's informations, language, fonts and set KCMessageHandler.
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    qInstallMessageHandler(KCMessageHandler);
    //Load QApplication Object.
    QApplication app(argc,argv);
    loadApplicationInfo();
    processArg();

    //Load Splash Screen
    KCSplashScreen splash;
    splash.setPixmap(QPixmap(":/img/image/Splash.png"));
    splash.show();
    splash.raise();
    app.processEvents();

    loadApplicationSettings();
    app.processEvents();

    //Initalize Application Palette.
    QPalette pal=app.palette();
    KCColorConfigure::getInstance()->getPalette(pal,"Global");
    app.setPalette(pal);

    //Initalize Application Style
    KreogistCuteStyle *cuteStyle=new KreogistCuteStyle;
    app.setStyle(cuteStyle);

    //Initalize and show Application MainWindow.
    MainWindow mainWindow;
    mainWindow.show();
    splash.finish(&mainWindow);

    return app.exec();
}
