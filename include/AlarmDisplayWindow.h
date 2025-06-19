#ifndef ALARMDISPLAYWINDOW_H
#define ALARMDISPLAYWINDOW_H

#include <QMainWindow>

namespace Ui { class AlarmDisplayWindow; }

class AlarmDisplayWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AlarmDisplayWindow(QWidget *parent = nullptr);
    ~AlarmDisplayWindow();

private slots:
    void onFilterChanged();
    void loadAlarms();

private:
    Ui::AlarmDisplayWindow *ui;
};

#endif // ALARMDISPLAYWINDOW_H 