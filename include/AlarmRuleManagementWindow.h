#ifndef ALARMRULEMANAGEMENTWINDOW_H
#define ALARMRULEMANAGEMENTWINDOW_H

#include <QMainWindow>

namespace Ui { class AlarmRuleManagementWindow; }

class AlarmRuleManagementWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AlarmRuleManagementWindow(QWidget *parent = nullptr);
    ~AlarmRuleManagementWindow();

private slots:
    void onAddRule();
    void onEditRule();
    void onDeleteRule();
    void loadRules();

private:
    Ui::AlarmRuleManagementWindow *ui;
};

#endif // ALARMRULEMANAGEMENTWINDOW_H 