#ifndef TEMP_H
#define TEMP_H

#include "spelplan2.h"
#include <Qtimer>
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <chrono>
#include <thread>
#include <QDebug>
#include <ctime>
#include <QThread>

Spelplan2::Spelplan2(QWidget *parent)
    : QWidget{parent}
{

    m_speed = 100;
    m_ptimer = new QTimer(this);
    connect(m_ptimer, &QTimer::timeout, this, &Spelplan2::uppdateraMasken);
    m_ptimer->start(m_speed);

    setFocus();
    this->setFocusPolicy(Qt::StrongFocus);
    restartGame();
    m_twoPl = false;
    update();


}

int Spelplan2::score1()
{
    return m_score1;
}

void Spelplan2::restartGame()
{
    m_masken1.clear();
    m_masken2.clear();
    m_obstacle.clear();

    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;
    m_rkRiktning1 = RK(0,0);
    m_rkRiktning2 = RK(0,0);
    srand(time(NULL));
    if(m_twoPl){
        m_rkHuvud1 = RK(rows/4, cols/2);
        m_masken1.push_front(m_rkHuvud1);
        m_rkHuvud2 = RK((rows+3)/2, cols/2);
        m_masken2.push_front(m_rkHuvud2);
    }
    else{
        m_rkHuvud1 = RK((rows/2), (cols/2));
        m_masken1.push_front(m_rkHuvud1);
    }
    m_rkFruit = RK((rand()%(rows-2))+1, (rand()%(cols-2))+1);
    m_lengh1 = 10;
    m_lengh2 = 10;
    m_collision1 = false;
    m_collision2 = false;
    m_startedMoving = false;
    m_eatedFruit = false;
    m_obsLvl = false;
    m_score1 = 0;
    m_speed = 100;
    emit scoreChanged1();

    update();

}
void Spelplan2::puseGame(){
    m_rkRiktning1 = RK(0,0);
    m_rkRiktning2 = RK(0,0);
    m_startedMoving = false;
    update();
}


void Spelplan2::addPlayer(bool add)
{
    m_twoPl = add;
    //    restartGame();
}

void Spelplan2::noTransWall()
{
    m_transWall = false;
    //    if(m_twoPl){
    //        m_masken1.clear();
    //        m_masken2.clear();
    //        m_obstacle.clear();

    //        int rows = this->height() / m_cellstorlek;
    //        int cols = this->width() / m_cellstorlek;
    //        m_rkRiktning1 = RK(0,0);
    //        m_rkRiktning2 = RK(0,0);
    //        srand(time(NULL));
    //        if(m_twoPl){
    //            m_rkHuvud1 = RK(rows/4, cols/2);
    //            m_masken1.push_front(m_rkHuvud1);
    //            m_rkHuvud2 = RK((rows+3)/2, cols/2);
    //            m_masken2.push_front(m_rkHuvud2);
    //        }
    //        else{
    //            m_rkHuvud1 = RK((rows/2), (cols/2));
    //            m_masken1.push_front(m_rkHuvud1);
    //        }
    //        m_rkFruit = RK((rand()%(rows-2))+1, (rand()%(cols-2))+1);
    //        m_lengh1 = 10;
    //        m_lengh2 = 10;
    //        m_collision1 = false;
    //        m_collision2 = false;
    //        m_startedMoving = false;
    //        m_eatedFruit = false;
    //        m_finsObs = false;
    //        m_score = 0;
    //        m_speed = 100;
    //        emit scoreChanged();

    //        update();

    //    }
}

void Spelplan2::obstacles(int lvl)
{
    if(lvl == 0){
        m_obsLvl = false;
        return;
    }
    else m_obsLvl = true;
    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;
    srand(time(NULL));

    if(lvl == 1){
        m_obstacle.clear();
        for(int i=0; i<(rows/10); i++){
            m_obstacle.push_back(RK((rand()%rows-1)+1, (rand()%cols-1)+1));
        }
    }
    else if(lvl == 2){
        m_obstacle.clear();
        for(int i=0; i<(rows/5); i++){
            m_obstacle.push_back(RK((rand()%rows-1)+1, (rand()%cols-1)+1));
        }    }
    else if(lvl == 3){
        m_obstacle.clear();
        for(int i=0; i<(rows/2); i++){
            m_obstacle.push_back(RK((rand()%rows-1)+1, (rand()%cols-1)+1));
        }    }

}

void Spelplan2::speedingOn(bool spd)
{
    m_speedON = spd;
}

void Spelplan2::checkCollision()
{

}

