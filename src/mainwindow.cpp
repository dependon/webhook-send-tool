#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QDateTime>

#include "historydata.h"
const QString CONFIG_PATH =   QDir::homePath() +
                              "/.config/webhook-send-tool/config.ini";
const QString SQL_PATH =   QDir::homePath() +
                           "/.config/webhook-send-tool/webhook.db";
const QString SQL_DIR =   QDir::homePath() +
                          "/.config/webhook-send-tool/";
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //创建一个管理器
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
    ui->setupUi(this);

    m_dataView = new historyData();
    initDB();
}

void MainWindow::httpConnectWeChatRobot(QString url, QString msg)
{
    if (msg.isEmpty()) {
        QMessageBox::about(this, "Error", QString::fromLocal8Bit("信息不能为空!"));
        return;
    }
    if (url.isEmpty()) {
        QMessageBox::about(this, "Error", QString::fromLocal8Bit("url不能为空!"));
        return;
    }

    QJsonObject Msg, text;
    //QString total = QString::fromLocal8Bit("%1").arg(msg);    //如果信息包含中文时可能需要fromLocal8Bit进行转码
    text.insert("content", msg);    //text.insert("content", total);
    Msg.insert("text", text);
    Msg.insert("msgtype", "text");
    QJsonDocument document = QJsonDocument(Msg);
    QByteArray qByteHttpData = document.toJson(QJsonDocument::Indented);   //构造出机器人能够识别的json格式
//    int index = -1;
//    do {
//        index = qByteHttpData.indexOf("\\n");
//        qByteHttpData.replace(index - 1, 3, "\n");
//    } while (index > 0);


    /*
    {
        "msgtype" : "text",
        "text" : {
            "content" : "msg的内容"
        }
    }
    */

//    QNetworkAccessManager m_pHttpMgr;
    QNetworkRequest requestInfo;
    requestInfo.setUrl(QUrl(url));
    requestInfo.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json; encoding=utf-8"));
    m_reply =  m_manager->post(requestInfo, qByteHttpData);

    QString Time = QDateTime::currentDateTime().toString();
    QString WebPath = url;
    QString Data = msg;
    addHistory(Time, WebPath, Data);


}

MainWindow::~MainWindow()
{
    if (m_dataView) {
        m_dataView->deleteLater();
        m_dataView = nullptr;
    }
    delete ui;
}

void MainWindow::initDB()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QFile file(SQL_PATH);
    if (file.exists()) {
        qDebug() << "file exists !!!";
    } else {
        qDebug() << "file no exists,mkdir file!!!.";
//        QDir::mkdir(SQL_DIR);
        QDir a;
        a.mkdir(SQL_DIR);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
    m_db.setDatabaseName(SQL_PATH);
    //打开数据库
    if (! m_db.isValid()) {
        qDebug() << "error isValid !!!";
        return;
    }
    if (!m_db.open()) {
        qDebug() << "error open !!!";
        return;
    }

    QSqlQuery querycreate;
    querycreate.exec("CREATE TABLE IF NOT EXISTS historyData(Time Text ,WebPath Text,Data Text)");

    QSqlQuery query;
    query.exec("select * from historyData");

    while (query.next()) { //一行一行遍历
        //取出当前行的内容
        //以列为单位的     //第0列
        QString Time = query.value(0).toString();
        QString WebPath = query.value(1).toString();
        QString Data = query.value(2).toString();
        dataWebHook hook;
        hook.Time = Time;
        hook.WebPath = WebPath;
        hook.Data = Data;
        m_historyInfo.insert(Time, hook);
        if (m_dataView) {
            m_dataView->addHistory(Time, WebPath, Data);
        }
    }
}

void MainWindow::addHistory(const QString &Time, const QString &WebPath, const QString &Data)
{
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        QString insert_sql = "insert into historyData values (?, ?, ?)";
        query.prepare(insert_sql);
        query.addBindValue(Time);
        query.addBindValue(WebPath);
        query.addBindValue(Data);
        query.setForwardOnly(true);
        query.exec();

        dataWebHook hook;
        hook.Time = Time;
        hook.WebPath = WebPath;
        hook.Data = Data;
        m_historyInfo.insert(Time, hook);
        if (m_dataView) {
            m_dataView->addHistory(Time, WebPath, Data);
        }
    }
}

void MainWindow::readSettings()
{
//    QSettings settings(CONFIG_PATH, QSettings::IniFormat);
}

void MainWindow::saveSettings()
{

}

void MainWindow::on_sendBtn_clicked()
{
    httpConnectWeChatRobot(ui->urlEdit->text(), ui->dataEdit->toPlainText());
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
    QByteArray responseText = reply->readAll();
    reply->deleteLater();
    reply = nullptr;
}

void MainWindow::on_historyBtn_clicked()
{
    if (m_dataView) {
        m_dataView->show();
        m_dataView->activateWindow();
    }
}
