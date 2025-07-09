#include "gamewidget.h"
#include "attackbar.h"
#include "projectile.h"
#include "hpbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QFontDatabase>
#include <QRandomGenerator>
#include <QMessageBox>

GameWidget::GameWidget(QWidget* parent): QWidget(parent)
{
    setObjectName("levelScreen");
    setAttribute(Qt::WA_StyledBackground, true);

    // 2. Проверьте загрузку фона
    QPixmap bgTest(":/images/background_level.png");
    if (bgTest.isNull()) {
        qDebug() << "Ошибка: фон не загружен!";
    }
    else {
        qDebug() << "Фон загружен, размер:" << bgTest.size();
    }

    int fontId = QFontDatabase::addApplicationFont(":/fonts/fritzquadratacyrillic.ttf");
    if (fontId == -1) {
        qWarning() << "Failed to load font! Using fallback";
    }

    // 3. Установите фиксированный размер
    setFixedSize(1280, 720);

    // 4. Примените стиль с !important для приоритета
    setStyleSheet(R"(
        QWidget#levelScreen {
            background-image: url(:/images/background_level.png) !important;
            background-repeat: no-repeat;
            background-position: center;
            background-size: cover;
        }
        QLabel, QPushButton { color: white; }
        QPushButton {
            background: rgba(0, 0, 0, 150);
            border: 2px solid #666;
            border-radius: 8px;
            padding: 6px 12px;
            font-size: 24px;
        }
    )");

    QVBoxLayout* root = new QVBoxLayout(this);

    // ---- Heroes row ----
    QHBoxLayout* top = new QHBoxLayout;
    root->addLayout(top);

    QStringList elems = {"fire","water","earth","air"};
    for(int i=0;i<4;++i){
        QToolButton* btn = new QToolButton(this);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setIcon(QPixmap(":/images/good_"+elems[i]+".png"));
        btn->setIconSize(QSize(96,96));       // enlarged
        btn->setFixedSize(104,104);
        btn->setStyleSheet("background: transparent;");

        HPBar* hpBar = new HPBar(500,this);

        QVBoxLayout* cell = new QVBoxLayout;
        QWidget* w = new QWidget(this);
        w->setLayout(cell);
        cell->addWidget(btn,0,Qt::AlignHCenter);
        cell->addWidget(hpBar);
        top->addWidget(w);

        heroes_[i] = {elems[i], 500, nullptr, hpBar};

        connect(btn, &QToolButton::clicked, this, [this,i]{ onHeroClicked(i); });
        if(i==0) btn->setChecked(true);
    }

    // ---- Battle area ----
    QHBoxLayout* battle = new QHBoxLayout;
    root->addLayout(battle,1);

    heroSprite_ = new QLabel(this);
    heroSprite_->setPixmap(QPixmap(":/images/good_fire.png"));
    heroSprite_->setScaledContents(true);
    heroSprite_->setFixedSize(200,200);

    enemySprite_ = new QLabel(this);
    enemySprite_->setPixmap(QPixmap(":/images/evil_fire.png"));
    enemySprite_->setScaledContents(true);
    enemySprite_->setFixedSize(200,200);

    // column for hero sprite + placeholder (no global bar)
    QVBoxLayout* heroCol = new QVBoxLayout;
    heroCol->addWidget(heroSprite_,0,Qt::AlignLeft|Qt::AlignVCenter);
    heroCol->addStretch();

    QVBoxLayout* enemyCol = new QVBoxLayout;
    enemyCol->addWidget(enemySprite_,0,Qt::AlignRight|Qt::AlignVCenter);
    enemyHpBar_ = new HPBar(500,this);
    enemyCol->addWidget(enemyHpBar_,0,Qt::AlignRight);
    battle->addLayout(heroCol,1);
    battle->addLayout(enemyCol,1);

    for(int i=0;i<4;++i)
        enemies_[i] = {elems[i], 500, nullptr, nullptr};

    // ---- Attack bar and button ----
    bar_ = new AttackBar(this);
    bar_->setFixedHeight(40);     // enlarged
    bar_->start();

    atkBtn_ = new QPushButton("УДАР!",this);
    atkBtn_->setStyleSheet(R"(
        QPushButton {
            color: #f0e6d2;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #8b4513, stop:1 #5d2906);
            border: 2px solid #d4af37;
            border-radius: 10px;
            font-family: 'Fritz Quadrata Cyrillic', 'Friz Quadrata', sans-serif;
            font-size: 24px;
            padding: 10px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                      stop:0 #9d5a1f, stop:1 #6d390c);
        }
        )");

    QFontDatabase::addApplicationFont(":/fonts/fritzquadratacyrillic.ttf");
    QString fantasyFont = "Fritz Quadrata Cyrillic";
    atkBtn_->setFont(fantasyFont);
    atkBtn_->setFixedSize(160,60);

    connect(bar_, &AttackBar::stopped, this, &GameWidget::onBarStopped);
    connect(atkBtn_, &QPushButton::clicked, bar_, &AttackBar::handleClick); // simulate click

    QHBoxLayout* bottom = new QHBoxLayout;
    bottom->addWidget(bar_,1);
    bottom->addWidget(atkBtn_);
    root->addLayout(bottom);

    timer_ = new QTimer(this);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, this, &GameWidget::enemyAttack);
}

