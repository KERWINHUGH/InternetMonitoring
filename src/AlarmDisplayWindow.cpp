#include "AlarmDisplayWindow.h"
#include "ui_AlarmDisplayWindow.h"

AlarmDisplayWindow::AlarmDisplayWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AlarmDisplayWindow)
{
    ui->setupUi(this);
    loadAlarms();
}

AlarmDisplayWindow::~AlarmDisplayWindow()
{
    delete ui;
}

void AlarmDisplayWindow::onFilterChanged() { /* TODO: 筛选条件变化 */ }
void AlarmDisplayWindow::loadAlarms() { /* TODO: 加载告警 */ } 