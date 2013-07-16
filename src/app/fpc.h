#ifndef FPC_H
#define FPC_H

#include "compilerbase.h"

class fpc : public compilerBase
{
    Q_OBJECT
public:
    explicit fpc(QObject *parent = 0);
    void startCompile(const QString &filePath);
    QString path(){return fpcPath;}
    void setCompilerPath(const QString& path);
    virtual QString compilerName() { return "fpc";}
signals:
    
public slots:
    void onOutputReady();
    
protected:
    QStringList getVersionArg();
    QStringList getCompileArg(const QString &filePath);
    QStringList getcompileEnv();

private:
    void parseMessage(const QString& msg);
    static QString fpcPath;
};

#endif // FPC_H
