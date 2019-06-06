#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <Windows.h>
#include <QDebug>
#include <QSettings>

//在注册表该目录下增加新内容
#define TASKMANAGER "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**********************************************************
  不用调用dll就可以执行的全局钩子示例:
  只留下F12做为程序退出,屏蔽键盘和鼠标,测试时请注意
  ********************************************************/

HHOOK keyHook=NULL;
HHOOK mouseHook=NULL;

//键盘钩子过程
LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    //在WH_KEYBOARD_LL模式下lParam 是指向KBDLLHOOKSTRUCT类型地址
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
    //如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
    if(nCode == HC_ACTION){
        if(pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL)& 0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000){
                    qDebug() << "Ctrl+Shift+Esc";
        }else if(pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000){
            qDebug() << "Ctrl+Esc";
        }else if(pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN){
            qDebug() << "Alt+Tab";
        }else if(pkbhs->vkCode == VK_ESCAPE && pkbhs->flags &LLKHF_ALTDOWN){
            qDebug() << "Alt+Esc";
        }else if(pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN){
            qDebug() << "LWIN/RWIN";
        }else if(pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN){
            qDebug() << "Alt+F4";
        }
        if(pkbhs->vkCode == VK_F12)
        {
            void unHook();
            qApp->quit();
        }

        return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}
//鼠标钩子过程
LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam )
{

    return 1;

}
//卸载钩子
void unHook()
{
    UnhookWindowsHookEx(keyHook);
  //  UnhookWindowsHookEx(mouseHook);

}
//安装钩子,调用该函数即安装钩子
void setHook()
{
    //这两个底层钩子,不要DLL就可以全局
    //底层键盘钩子
    keyHook =SetWindowsHookEx( WH_KEYBOARD_LL,keyProc,GetModuleHandle(NULL),0);
    //底层鼠标钩子
//    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);
}

//添加注册评比Ctrl+Alt+del
void TaskManager(bool flag)
{
    QSettings *settings = new QSettings(TASKMANAGER, QSettings::NativeFormat);
    if(flag){
        settings->setValue("DisableTaskMgr", "1");
    }else{
        settings->remove("DisableTaskMgr");
    }
    delete settings;
}

void MainWindow::on_pushButton_clicked()
{
    //注册 钩子
    setHook();
    qDebug() << "0000000";
}


void MainWindow::on_pushButton_2_clicked()
{
    // 销毁 钩子
    unHook();
    qDebug() << "33333333";
}

void MainWindow::on_pushButton_3_clicked()
{
    TaskManager(true);
}

void MainWindow::on_pushButton_4_clicked()
{
    TaskManager(false);
}

void MainWindow::on_pushButton_5_clicked()
{
    if(ui->pushButton_5->text() == QString::fromLocal8Bit("隐藏任务栏")){
        task=FindWindow(L"Shell_TrayWnd",NULL);
        ShowWindow(task,SW_HIDE);
        ui->pushButton_5->setText(QString::fromLocal8Bit("显示任务栏"));
    }else{
        ShowWindow(task,SW_SHOW);
        ui->pushButton_5->setText(QString::fromLocal8Bit("隐藏任务栏"));
    }
}
