#include "spelplan2.h"
#include <Qtimer>
#include <QPainter>
#include <QKeyEvent>
#include <QPaintEvent>
#include <chrono>
#include <optional>
#include <thread>
#include <QDebug>
#include <ctime>
#include <QThread>
#include <vector>

Spelplan2::Spelplan2(QWidget *parent)
    : QWidget{parent}
{

    m_speed = 100;
    m_ptimer = new QTimer(this);
    connect(m_ptimer, &QTimer::timeout, this, &Spelplan2::uppdateraMasken);
    m_ptimer->start(m_speed);

    setFocus();
    this->setFocusPolicy(Qt::StrongFocus);

    m_rkRiktning1 = RK(0,0);
    srand(time(NULL));
    m_rkHuvud1 = RK(10,10);
    m_masken1.push_front(m_rkHuvud1);
    m_rkFruit = RK((rand()%24)+1, (rand()%24)+1);
    m_lengh1 = 5;
    m_lengh2 = 5;
    m_collision1 = false;
    m_collision2 = false;
    m_startedMoving = false;
    m_eatedFruit = false;
    m_score1 = 0;
    m_score2 = 0;
    m_twoPl = false;
    m_datorSpelar = false;
    m_transWall = true;
    m_obsLvl = false;
//    m_pcCalculated = false;
    m_timesEated = 0;
    m_tapA = 0;
    m_tapB = 0;
    m_obsLvl = 0;
    m_winscore = 10;
    m_oldSpeed = m_speed;
    m_restartedAfterSizeChange = false;


    update();

}

int Spelplan2::score1()
{
    return m_score1;
}

int Spelplan2::score2()
{
    return m_score2;
}

void Spelplan2::restartGame()
{
    emit replayed();
    m_masken1.clear();
    m_masken2.clear();
    m_obstacle.clear();
    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;
    if(m_twoPl){
        m_rkHuvud2 = RK(((rows*3)/4), (cols/2));
        m_masken2.push_front(m_rkHuvud2);

        m_collision2 = false;
        if(m_datorSpelar){
            if(!m_rkNext.empty())m_rkNext.clear();
        }
    }
    m_rkRiktning1 = RK(0,0);
    if(m_datorSpelar) m_rkRiktning2 = RK(0,0);

    srand(time(NULL));
    m_rkHuvud1 = RK((rows/4), (cols/2));
    m_masken1.push_front(m_rkHuvud1);
    m_rkFruit = RK((rand()%(rows-2))+1, (rand()%(cols-2))+1);
    if(!m_restartedAfterSizeChange){
        m_lengh1 = 5;
        m_lengh2 = 5;
        m_restartedAfterSizeChange = false;
    }
    m_collision1 = false;
    m_collision2 = false;
    m_startedMoving = false;
    m_eatedFruit = false;
//    m_pcCalculated = false;

    m_score1 = 0;
    m_score2 = 0;
    m_speed = 100;
    m_tapA = 0;
    m_tapB = 0;
    m_timesEated = 0;
    m_oldSpeed = m_speed;

    obstacles(m_obsLvl);
    emit scoreChanged1();
    emit scoreChanged2();

    update();

}
void Spelplan2::puseGame(){
    m_rkRiktning1 = RK(0,0);
    m_rkRiktning2 = RK(0,0);
    m_startedMoving = false;
    update();
}