void Spelplan2::uppdateraMasken()
{
    if(m_collision1 || m_collision2){
        update();
        return;
    }


    m_rkHuvud1.m_r += m_rkRiktning1.m_r;
    m_rkHuvud1.m_k += m_rkRiktning1.m_k;
    if(m_twoPl){
        m_rkHuvud2.m_r += m_rkRiktning2.m_r;
        m_rkHuvud2.m_k += m_rkRiktning2.m_k;
    }
    
    checkEatFruit();

    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;

    if(m_startedMoving){
        if(m_twoPl){
            m_masken2.push_front(m_rkHuvud2);
            if(m_masken2.size() > m_lengh2)
                m_masken2.pop_back();
            if (!m_masken2.empty()){
                for(auto part = std::next(m_masken2.begin()); part != m_masken2.end(); ++part){
                    if(m_rkHuvud2.m_k == part->m_k && m_rkHuvud2.m_r == part->m_r)
                    {
                        m_rkRiktning2 = RK(0,0);
                        m_rkRiktning1 = RK(0,0);
                        m_collision2 = true;
                        return;
                    }
                    if(m_rkHuvud1.m_k== part->m_k && m_rkHuvud1.m_r == part->m_r){
                        m_rkRiktning2 = RK(0,0);
                        m_rkRiktning1 = RK(0,0);
                        m_collision1 = true;
                        return;
                    }
                }
            }
            if (!m_obstacle.empty()){
                for(auto part = std::next(m_obstacle.begin()); part != m_obstacle.end(); ++part){
                    if(m_rkHuvud2.m_k == part->m_k && m_rkHuvud2.m_r == part->m_r)
                    {
                        m_rkRiktning2 = RK(0,0);
                        m_rkRiktning1 = RK(0,0);
                        m_collision2 = true;
                        update();
                        return;
                    }
                }
            }

            if(m_transWall){
                if(m_rkHuvud2.m_r >= rows)
                    m_rkHuvud2.m_r = 1;
                else if(m_rkHuvud2.m_r < 1) m_rkHuvud2.m_r = rows-1;
                else if(m_rkHuvud2.m_k >= cols) m_rkHuvud2.m_k = 1;
                else if(m_rkHuvud2.m_k < 1) m_rkHuvud2.m_k = cols-1;
            }
            else if(m_transWall==false){
                if(m_rkHuvud2.m_r >= rows) m_collision2 = true;
                else if(m_rkHuvud2.m_r < 1) m_collision2 = true;
                else if(m_rkHuvud2.m_k >= cols) m_collision2 = true;
                else if(m_rkHuvud2.m_k < 1) m_collision2 = true;
            }
        }
        m_masken1.push_front(m_rkHuvud1);
        if(m_masken1.size() > m_lengh1)
            m_masken1.pop_back();

        if (!m_masken1.empty()){
            for(auto part = std::next(m_masken1.begin()); part != m_masken1.end(); ++part){
                if(m_rkHuvud1.m_k == part->m_k && m_rkHuvud1.m_r == part->m_r)
                {
                    m_rkRiktning1 = RK(0,0);
                    m_rkRiktning2 = RK(0,0);
                    m_collision1 = true;
                    return;
                }
                if(m_rkHuvud2.m_k == part->m_k && m_rkHuvud2.m_r == part->m_r){
                    m_rkRiktning2 = RK(0,0);
                    m_rkRiktning1 = RK(0,0);
                    m_collision2 = true;
                    return;
                }
            }
        }
        if (!m_obstacle.empty()){
            for(auto part = std::next(m_obstacle.begin()); part != m_obstacle.end(); ++part){
                if(m_rkHuvud1.m_k == part->m_k && m_rkHuvud1.m_r == part->m_r)
                {
                    m_rkRiktning1 = RK(0,0);
                    m_rkRiktning2 = RK(0,0);
                    m_collision1 = true;
                    update();
                    return;
                }
            }
        }

        if(m_transWall){
            if(m_rkHuvud1.m_r >= rows)
                m_rkHuvud1.m_r = 1;
            else if(m_rkHuvud1.m_r < 1) m_rkHuvud1.m_r = rows-1;
            else if(m_rkHuvud1.m_k >= cols) m_rkHuvud1.m_k = 1;
            else if(m_rkHuvud1.m_k < 1) m_rkHuvud1.m_k = cols-1;
        }
        else if(m_transWall==false){
            if(m_rkHuvud1.m_r >= rows) m_collision1 = true;
            else if(m_rkHuvud1.m_r < 1) m_collision1 = true;
            else if(m_rkHuvud1.m_k >= cols) m_collision1 = true;
            else if(m_rkHuvud1.m_k < 1) m_collision1 = true;
        }
    }
    update();
}


