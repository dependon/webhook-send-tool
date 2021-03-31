#include "historydata.h"
#include "ui_historydata.h"

historyData::historyData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::historyData)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("historyData"));
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

historyData::~historyData()
{
    delete ui;
}

void historyData::addHistory(const QString &Time, const QString &WebPath, const QString &Data)
{
    int index = ui->tableWidget->rowCount() + 1;
    ui->tableWidget->setRowCount(index);
    ui->tableWidget->setItem(index - 1, 0, new QTableWidgetItem(Time));
    ui->tableWidget->setItem(index - 1, 1, new QTableWidgetItem(WebPath));
    ui->tableWidget->setItem(index - 1, 2, new QTableWidgetItem(Data));
}