bool Spelplan2::m_foundShortestPathToGoalFIFO(RK huvud)
{
    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;

    std::vector<std::vector<RK>> rkPrev( rows, std::vector<RK>(cols, RK()));
    std::vector<RK> rkVisited {};
    RK startPoint = m_rkHuvud2;
    std::deque<RK> rkTodo;
    rkTodo.push_front(m_rkHuvud2);
    rkVisited.push_back(m_rkHuvud2);

    int timeout = 10000;



    if(!m_eatedFruit && timeout>0){
        while(rkTodo.front() != m_rkFruit)
        {
            RK front = rkTodo.front();
            std::vector<Spelplan2::RK> neighbors = {
                {front.m_r+1, front.m_k},
                {front.m_r-1, front.m_k},
                {front.m_r, front.m_k+1},
                {front.m_r, front.m_k-1}
            };


            for(int i=0; i<4; i++)
            {
                bool opponent = false;
                bool obstacle = false;
                bool wall = false;
                bool visited = false;
                bool ownBody = false;

                for(RK &opPart : m_masken1){
                    if(neighbors[i] == opPart){
                        opponent = true;
                        break;
                    }
                }
                for(RK bodyPart : m_masken2){
                    if(neighbors[i] == bodyPart){
                        ownBody = true;
                        break;
                    }
                }
                for(RK obs : m_obstacle){
                    if(neighbors[i] == obs){
                        obstacle = true;
                        break;
                    }
                }
                if(m_transWall)
                    if(neighbors[i].m_r >= rows ||
                        neighbors[i].m_r <= 0 ||
                        neighbors[i].m_k >= cols ||
                        neighbors[i].m_k <= 0)
                        wall = true;

                for(RK old : rkVisited){
                    if(neighbors[i] == old){
                        visited = true;
                        break;
                    }
                }
                if(m_score2 > 35)
                    qDebug() <<" !!!!!!!! ";

                if(!opponent && !obstacle && !wall && !visited && !ownBody &&
                    neighbors[i] != m_rkRiktningBefor2)
                {
                    rkTodo.push_back(neighbors[i]);
                    rkVisited.push_back(rkTodo.back());
                    rkPrev[rkTodo.back().m_r][rkTodo.back().m_k] = RK(front.m_r, front.m_k);
                }
            }

            if(rkTodo.size()<2) rkTodo.push_back(m_rkHuvud2);

            if(timeout <= 0 ){             //uncomment to make AI stuped but doesnt keep on looping if there is no way to fruit
                break;
            }
            timeout--;
            rkTodo.pop_front();
        }
        if(timeout <= 0 ){             //uncomment to make AI stuped but doesnt keep on looping if there is no way to fruit
            return false;
        }
        if(!rkPrev.empty()){
            RK rk = rkPrev[rkTodo.front().m_r][rkTodo.front().m_k];
            std::vector<std::vector<RK>> rkNext( rows, std::vector<RK>(cols, RK()));
            rkNext[rk.m_r][rk.m_k] = rkTodo.front();

            while(rk != startPoint){
                RK n = rk;
                rk = rkPrev[rk.m_r][rk.m_k];
                rkNext[rk.m_r][rk.m_k] = n;
            }
            m_rkNext.clear();
            m_rkNext = rkNext;
        }
        update();
        return false;
    }

    return true;
}

void Spelplan2::addPlayer(int i)
{
    if(i == 0){
        m_twoPl = false;
        m_datorSpelar = false;
        restartGame();
    }
    else if (i == 1){
        m_twoPl = true;
        m_datorSpelar = false;
        restartGame();
    }
    else if (i == 2){
        m_datorSpelar = true;
        m_twoPl = true;
        restartGame();
    }
}

void Spelplan2::transWalls(int i)
{
    if(i == 0)
        m_transWall = true;
    else m_transWall = false;
}

void Spelplan2::obstacles(int lvl)
{
    m_obsLvl = lvl;
    if(lvl == 0){
        return;
    }
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
        }
    }
    else if(lvl == 4){
        m_obstacle.clear();
        for(int i=0; i<(rows); i++){
            m_obstacle.push_back(RK((rand()%rows-1)+1, (rand()%cols-1)+1));
        }
    }
}

void Spelplan2::speedingOn(bool spd)
{
    puseGame();
    m_speedON = spd;
}

