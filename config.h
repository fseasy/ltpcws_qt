#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <QObject>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QtDebug>
#include <QMessageBox>
class Config
{
   public :
    Config() ;
    enum Platform { P_WIN32 , P_LINUX32 , P_OSX , P_OTHERS} ;
    QString basicModeTrainIntro ;
    QString customModeTrainIntro ;
    QString basicModePredictIntro ;
    QString customModePredictIntro ;
    bool saveTrainConfigAndSetState(bool isCustomMode ,QString trainingSetPath ,QString devingSetPath ,
                         QString modelSavingPath , QString max_ite ,QString basicModelPath)  ;
    bool loadTrainConfig(bool isCustomMode ,QString & trainingSetPath ,QString & devingSetPath ,
                         QString & modelSavingPath , QString & max_ite ,QString & basicModelPath) ;
    void getPlatform(Platform &curPlatform ) ;
    bool getCwsExeState() ;
    QString getCurrentTrainConf() ;
    QString getCurrentCwsExePath() ;
private :
    QDir baseDir ;
    QString basicTrainConfPath ;
    QString basicTestConfPath ;
    QString customTrainConfPath ;
    QString customTestConfPath ;
    QString currentTrainConf ;

    Platform platform ;

    bool hasRightCwsExe ;
    QString basicCwsExePath ;
    QString customCwsExePath ;
    QString currentCwsExePath ;
} ;
#endif // CONFIG_H