void GameWidget::onHeroClicked(int idx){
    curHero_ = idx;
    heroSprite_->setPixmap(QPixmap(":/images/good_" + heroes_[idx].element + ".png"));
}

int GameWidget::dmg(const QString& atk,const QString& def,int base){
    QMap<QString,QString> adv{{"water","fire"},{"fire","earth"},{"earth","air"},{"air","water"}};
    if(adv[atk]==def) return base*2;
    if(adv[def]==atk) return base/2+10;
    return base;
}

void GameWidget::projectile(bool fromHero,const QString& elem){
    QString res = QString(":/images/%1_aura_%2.png").arg(fromHero?"good":"evil",elem);
    Projectile* p = new Projectile(QPixmap(res), this);
    int half = 40; // projectile half-size
    QPoint s = (fromHero?heroSprite_:enemySprite_)->geometry().center();
    QPoint e = (fromHero?enemySprite_:heroSprite_)->geometry().center();
    p->move(s - QPoint(half,half));
    p->show();
    QPropertyAnimation* a = new QPropertyAnimation(p,"pos",p);
    a->setDuration(400);
    a->setStartValue(s - QPoint(half,half));
    a->setEndValue(e - QPoint(half,half));
    connect(a, &QPropertyAnimation::finished, p, &Projectile::deleteLater);
    a->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameWidget::onBarStopped(int power){
    bar_->start();
    int damage = dmg(heroes_[curHero_].element, enemies_[curEnemy_].element, power);
    projectile(true, heroes_[curHero_].element);

    enemies_[curEnemy_].hp -= damage;
    if(enemies_[curEnemy_].hp < 0) enemies_[curEnemy_].hp = 0;
    enemyHpBar_->setValue(enemies_[curEnemy_].hp);

    if(enemies_[curEnemy_].hp == 0){
        ++curEnemy_;
        if(curEnemy_ >= 4){
            QMessageBox::information(nullptr,"Победа!","Все враги повержены!");
            return;
        }
        enemySprite_->setPixmap(QPixmap(":/images/evil_" + enemies_[curEnemy_].element + ".png"));
        enemies_[curEnemy_].hp = 500;
        enemyHpBar_->setValue(500);
    }
    timer_->start(600);
}

void GameWidget::enemyAttack(){
    int base = QRandomGenerator::global()->bounded(10,100);
    int damage = dmg(enemies_[curEnemy_].element, heroes_[curHero_].element, base);
    projectile(false, enemies_[curEnemy_].element);

    heroes_[curHero_].hp -= damage;
    if(heroes_[curHero_].hp < 0) heroes_[curHero_].hp = 0;
    heroes_[curHero_].hpBar->setValue(heroes_[curHero_].hp);

    if(heroes_[curHero_].hp == 0){
        QMessageBox::information(nullptr,"Поражение!","Ваш герой умер! Поменяйте его, или выйдите из игры");
    }
}
