#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QSqlDatabase>
#include <QMutex>

struct dataWebHook {
    QString Time = "";
    QString WebPath = "";
    QString Data = "";
};

class historyData;
class QNetworkReply;
class QNetworkAccessManager;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void httpConnectWeChatRobot(QString url, QString msg);
    ~MainWindow();
    void initDB();
    void addHistory(const QString &Time, const QString &WebPath, const QString &Data);
    void readSettings();
    void saveSettings();

private slots:
    void on_sendBtn_clicked();
    void replyFinished(QNetworkReply *reply);
    void on_historyBtn_clicked();

private:
    QNetworkAccessManager *m_manager {nullptr};
    QNetworkReply *m_reply {nullptr};
    Ui::MainWindow *ui;

    QMap <QString, dataWebHook> m_historyInfo;

    QSqlDatabase m_db;
    QMutex *m_mutex{nullptr};
    historyData *m_dataView{nullptr};
};

#endif // MAINWINDOW_H