void Spelplan2::checkEatFruit()
{
    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;

    if(m_rkHuvud1.m_k == m_rkFruit.m_k  && m_rkHuvud1.m_r == m_rkFruit.m_r){
        m_eatedFruit = true;
        m_score1++;
        m_lengh1++;
        m_rkFruit = RK((rand()%(rows-2))+1, (rand()%(cols-2))+1);
        if(m_speedON){
            m_speed--;
            m_ptimer->start(m_speed);
        }
        emit scoreChanged1();
    }
    if(m_rkHuvud2.m_k == m_rkFruit.m_k  && m_rkHuvud2.m_r == m_rkFruit.m_r){
        m_eatedFruit = true;
        m_score1++;
        m_lengh2++;
        m_rkFruit = RK((rand()%(rows-2))+1, (rand()%(cols-2))+1);
        if(m_speedON){
            m_speed--;
            m_ptimer->start(m_speed);
        }
        emit scoreChanged1();
    }
    update();

}

void Spelplan2::keyPressEvent(QKeyEvent *event)
{
    int k = event->key();

    m_rkRiktningBefor1 = m_rkRiktning1;
    if(k==Qt::Key_Down && m_rkRiktning1.m_r != -1 && m_rkRiktningBefor1.m_r != -1) m_rkRiktning1 = RK(1,0);
    if(k==Qt::Key_Up && m_rkRiktning1.m_r != 1 && m_rkRiktningBefor1.m_r != 1) m_rkRiktning1 = RK(-1, 0);
    if(k==Qt::Key_Left && m_rkRiktning1.m_k != 1 && m_rkRiktningBefor1.m_k != 1) m_rkRiktning1 = RK(0, -1);
    if(k==Qt::Key_Right && m_rkRiktning1.m_k != -1 && m_rkRiktningBefor1.m_k != -1) m_rkRiktning1 = RK(0, 1);

    m_rkRiktningBefor2 = m_rkRiktning2;
    if(m_twoPl){
        if(k==Qt::Key_S && m_rkRiktning2.m_r != -1 && m_rkRiktningBefor2.m_r != -1) m_rkRiktning2 = RK(1,0);
        if(k==Qt::Key_W && m_rkRiktning2.m_r != 1 && m_rkRiktningBefor2.m_r != 1) m_rkRiktning2 = RK(-1, 0);
        if(k==Qt::Key_A && m_rkRiktning2.m_k != 1 && m_rkRiktningBefor2.m_k != 1) m_rkRiktning2 = RK(0, -1);
        if(k==Qt::Key_D && m_rkRiktning2.m_k != -1 && m_rkRiktningBefor2.m_k != -1) m_rkRiktning2 = RK(0, 1);
    }
    m_startedMoving = true;

}

void Spelplan2::paintEvent(QPaintEvent *event)
{
    QColor snakeColor1 = m_collision1 ? Qt::red : Qt::black;
    QColor snakeColor2 = m_collision1 ? Qt::red : Qt::darkGreen;

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);    // white background

    painter.fillRect(m_rkFruit.m_k*m_cellstorlek,   // fruit paint
                     m_rkFruit.m_r*m_cellstorlek,
                     m_cellstorlek,
                     m_cellstorlek,
                     Qt::blue);

    QPen pen1;
    painter.setPen(pen1);
    painter.setBrush(snakeColor1);
    painter.drawEllipse(m_masken1.front().m_k*m_cellstorlek,
                        m_masken1.front().m_r*m_cellstorlek,
                        m_cellstorlek,
                        m_cellstorlek);

    for(unsigned int i=0; i<m_masken1.size(); i++){
        RK rk = m_masken1[i];
        painter.drawEllipse(rk.m_k*m_cellstorlek,
                            rk.m_r*m_cellstorlek,
                            m_cellstorlek,
                            m_cellstorlek);
    }
    if(m_twoPl){
        QPen pen2;
        painter.setPen(pen2);
        painter.setBrush(snakeColor2);
        painter.drawEllipse(m_masken2.front().m_k*m_cellstorlek,
                            m_masken2.front().m_r*m_cellstorlek,
                            m_cellstorlek,
                            m_cellstorlek);

        for(unsigned int i=0; i<m_masken2.size(); i++){
            RK rk = m_masken2[i];
            painter.drawEllipse(rk.m_k*m_cellstorlek,
                                rk.m_r*m_cellstorlek,
                                m_cellstorlek,
                                m_cellstorlek);
        }
    }
    
    if(m_obsLvl){
        for(unsigned int i=0; i<m_obstacle.size(); i++){
            RK rk = m_obstacle[i];
            painter.fillRect(rk.m_k*m_cellstorlek,
                             rk.m_r*m_cellstorlek,
                             m_cellstorlek,
                             m_cellstorlek,
                             Qt::darkRed);
        }
    }

}



#endif // TEMP_H