void Spelplan2::checkCollision()
{
    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;

    if(m_startedMoving){
        if(m_twoPl){
            if(m_rkHuvud1 == m_rkHuvud2){
                m_collision1 = true;
                m_collision2 = true;
                return;
            }
            m_masken2.push_front(m_rkHuvud2);
            if(m_masken2.size() > m_lengh2)
                m_masken2.pop_back();
            if (!m_masken2.empty()){
                for(auto part1 = std::next(m_masken2.begin()); part1 != m_masken2.end(); ++part1){
                    if(m_rkHuvud2.m_k == part1->m_k && m_rkHuvud2.m_r == part1->m_r)
                    {
                        m_rkRiktning2 = RK(0,0);
                        m_collision2 = true;
                        return;
                    }
                    if(part1->m_k == m_rkHuvud1.m_k && part1->m_r == m_rkHuvud1.m_r){
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
                        m_collision2 = true;
//                        update();
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
            for(auto part2 = std::next(m_masken1.begin()); part2 != m_masken1.end(); ++part2){
                if(m_rkHuvud1.m_k == part2->m_k &&
                    m_rkHuvud1.m_r == part2->m_r)
                {
                    m_rkRiktning1 = RK(0,0);
                    m_collision1 = true;
                    return;
                }
                if(part2->m_k == m_rkHuvud2.m_k && part2->m_r == m_rkHuvud2.m_r){
                    m_rkRiktning2 = RK(0,0);
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
                    m_collision1 = true;
//                    update();
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
        if(m_score1 >= m_winscore){
            m_collision2 = true;
            m_rkRiktning2 = RK(0,0);
            return;
        }
        else if(m_score2 >= m_winscore){
            m_collision1 = true;
            m_rkRiktning1 = RK(0,0);
            return;
        }
    }
}

void Spelplan2::checkWinner()
{
    if(!m_twoPl && !m_collision1)
        m_winner = 10;
    else if(m_collision1 && !m_twoPl)
        m_winner = 0;
    else if((!m_collision1 && m_collision2)){
        m_winner = 1;
    }
    else if((!m_collision2 && m_collision1 && !m_datorSpelar)){
        m_winner = 2;
    }
    else if((!m_collision2 && m_collision1 && m_datorSpelar)){
        m_winner = 3;
    }
    else if(m_collision1 && m_collision2)
        m_winner = 4;
    else return;
}

int Spelplan2::winner()
{
    return m_winner;
}

int Spelplan2::winScore(int &score)
{
    if(score == 0)
        m_winscore = 10;
    else if(score == 1)
        m_winscore = 20;
    else if(score == 2)
        m_winscore = 30;
    else return 0;
    return m_winscore;
}

void Spelplan2::changeSpeed(int speed)
{
    m_speed = m_oldSpeed;
    m_speed -= speed;
    if(m_speed < 30) m_speed = 30;
    else if(m_speed > 100) m_speed = 100;
    m_ptimer->start(m_speed);
}

void Spelplan2::changeLength(int length)
{
    m_lengh1 = length;
    m_lengh2 = length;
    update();
}

void Spelplan2::restartedAfterSizeChange(bool b)
{
    m_restartedAfterSizeChange = b;
}


void Spelplan2::uppdateraMasken()
{
    if(m_collision1 || m_collision2){
        checkWinner();
        emit someOneWon();
        update();
        return;
    }

    if(m_datorSpelar && m_startedMoving){
        m_foundShortestPathToGoalFIFO(m_rkHuvud2);
        if(!m_rkNext.empty())
            m_rkRiktning2 = (m_rkNext[m_rkHuvud2.m_r][m_rkHuvud2.m_k] - m_rkHuvud2);
    }

    m_rkHuvud1.m_r += m_rkRiktning1.m_r;
    m_rkHuvud1.m_k += m_rkRiktning1.m_k;
    if(m_twoPl){
        m_rkHuvud2.m_r += m_rkRiktning2.m_r;
        m_rkHuvud2.m_k += m_rkRiktning2.m_k;
    }

    checkEatFruit();
    checkCollision();

    update();
}

void Spelplan2::fruitPosition()
{
    int rows = this->height() / m_cellstorlek;
    int cols = this->width() / m_cellstorlek;
    bool onObs = false;
    bool onMask1 = false;
    bool onMask2 = false;

    RK position = RK((rand()%(rows-2))+1, (rand()%(cols-2))+1);
    if(!m_obstacle.empty()){
        for(auto part = std::next(m_obstacle.begin()); part != m_obstacle.end(); part++){
            if(position.m_r == part->m_r && position.m_k == part->m_k){
                onObs = true;
            }
        }
        if(!m_masken1.empty()){
            for(auto part = std::next(m_masken1.begin()); part != m_masken1.end(); part++){
                if(position.m_r == part->m_r && position.m_k == part->m_k){
                    onMask1 = true;
                }
            }
        }
        if(!m_masken2.empty()){
            for(auto part = std::next(m_masken2.begin()); part != m_masken2.end(); part++){
                if(position.m_r == part->m_r && position.m_k == part->m_k){
                    onMask2 = true;
                }
            }
        }
    }
    if(onObs || onMask1 || onMask2 ) fruitPosition();
    else m_rkFruit = position;
}


void Spelplan2::checkEatFruit()
{
    if(m_rkHuvud1.m_k == m_rkFruit.m_k  && m_rkHuvud1.m_r == m_rkFruit.m_r){
        m_eatedFruit = true;
        m_score1++;
        m_lengh1++;
        emit scoreChanged1();
    }
    if(m_twoPl && m_rkHuvud2.m_k == m_rkFruit.m_k  && m_rkHuvud2.m_r == m_rkFruit.m_r){
        m_eatedFruit = true;
        m_score2++;
        m_lengh2++;
        emit scoreChanged2();
    }

    if(m_eatedFruit){
        m_timesEated++;
        if(m_timesEated >= 2) m_timesEated = 0;

        fruitPosition();

        if(m_speedON && m_speed > 30 && m_timesEated==0){
            m_speed--;
            m_ptimer->start(m_speed);
        }
        m_rkNext.clear();
//        m_pcCalculated = false;
    }
    m_eatedFruit = false;
    update();

}

void Spelplan2::keyPressEvent(QKeyEvent *event)
{
    int k = event->key();
    static int count = 0;
    m_rkRiktningBefor2 = m_rkRiktning2;
    if(m_twoPl && !m_datorSpelar){
        if(m_startedMoving==false){
            for(int i=0; i<5; i++){
                m_rkRiktning2 = RK(0,-1);
                m_rkRiktning1 = RK(0,1);
                if(i>4)
                    m_startedMoving = true;
                update();
            }
        }
        if(k==Qt::Key_S && m_rkRiktning2.m_r != -1 && m_rkRiktningBefor2.m_r != -1) {m_rkRiktning2 = RK(1,0); m_startedMoving = true; m_tapB++;}
        if(k==Qt::Key_W && m_rkRiktning2.m_r != 1 && m_rkRiktningBefor2.m_r != 1) {m_rkRiktning2 = RK(-1, 0); m_startedMoving = true; m_tapB++;}
        if(k==Qt::Key_A && m_rkRiktning2.m_k != 1 && m_rkRiktningBefor2.m_k != 1) {m_rkRiktning2 = RK(0, -1); m_startedMoving = true; m_tapB++;}
        if(k==Qt::Key_D && m_rkRiktning2.m_k != -1 && m_rkRiktningBefor2.m_k != -1) {m_rkRiktning2 = RK(0, 1); m_startedMoving = true; m_tapB++;}
    }
//    else if(m_twoPl && m_datorSpelar){
////        m_foundShortestPathToGoalFIFO(m_rkHuvud2);
//    }
    std::vector<int> nums {1,1,2};
//    t.erase(t.begin()+1);
    for(int i=1; i<nums.size(); i++){
        if(nums[i-1] == nums[i]){
            nums.erase(nums.begin() + i);
            i--;
        }
        //auto it = std::remove(nums.begin(), nums.end(), (i-1));
    }
    for(auto &num : nums){
        qDebug()<<num << " ";
    }
    m_rkRiktningBefor1 = m_rkRiktning1;
    if(k==Qt::Key_Down && m_rkRiktning1.m_r != -1 && m_rkRiktningBefor1.m_r != -1) {m_rkRiktning1 = RK(1,0); m_startedMoving = true; m_tapA++;}
    if(k==Qt::Key_Up && m_rkRiktning1.m_r != 1 && m_rkRiktningBefor1.m_r != 1) {m_rkRiktning1 = RK(-1, 0); m_startedMoving = true; m_tapA++;}
    if(k==Qt::Key_Left && m_rkRiktning1.m_k != 1 && m_rkRiktningBefor1.m_k != 1) {m_rkRiktning1 = RK(0, -1); m_startedMoving = true; m_tapA++;}
    if(k==Qt::Key_Right && m_rkRiktning1.m_k != -1 && m_rkRiktningBefor1.m_k != -1) {m_rkRiktning1 = RK(0, 1); m_startedMoving = true; m_tapA++;}

    if(m_collision1 || m_collision2){
        count++;
        if(count > 1){
            count = 0;
            restartGame();
        }
    }
    /*
    int k = event->key();
    static int count = 0;
    if(m_twoPl && !m_datorSpelar){
        m_rkRiktningBefor2 = m_tempRikt2;
//        if(m_startedMoving==false){
//            for(int i=0; i<5; i++){
//                m_rkRiktning2 = RK(0,-1);
//                m_rkRiktning1 = RK(0,1);
//                if(i>3)
//                    m_startedMoving = true;
//                update();
//            }
//        }
        if(k==Qt::Key_S && m_rkRiktning2.m_r != -1 && m_rkRiktningBefor2.m_r != -1) {
            m_tempRikt2 = RK(1,0); m_tapB++;}
        else if(k==Qt::Key_W && m_rkRiktning2.m_r != 1 && m_rkRiktningBefor2.m_r != 1) {
            m_tempRikt2 = RK(-1, 0); m_tapB++;}
        else if(k==Qt::Key_A && m_rkRiktning2.m_k != 1 && m_rkRiktningBefor2.m_k != 1) {
            m_tempRikt2 = RK(0, -1); m_tapB++;}
        else if(k==Qt::Key_D && m_rkRiktning2.m_k != -1 && m_rkRiktningBefor2.m_k != -1) {
            m_tempRikt2 = RK(0, 1); m_tapB++;}
//        update();
    }
//    else if(m_twoPl && m_datorSpelar){
////        m_foundShortestPathToGoalFIFO(m_rkHuvud2);
//    }
    m_rkRiktningBefor1 = m_tempRikt1;
    if(k==Qt::Key_Down && m_rkRiktning1.m_r != -1 && m_rkRiktningBefor1.m_r != -1) {
        m_tempRikt1 = RK(1,0); ; m_tapA++;}
    else if(k==Qt::Key_Up && m_rkRiktning1.m_r != 1 && m_rkRiktningBefor1.m_r != 1) {
        m_tempRikt1 = RK(-1, 0); m_tapA++;}
    else if(k==Qt::Key_Left && m_rkRiktning1.m_k != 1 && m_rkRiktningBefor1.m_k != 1) {
        m_tempRikt1 = RK(0, -1); m_tapA++;}
    else if(k==Qt::Key_Right && m_rkRiktning1.m_k != -1 && m_rkRiktningBefor1.m_k != -1) {
        m_tempRikt1 = RK(0, 1); m_tapA++;}

    if(m_tapA>0){
        if(!m_twoPl) m_startedMoving = true;
        else if(m_tapB>0){
            m_startedMoving = true;
            m_rkRiktning1 = m_tempRikt1;
            m_rkRiktning2 = m_tempRikt2;
        }
    }

    if(m_collision1 || m_collision2){
        count++;
        if(count > 1){
            count = 0;
            restartGame();
        }
    }
*/
}

void Spelplan2::paintEvent(QPaintEvent *event)
{
    QColor snakeColor1 = m_collision1 ? Qt::red : Qt::blue;
    QColor snakeColor2 = m_collision2 ? Qt::red : Qt::darkGreen;
    QColor fruitColor = {rand()%100, (rand()%200)+50, (rand()%200)+50};

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);    // white background

//    painter.fillRect(m_rkFruit.m_k*m_cellstorlek,   // fruit paint
//                     m_rkFruit.m_r*m_cellstorlek,
//                     m_cellstorlek,
//                     m_cellstorlek,
//                     fruitColor);
    QPolygon hexagon;

    int startX = m_rkFruit.m_k*m_cellstorlek;
    int startY = m_rkFruit.m_r*m_cellstorlek;

    int hexWidth = m_cellstorlek;
    int hexHeight = m_cellstorlek;

    hexagon << QPoint(startX + hexWidth / 2, startY)
            << QPoint(startX + hexWidth, startY + hexHeight / 4)
            << QPoint(startX + hexWidth, startY + hexHeight * 3 / 4)
            << QPoint(startX + hexWidth / 2, startY + hexHeight)
            << QPoint(startX, startY + hexHeight * 3 / 4)
            << QPoint(startX, startY + hexHeight / 4);

    QRadialGradient gradient(startX + hexWidth / 2,
                             startY + hexHeight / 2,
                             std::sqrt(hexWidth * hexWidth + hexHeight * hexHeight) / 2,
                             startX + hexWidth / 2,
                             startY + hexHeight / 2);

    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, fruitColor);

    painter.setBrush(gradient);
    painter.drawPolygon(hexagon);



    QPen pen1;
    painter.setPen(pen1);
    painter.setBrush(snakeColor1);
    painter.drawEllipse(m_masken1.front().m_k*m_cellstorlek,
                     m_masken1.front().m_r*m_cellstorlek,
                     m_cellstorlek,
                     m_cellstorlek);

//    for(unsigned int i=0; i<m_masken1.size(); i++){
//        RK rk = m_masken1[i];
//        painter.drawEllipse(rk.m_k*m_cellstorlek,
//                            rk.m_r*m_cellstorlek,
//                            m_cellstorlek,
//                            m_cellstorlek);
    for(unsigned int i=0; i<m_masken1.size(); i++){
        RK rk = m_masken1[i];

        QRadialGradient gradient(rk.m_k*m_cellstorlek + m_cellstorlek/2,
                                 rk.m_r*m_cellstorlek + m_cellstorlek/2,
                                 m_cellstorlek/2,
                                 rk.m_k*m_cellstorlek + m_cellstorlek/2,
                                 rk.m_r*m_cellstorlek + m_cellstorlek/2);

        gradient.setColorAt(0, Qt::white);
        gradient.setColorAt(1, snakeColor1);
        painter.setBrush(gradient);

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

//        for(unsigned int i=0; i<m_masken2.size(); i++){
//            RK rk = m_masken2[i];
//            painter.drawEllipse(rk.m_k*m_cellstorlek,
//                                rk.m_r*m_cellstorlek,
//                                m_cellstorlek,
//                                m_cellstorlek);
//        }
        for(unsigned int i=0; i<m_masken2.size(); i++){
            RK rk = m_masken2[i];

            QRadialGradient gradient(rk.m_k*m_cellstorlek + m_cellstorlek/2,
                                     rk.m_r*m_cellstorlek + m_cellstorlek/2,
                                     m_cellstorlek/2,
                                     rk.m_k*m_cellstorlek + m_cellstorlek/2,
                                     rk.m_r*m_cellstorlek + m_cellstorlek/2);

            gradient.setColorAt(0, Qt::white);
            gradient.setColorAt(1, snakeColor2);
            painter.setBrush(gradient);

            painter.drawEllipse(rk.m_k*m_cellstorlek,
                                rk.m_r*m_cellstorlek,
                                m_cellstorlek,
                                m_cellstorlek);
        }
    }

    if(m_obsLvl){
//        for(unsigned int i=0; i<m_obstacle.size(); i++){
//            RK rk = m_obstacle[i];
//            painter.fillRect(rk.m_k*m_cellstorlek,
//                             rk.m_r*m_cellstorlek,
//                             m_cellstorlek,
//                             m_cellstorlek,
//                             Qt::darkRed);
//        }
        for(unsigned int i=0; i<m_obstacle.size(); i++){
            RK rk = m_obstacle[i];

            QRadialGradient gradient(rk.m_k*m_cellstorlek + m_cellstorlek/2,
                                     rk.m_r*m_cellstorlek + m_cellstorlek/2,
                                     m_cellstorlek/2,
                                     rk.m_k*m_cellstorlek + m_cellstorlek/2,
                                     rk.m_r*m_cellstorlek + m_cellstorlek/2);

            gradient.setColorAt(0, Qt::white);
            gradient.setColorAt(1, Qt::darkRed);
            painter.setBrush(gradient);

            painter.drawEllipse(rk.m_k*m_cellstorlek,
                                rk.m_r*m_cellstorlek,
                                m_cellstorlek,
                                m_cellstorlek);
        }
    }

}

bool operator==(const Spelplan2::RK& rk1, Spelplan2::RK& rk2)
{
    return rk1.m_r == rk2.m_r && rk1.m_k == rk2.m_k;

}
bool operator!=(const Spelplan2::RK& rk1, Spelplan2::RK& rk2)
{
    return !(rk1 == rk2);
}

Spelplan2::RK operator-(const Spelplan2::RK& rk1, Spelplan2::RK& rk2)
{
    return Spelplan2::RK(rk1.m_r-rk2.m_r, rk1.m_k-rk2.m_k);
}


