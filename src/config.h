#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <QObject>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStandardPaths>
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
    bool savePredictConfigAndSetState(bool isCustomMode ,QString basicModelPath , QString customModelPath) ;
    bool loadPredictConfig(bool isCustomMode , QString &basicModelPath , QString &customModelPath) ;
    bool savePredictInputContent(QString &content) ;
    void getPlatform(Platform &curPlatform ) ;
    bool getCwsExeState() ;
    QString getCurrentTrainConf() ;
    QString getCurrentPredictConf() ;
    QString getCurrentCwsExePath() ;
    QString getConfInfo() ;
private :
    QDir confBaseDir ;

    QString basicTrainConfPath ;
    QString basicTestConfPath ;
    QString customTrainConfPath ;
    QString customTestConfPath ;
    QString predictInputTmpFilePath ;

    QString currentTrainConf ;
    QString currentPredictConf ;

    Platform platform ;

    bool hasRightCwsExe ;
    QString basicCwsExePath ;
    QString customCwsExePath ;
    QString currentCwsExePath ;
} ;
#endif // CONFIG_H

