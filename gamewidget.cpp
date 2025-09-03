#include "gamewidget.h"
#include "attackbar.h"
#include "projectile.h"
#include "hpbar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QMessageBox>

namespace {
const char* BTN_CSS = R"(
QPushButton, QToolButton {
    color: #f0e6d2;
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #8b4513, stop:1 #5d2906);
    border: 2px solid #d4af37;
    border-radius: 10px;
    font-family: 'Fritz Quadrata Cyrillic', 'Friz Quadrata', sans-serif;
    font-size: 24px;
    padding: 10px;
}
QPushButton:hover:enabled, QToolButton:hover:enabled {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                stop:0 #9d5a1f, stop:1 #6d390c);
}
)";
}

GameWidget::GameWidget(int lvl, QWidget* parent)
    : QWidget(parent), level_(lvl)
{
    setObjectName("levelScreen");
    setFixedSize(1280,720);
    setAttribute(Qt::WA_StyledBackground,true);
    setStyleSheet(QString(
                      "QWidget#levelScreen{background-image:url(:/images/background_level_%1.png);"
                      "background-repeat:no-repeat;background-position:center;background-size:cover;}"
                      ).arg(level_));

    QVBoxLayout* root = new QVBoxLayout(this);

    QHBoxLayout* top = new QHBoxLayout;
    root->addLayout(top);

    const QStringList elems{ "fire","water","earth","air" };
    const int heroHP = 500;

    for (int i=0;i<4;++i) {
        QToolButton* btn = new QToolButton(this);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setIcon(QPixmap(":/images/good_"+elems[i]+".png"));
        btn->setIconSize(QSize(96,96));
        btn->setFixedSize(104,104);
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        btn->setAutoRaise(false);
        btn->setStyleSheet("QToolButton { border: none; background: none; }"
                           "QToolButton::pressed { border: none; background: none; }");

        HPBar* hp = new HPBar(heroHP,this);

        QVBoxLayout* cellL = new QVBoxLayout;
        cellL->addWidget(btn,0,Qt::AlignHCenter);
        cellL->addWidget(hp);

        QWidget* cell = new QWidget(this);
        cell->setLayout(cellL);
        top->addWidget(cell);

        heroes_[i] = { elems[i], heroHP, nullptr, hp, btn };
        connect(btn,&QToolButton::clicked,this,[this,i]{ onHeroClicked(i); });
        if (i==0) btn->setChecked(true);
    }

    QHBoxLayout* arena = new QHBoxLayout;
    root->addLayout(arena,1);

    heroSprite_ = new QLabel(this);
    heroSprite_->setPixmap(QPixmap(":/images/good_fire.png"));
    heroSprite_->setScaledContents(true);
    heroSprite_->setFixedSize(200,200);

    enemySprite_ = new QLabel(this);
    enemySprite_->setPixmap(QPixmap(":/images/evil_fire.png"));
    enemySprite_->setScaledContents(true);
    enemySprite_->setFixedSize(200,200);

    const int enemyBase = 500 + (level_-1)*100;
    enemyHpBar_ = new HPBar(enemyBase,this);

    QVBoxLayout* heroC = new QVBoxLayout;
    heroC->addWidget(heroSprite_,0,Qt::AlignLeft|Qt::AlignVCenter);
    heroC->addStretch();

    QVBoxLayout* enemyC = new QVBoxLayout;
    enemyC->addWidget(enemySprite_,0,Qt::AlignRight|Qt::AlignVCenter);
    enemyC->addWidget(enemyHpBar_,0,Qt::AlignRight);

    arena->addLayout(heroC,1);
    arena->addLayout(enemyC,1);

    for (int i=0;i<4;++i) enemies_[i] = { elems[i], enemyBase, nullptr, nullptr, nullptr };


    bar_ = new AttackBar(this);
    atkBtn_= new QPushButton("УДАР!",this);
    atkBtn_->setStyleSheet(BTN_CSS);
    atkBtn_->setFixedSize(180,60);

    connect(bar_, &AttackBar::stopped,this,&GameWidget::onBarStopped);
    connect(atkBtn_, &QPushButton::clicked, bar_, &AttackBar::stop);

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addWidget(bar_,1);
    bottom->addWidget(atkBtn_);
    root->addLayout(bottom);

    timer_ = new QTimer(this);
    timer_->setSingleShot(true);
    connect(timer_,&QTimer::timeout,this,&GameWidget::enemyAttack);

    bar_->start();
}

