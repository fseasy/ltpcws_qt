##部署QT程序

###在WINDOWS上部署

####了解WINDOWS`用户帐户管理数据重定向`

详细信息，见[用户帐户管理数据重定向](https://msdn.microsoft.com/zh-cn/library/ee532451.aspx) .

对于Program File , Windows这些受写保护的文件夹，如果我们在程序中要向其中写入文件（数据），将会被重定向！这是在WINDOWS VISTA及之后引入的UCA机制导致的。具体会映射到用户目录的AppData\Local\VirtualStore\...的位置。

故如果我们采用安装包部署，则要改变之前将config文件写入到与运行程序同级的conf文件夹下的逻辑。

**具体来说，使用QT的StandardPaths::WritablePaths(QStandardPaths::GenericConfigLocation)函数能够得到在具体Windows操作系统上的可写路径（"C:/Users/<USER>/AppData/Local", "C:/ProgramData"）。**

####配置文件写入时不要设置QTextStream编码

在QT内部应该将QTextStream的编码设置为当前操作系统的默认编码。而在LTPCWS程序中，应该是使用了类似逻辑。故如果设置配置文件写入的编码，则在处理中文时会出现错误。

具体的原因，因为并没有详细看LTPCWS代码，故不能确定。但这里的确需要注意。

####寻找QT程序依赖的动态库

当我们将以Release模式编译程序发布到其他计算机时，其是缺少相应运行库的。故我们需要配上相应的运行库。

有两种方式，第一种使用静态编译，把需要的库直接链接到程序上。这样可以保证最后只有一个Exe文件，比较干净。但是需要先编译静态的QT库（默认QT的安装是动态库模式）。这个没有做。

采用第二种动态库的方式。这样需要把相应的dll放到exe能够找到的位置。放在同目录下是不必单独设置的，放在其他位置需要再main函数中设置。此外，还需要platform文件夹，里面放上qwindows.dll；可能还需要pluging等。

为了减少动态库的需求，在程序中尽量少使用多余的控件。如最后将要求webkit动态库的QWebView换成了轻量的QTextBrowser。

可以每次运行程序，来得到需要的运行库。然后从c:/path/to/qt中找到对应的dll即可。

也可以使用VS中的一个工具来分析依存关系，工具地址是：[Dependency Walker](http://www.dependencywalker.com/)

更加具体的信息，见QT的官方文档——[Qt for Windows - deploy](http://doc.qt.io/qt-5/windows-deployment.html#application-dependencies)

####加入Icon

首先程序内部，即在窗口的标题的地方，在Widget构造函数中使用setWindowIcon(QIcon(~~))即可。

除了程序内部显示，还需要在资源管理器中显示（可能也与任务栏显示Icon有关）。这个是操作系统相关的。

故这里只考虑Windows平台。

需要建立一个xxx.rc文件，里面写上：

        IDI_ICON1   ICON DISCARDABLE "path/to/icon"
        
把icon准备好。然后在项目.pro中加入

        RC_FILE += xxx.rc

编译就好了。

具体信息见官方文档:[setting application icon](http://doc.qt.io/qt-4.8/appicon.html)

####打包发布

上述把程序、需要的动态库准备好，放在一个文件夹中，这时正确的情况下，无论在任何受支持的操作系统上，只要点击程序，就能够正确的运行了。

这应该就是绿色版了。

但是我们把它打包更好。

不必使用第三方打包工具，直接使用VS2010即可。

详细信息见百度经验——[vs2010项目打包部署，Windows程序打包部署
](http://jingyan.baidu.com/album/e8cdb32b871eaa37052badd1.html?picindex=3) 

VS真的非常强大。做ICON、打包都可以做。



