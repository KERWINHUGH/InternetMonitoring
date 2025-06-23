#ifndef ALARMRULEEDITDIALOG_H
#define ALARMRULEEDITDIALOG_H

#include <QDialog>
#include <QVariantMap>

class QComboBox;
class QLineEdit;
class QTextEdit;

class AlarmRuleEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmRuleEditDialog(QWidget *parent = nullptr);
    void setRuleData(const QVariantMap &ruleData);
    QVariantMap getRuleData() const;

private:
    void setupUi();
    void loadDevices();

    QComboBox* deviceComboBox;
    QLineEdit* descriptionLineEdit;
    QTextEdit* conditionTextEdit;
    QTextEdit* actionTextEdit;
};

#endif // ALARMRULEEDITDIALOG_H 