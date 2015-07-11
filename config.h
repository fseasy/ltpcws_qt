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

    QString basicModelIntro ;
    bool saveTrainConfig(bool isCustomMode ,QString trainingSetPath ,QString devingSetPath ,
                         QString modelSavingPath , QString max_ite ,QString basicModelPath)  ;
    bool loadTrainConfig(bool isCustomMode ,QString & trainingSetPath ,QString & devingSetPath ,
                         QString & modelSavingPath , QString & max_ite ,QString & basicModelPath) ;

private :
    QDir baseDir ;
    QString basicTrainConfPath ;
    QString basicTestConfPath ;
    QString customTrainConfPath ;
    QString customTestConfPath ;

} ;
#endif // CONFIG_H

