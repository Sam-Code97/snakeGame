#ifndef SPELPLAN2_H
#define SPELPLAN2_H

#include <QWidget>
#include <deque>

class Spelplan2 : public QWidget
{
    Q_OBJECT
public:
    explicit Spelplan2(QWidget *parent = nullptr);
//    ~Spelplan2();
    int score1();
    int score2();
    void restartGame();
    void puseGame();
    void addPlayer(int i);
    void transWalls(int i);
    void obstacles(int lvl);
    void speedingOn(bool spd);
    void checkCollision();
    void checkWinner();
    int winner();
    int winScore(int &score);

    void changeSpeed(int speed);
    void changeLength(int length);
    void restartedAfterSizeChange(bool b);
    void fruitPosition();





signals:
    void scoreChanged1();
    void scoreChanged2();
    void someOneWon();
    void replayed();
    void updateSpeed();

private slots:
    void uppdateraMasken();
    void checkEatFruit();




    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QTimer *m_ptimer;
    const int m_cellstorlek = 40;
    struct RK{
        RK(int r=0, int k=0) : m_r(r), m_k(k){}
        int m_r;
        int m_k;
    };
    friend bool operator==(const Spelplan2::RK& rk1, Spelplan2::RK& rk2);
    friend bool operator!=(const Spelplan2::RK& rk1, Spelplan2::RK& rk2);
    friend Spelplan2::RK operator-(const Spelplan2::RK& rk1, Spelplan2::RK& rk2);

    bool m_foundShortestPathToGoalFIFO(RK huvud);

    int m_score1;
    int m_score2;
    int m_speed;

    RK m_rkRiktning1;
    RK m_rkRiktningBefor1;
    RK m_rkRiktning2;
    RK m_rkRiktningBefor2;

    RK m_tempRikt2;
    RK m_tempRikt1;

    RK m_rkHuvud1;
    RK m_rkHuvud2;
    RK m_rkFruit;

    std::deque<RK> m_masken1;
    std::deque<RK> m_masken2;
    std::vector<RK> m_obstacle;
    std::vector<std::vector<RK>> m_rkNext;
//    std::deque<RK> rkTodo;

    int m_lengh1;
    int m_lengh2;

    bool m_collision1;
    bool m_collision2;

    bool m_startedMoving;
    bool m_eatedFruit;
    int m_timesEated;
    int m_rows;
    int m_cols ;
    bool m_twoPl;
    bool m_transWall;
    int m_obsLvl;
    bool m_speedON;

    int m_tapA;
    int m_tapB;

    bool m_datorSpelar;
    bool m_pcCalculated;
    int m_winner;
    int m_winscore;

    int m_oldSpeed;
    bool m_restartedAfterSizeChange;

};

inline bool operator==(const Spelplan2::RK& rk1, Spelplan2::RK& rk2);
inline bool operator!=(const Spelplan2::RK& rk1, Spelplan2::RK& rk2);
inline Spelplan2::RK operator-(const Spelplan2::RK& rk1, Spelplan2::RK& rk2);


#endif // SPELPLAN2_H
