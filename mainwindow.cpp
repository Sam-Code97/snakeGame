#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
//#include <QLabel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->spelplan, &Spelplan2::scoreChanged1, this, &MainWindow::on_Score_overflow);
    connect(ui->spelplan, &Spelplan2::scoreChanged2, this, &MainWindow::on_lcdNumber_overflow);
    connect(ui->spelplan, &Spelplan2::someOneWon, this, &MainWindow::win);
    connect(ui->spelplan, &Spelplan2::replayed, this, &MainWindow::hideWinner);

//    connect(ui->spelplan, &Spelplan2::scoreChanged1, this, &MainWindow::on_winScore_activated);
//    connect(ui->spelplan, &Spelplan2::scoreChanged2, this, &MainWindow::on_winScore_activated);

    winnerLabel1 = new QLabel(this);
    winnerLabel1->setMinimumWidth(600);
    winnerLabel1->setMinimumHeight(100);
    winnerLabel1->hide();


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Score_overflow()
{
    ui->Score->display(ui->spelplan->score1());
}


void MainWindow::on_pushButton_3_clicked()
{
    this->close();
}


void MainWindow::on_Quit_clicked()
{
    this->close();
}


void MainWindow::on_Replay_clicked()
{
    ui->spelplan->restartGame();
    ui->spelplan->setFocus();
}


void MainWindow::on_Pause_clicked()
{
    ui->spelplan->puseGame();
    ui->spelplan->setFocus();
}


void MainWindow::on_comboBox_activated(int index)
{

    ui->spelplan->addPlayer(index);
    ui->spelplan->setFocus();
}


void MainWindow::on_comboBox_3_activated(int index)
{

    ui->spelplan->transWalls(index);
    ui->spelplan->setFocus();

}


void MainWindow::on_comboBox_2_activated(int index)
{
    ui->spelplan->obstacles(index);
    ui->spelplan->setFocus();
}


void MainWindow::on_comboBox_4_activated(int index)
{
    ui->spelplan->speedingOn(index == 0 ? false : true);
    ui->spelplan->setFocus();
}


void MainWindow::on_lcdNumber_overflow()
{
    ui->lcdNumber->display(ui->spelplan->score2());
}



void MainWindow::win()
{
//    winnerLabel1->hide();
    int winner = ui->spelplan->winner();
    if(winner == 0){
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("<p style=\"color:red; font-size:50px; font-weight:bold;\">You Lost!</br><p style=\"color:black; font-size:20px;\">(Press an arrow two times to replay)</p>");
        winnerLabel1->show();
    }
    else if(winner == 1){
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("<p style=\"color:blue; font-size:50px; font-weight:bold;\">Blue is the winner!</br><p style=\"color:black; font-size:20px;\">(Press an arrow two times to replay)</p>");
        winnerLabel1->show();
    }
    else if(winner == 2) {
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("<p style=\"color:green; font-size:50px; font-weight:bold;\">Green is the winner!</br><p style=\"color:black; font-size:20px;\">(Press an arrow two times to replay)</p>");
        winnerLabel1->show();
    }
    else if(winner == 3) {
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("<p style=\"color:red; font-size:50px; font-weight:bold;\">You Lost Against The AI !</br><p style=\"color:black; font-size:20px;\">(Press an arrow two times to replay)</p>");
        winnerLabel1->show();
    }
    else if(winner == 4) {
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("<p style=\"color:black; font-size:50px; font-weight:bold;\">Draw!</br><p style=\"color:black; font-size:20px;\">(Press an arrow two times to replay)</p>");
        winnerLabel1->show();
    }
    else if(winner == 10) {
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("<p style=\"color:blue; font-size:50px; font-weight:bold;\">You Won!</br><p style=\"color:black; font-size:20px;\">(Press an arrow two times to replay)</p>");
        winnerLabel1->show();
    }
    else {
        winnerLabel1->setStyleSheet("background-color: rgb(255,255,255);");
        winnerLabel1->setText("what the hack!");
        winnerLabel1->show();
    }
    winnerLabel1->move(30, 55);
}

void MainWindow::hideWinner()
{
    winnerLabel1->hide();
    winnerLabel1 = new QLabel(this);
    winnerLabel1->setMinimumWidth(600);
    winnerLabel1->setMinimumHeight(100);
    winnerLabel1->hide();
}


void MainWindow::on_winScore_activated(int index)
{
    ui->spelplan->winScore(index);
    ui->spelplan->setFocus();

}


void MainWindow::on_horizontalSlider_2_actionTriggered(int action)
{
    ui->spelplan->speedingOn(false);
}


void MainWindow::on_horizontalSlider_rangeChanged(int min, int max)
{

}


void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
    ui->spelplan->changeSpeed(position);
    ui->spelplan->setFocus();
}


void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    ui->spelplan->changeLength(position);
    ui->spelplan->setFocus();
}


void MainWindow::on_horizontalSlider_actionTriggered(int action)
{
    ui->spelplan->restartedAfterSizeChange(true);
    ui->spelplan->restartGame();
    ui->spelplan->setFocus();

}

