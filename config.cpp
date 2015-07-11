#include "config.h"

Config::Config()
{
    //QString exePath = QCoreApplication::applicationDirPath() ;
    QString exePath = QDir::currentPath() ;
    QString dirName = "conf" ;
    baseDir = QDir(exePath + "/" + dirName) ; // Always use '/' as separator
    if(!baseDir.exists())
    {
        baseDir.mkpath(baseDir.absolutePath()) ;
        qDebug() << baseDir.absolutePath() ;
    }
    basicTrainConfPath = baseDir.absolutePath() + "/" + "basic_train_conf.conf" ;
    basicTestConfPath = baseDir.absolutePath() + "/" + "basic_test_conf.conf" ;
    customTrainConfPath = baseDir.absolutePath() + "/" + "custom_train_conf.conf" ;
    customTestConfPath = baseDir.absolutePath() + "/" + "custom_test_conf.conf" ;
    basicModelIntro = QObject::tr("本模块用于训练基础模型（推荐使用上述LTP分词模型作为基础模型）。\n"
                                        "选择相应的训练集语料、开发集语料以及模型保存路径，点击训练按钮即开始训练。"
                                        "训练一般耗时较长，请耐心等待。生成的模型保存在模型保存路径指定的位置。" );
}

bool Config::saveTrainConfig(bool isCustomMode ,QString trainingSetPath ,QString devingSetPath ,
                             QString modelSavingPath , QString max_ite ,QString basicModelPath)
{
   QString trainConfPath = isCustomMode ? customTrainConfPath : basicTrainConfPath ;
   QFile trainF(trainConfPath) ;
   if(!trainF.open(QIODevice::WriteOnly | QIODevice::Text))
   {
       QMessageBox::information(NULL , QObject::tr("内部程序错误") , QObject::tr("找不到训练配置文件")) ;
       return false ;
   }
   QTextStream out(&trainF) ;
   out << "[train]" << "\n"
          << "train-file = " << trainingSetPath <<"\n"
          << "holdout-file = " << devingSetPath <<"\n"
          << "algorithm = pa" <<"\n"
          << "enable-incremental-training = 1" <<"\n"
          << "max-iter = " << max_ite <<"\n"
          << "rare-feature-threshold = 0" <<"\n" ;
   if(isCustomMode)
   {
       out << "baseline-model-file = " << basicModelPath << "\n"
              << "customized-model-name = " << modelSavingPath <<"\n" ;
   }
   else
   {
       out << "model-name = " << modelSavingPath << "\n" ;
   }
   return true ;
}
bool Config::loadTrainConfig(bool isCustomMode ,QString & trainingSetPath ,QString & devingSetPath ,
                             QString & modelSavingPath , QString & max_ite ,QString & basicModelPath)
{
    return true ;
}
