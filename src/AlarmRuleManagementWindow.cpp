#include "AlarmRuleManagementWindow.h"
#include "ui_AlarmRuleManagementWindow.h"

AlarmRuleManagementWindow::AlarmRuleManagementWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AlarmRuleManagementWindow)
{
    ui->setupUi(this);
    loadRules();
}

AlarmRuleManagementWindow::~AlarmRuleManagementWindow()
{
    delete ui;
}

void AlarmRuleManagementWindow::onAddRule() { /* TODO: 新建规则 */ }
void AlarmRuleManagementWindow::onEditRule() { /* TODO: 编辑规则 */ }
void AlarmRuleManagementWindow::onDeleteRule() { /* TODO: 删除规则 */ }
void AlarmRuleManagementWindow::loadRules() { /* TODO: 加载规则 */ } 