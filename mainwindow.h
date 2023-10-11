#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Score_overflow();

    void on_pushButton_3_clicked();

    void on_Quit_clicked();

    void on_Replay_clicked();

    void on_Pause_clicked();

    void on_comboBox_activated(int index);

    void on_comboBox_3_activated(int index);

    void on_comboBox_2_activated(int index);

    void on_comboBox_4_activated(int index);

    void on_lcdNumber_overflow();


    void win();
    void hideWinner();

    void on_winScore_activated(int index);

    void on_horizontalSlider_2_actionTriggered(int action);

    void on_horizontalSlider_rangeChanged(int min, int max);

    void on_horizontalSlider_2_sliderMoved(int position);

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_actionTriggered(int action);

private:
    Ui::MainWindow *ui;
    QLabel *winnerLabel1;
};
#endif // MAINWINDOW_H