void GameWidget::onHeroClicked(int idx)
{
    if (heroes_[idx].hp==0) return;
    curHero_ = idx;
    heroSprite_->setPixmap(QPixmap(":/images/good_"+heroes_[idx].element+".png"));
}

int GameWidget::dmg(const QString& atk,const QString& def,int base)
{
    static QMap<QString,QString> adv{ {"water","fire"},{"fire","air"},{"air","earth"},{"earth","water"} };
    if (adv[atk]==def) return base*2;
    if (adv[def]==atk) return base/2 + 10;
    return base;
}

void GameWidget::projectile(bool fromHero,const QString& elem)
{
    QString res = QString(":/images/%1_aura_%2.png")
    .arg(fromHero?"good":"evil", elem);
    Projectile* p = new Projectile(QPixmap(res),this);

    const QPoint src = (fromHero?heroSprite_:enemySprite_)->geometry().center();
    const QPoint dst = (fromHero?enemySprite_:heroSprite_)->geometry().center();
    p->move(src - QPoint(p->width()/2,p->height()/2));
    p->show();

    auto* anim = new QPropertyAnimation(p,"pos",p);
    anim->setDuration(400);
    anim->setStartValue(src - QPoint(p->width()/2,p->height()/2));
    anim->setEndValue(dst - QPoint(p->width()/2,p->height()/2));
    connect(anim,&QPropertyAnimation::finished,p,&QObject::deleteLater);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameWidget::onBarStopped(int power)
{
    if (!playerTurn_) return;
    playerTurn_ = false;
    atkBtn_->setEnabled(false);
    bar_->setEnabled(false);

    int damage = dmg(heroes_[curHero_].element, enemies_[curEnemy_].element, power);
    projectile(true, heroes_[curHero_].element);

    enemies_[curEnemy_].hp = qMax(0, enemies_[curEnemy_].hp - damage);
    enemyHpBar_->setValue(enemies_[curEnemy_].hp);

    if (enemies_[curEnemy_].hp == 0) {
        ++curEnemy_;
        if (curEnemy_ >= 4) {
            emit levelCompleted(level_);
            return;
        }
        enemySprite_->setPixmap(QPixmap(":/images/evil_"+enemies_[curEnemy_].element+".png"));
        enemies_[curEnemy_].hp = enemyHpBar_->maximum();
        enemyHpBar_->setValue(enemyHpBar_->maximum());
    }
    timer_->start(600);
}

void GameWidget::enemyAttack()
{
    const int base   = QRandomGenerator::global()->bounded(10,100);
    const int damage = dmg(enemies_[curEnemy_].element, heroes_[curHero_].element, base);

    projectile(false,enemies_[curEnemy_].element);

    heroes_[curHero_].hp = qMax(0, heroes_[curHero_].hp - damage);
    heroes_[curHero_].hpBar->setValue(heroes_[curHero_].hp);

    if (heroes_[curHero_].hp == 0) {
        heroes_[curHero_].btn->setEnabled(false);
        switchToNextAliveHero();
        if (aliveHeroes()==0) {
            QMessageBox::information(this,"Поражение","Все герои пали!");
            emit levelFailed();
            return;
        }
    }
    playerTurn_ = true;
    atkBtn_->setEnabled(true);
    bar_->setEnabled(true);
    bar_->start();
}

void GameWidget::switchToNextAliveHero()
{
    for (int i=0;i<4;++i) {
        int idx = (curHero_+1+i)%4;
        if (heroes_[idx].hp>0) {
            heroes_[idx].btn->setChecked(true);
            onHeroClicked(idx);
            return;
        }
    }
}

int GameWidget::aliveHeroes() const
{
    int c=0;  for (auto& h:heroes_) if (h.hp>0) ++c;  return c;
}
