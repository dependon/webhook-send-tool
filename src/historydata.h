#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include <QWidget>

namespace Ui {
class historyData;
}

class historyData : public QWidget
{
    Q_OBJECT

public:
    explicit historyData(QWidget *parent = nullptr);
    ~historyData();
    void addHistory(const QString &Time, const QString &WebPath, const QString &Data);
private:
    Ui::historyData *ui;
};

#endif // HISTORYDATA_H
