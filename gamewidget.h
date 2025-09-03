#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H
#include <QWidget>
#include <array>

class AttackBar;
class QPushButton;
class QLabel;
class QTimer;
class HPBar;
class QToolButton;

struct Fighter {
    QString element;
    int hp;
    QLabel* sprite;
    HPBar* hpBar;
    QToolButton* btn;
};

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(int level, QWidget* parent = nullptr);

signals:
    void levelCompleted(int level);
    void levelFailed();

private slots:
    void onHeroClicked(int idx);
    void onBarStopped(int power);
    void enemyAttack();

private:
    int dmg(const QString& atk, const QString& def, int base);
    void projectile(bool fromHero, const QString& elem);
    void switchToNextAliveHero();
    int aliveHeroes() const;

    std::array<Fighter,4> heroes_;
    std::array<Fighter,4> enemies_;
    int  curHero_ = 0;
    int  curEnemy_ = 0;
    int  level_ = 1;
    bool playerTurn_= true;

    AttackBar* bar_;
    QPushButton* atkBtn_;
    QTimer* timer_;

    QLabel* heroSprite_;
    QLabel* enemySprite_;
    HPBar* enemyHpBar_;
};
#endif
