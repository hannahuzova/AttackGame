#include "mainwindow.h"
#include "gamewidget.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QFontDatabase>
#include <QVector>
#include <QApplication>
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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    const int fid = QFontDatabase::addApplicationFont(":/fonts/fritzquadratacyrillic.ttf");
    if (fid != -1) {
        const QString fam = QFontDatabase::applicationFontFamilies(fid).value(0);
        if (!fam.isEmpty())
            QApplication::setFont(QFont(fam));
    }

    setFixedSize(1280,720);
    stack_ = new QStackedWidget(this);
    setCentralWidget(stack_);

    QWidget* menu = createMenu();
    rules_ = createRules();
    levelSelect_ = createLevelSelect();

    stack_->addWidget(menu);
    stack_->addWidget(rules_);
    stack_->addWidget(levelSelect_);

    connect(menu->findChild<QPushButton*>("startBtn"),
            &QPushButton::clicked,[this]{ stack_->setCurrentWidget(rules_); });

    connect(rules_->findChild<QPushButton*>("playBtn"),
            &QPushButton::clicked,[this]{
                refreshButtons();
                stack_->setCurrentWidget(levelSelect_);
            });

    connect(menu->findChild<QPushButton*>("mainMenuExitBtn"), &QPushButton::clicked, [] { QApplication::quit(); });
    connect(rules_->findChild<QPushButton*>("rulesBackBtn"), &QPushButton::clicked, [this, menu] { stack_->setCurrentWidget(menu); });
    connect(levelSelect_->findChild<QPushButton*>("levelSelectBackBtn"), &QPushButton::clicked, [this] { stack_->setCurrentWidget(rules_); });
}

QWidget* MainWindow::createMenu()
{
    QWidget* w = new QWidget; w->setObjectName("menuScreen");
    w->setAttribute(Qt::WA_StyledBackground,true);
    w->setStyleSheet(
        "QWidget#menuScreen{background-image:url(:/images/background_menu.png);"
        "background-repeat:no-repeat;background-position:center;background-size:cover;}"
        + QString(BTN_CSS));

    QVBoxLayout* l = new QVBoxLayout(w);
    l->addStretch();

    QLabel* title = new QLabel("АТАКА",w);
    title->setStyleSheet("font-size:64px;color:#f0e6d2;text-shadow:2px 2px 4px #000;");
    title->setAlignment(Qt::AlignCenter);
    l->addWidget(title);

    QPushButton* start = new QPushButton("В ИГРУ!",w);
    start->setObjectName("startBtn");
    start->setFixedSize(300,80);
    l->addWidget(start,0,Qt::AlignHCenter);

    QPushButton* exit = new QPushButton("Выйти",w);
    exit->setObjectName("mainMenuExitBtn");
    exit->setFixedSize(300,80);
    l->addWidget(exit,0,Qt::AlignHCenter);

    l->addStretch();
    applyDarkStyle(w);
    return w;
}

QWidget* MainWindow::createRules()
{
    QWidget* w = new QWidget; w->setObjectName("rulesScreen");
    w->setAttribute(Qt::WA_StyledBackground,true);
    w->setStyleSheet(
        "QWidget#rulesScreen{background-image:url(:/images/background_menu.png);"
        "background-repeat:no-repeat;background-position:center;background-size:cover;}"
        + QString(BTN_CSS));

    QVBoxLayout* l = new QVBoxLayout(w);

    QLabel* txt = new QLabel(
        "<html><head><style>"
        "body{color:#f0e6d2;font-family:'Fritz Quadrata Cyrillic','Friz Quadrata',sans-serif;"
        "font-size:28px;text-align:center;line-height:1.5;}"
        "h1{color:#d4af37;font-size:32px;font-weight:bold;text-shadow:2px 2px 4px #000;"
        "margin-bottom:20px;}"
        "ul{text-align:left;padding-left:40px;list-style-type:none;}"
        "li{margin-bottom:12px;position:relative;}"
        "li:before{content:'✦';color:#d4af37;position:absolute;left:-20px;}"
        ".element{font-size:24px;}"
        "</style></head><body>"
        "<h1>✨ ПРАВИЛА СРАЖЕНИЙ ✨</h1>"
        "<ul>"
        "<li>Некоторые персонажи сильнее других, в зависимости от противника.</li>"
        "<li>Каждый герой реагирует по-своему на удары врага.</li>"
        "<li>Следите за стихией оппонента — это ключ к победе.</li>"
        "<li>Потерявший силу герой покидает битву.</li>"
        "<li>Вода > Огонь > Воздух > Земля > Вода.</li>"
        "<li><b>Цикл стихий:</b> "
        "<span class='element'>💧</span> > "
        "<span class='element'>🔥</span> > "
        "<span class='element'>🌪️</span> > "
        "<span class='element'>🌍</span> > "
        "<span class='element'>💧</span></li>"
        "</ul></body></html>", w);
    txt->setAlignment(Qt::AlignCenter);
    txt->setWordWrap(true);
    l->addWidget(txt);

    QPushButton* play = new QPushButton("⚔️ В БОЙ!",w);
    play->setObjectName("playBtn");
    play->setFixedSize(300,80);
    l->addWidget(play,0,Qt::AlignHCenter);

    QPushButton* rulesBack = new QPushButton("Назад",w);
    rulesBack->setObjectName("rulesBackBtn");
    rulesBack->setFixedSize(300,80);
    l->addWidget(rulesBack,0,Qt::AlignHCenter);

    l->addStretch();
    applyDarkStyle(w);
    return w;
}

QWidget* MainWindow::createLevelSelect()
{
    QWidget* w = new QWidget; w->setObjectName("levelSelectScreen");
    w->setAttribute(Qt::WA_StyledBackground,true);
    w->setStyleSheet(
        "QWidget#levelSelectScreen{background-image:url(:/images/background_menu.png);"
        "background-repeat:no-repeat;background-position:center;background-size:cover;}"
        + QString(BTN_CSS));

    QVBoxLayout* v = new QVBoxLayout(w);
    v->addStretch();

    QHBoxLayout* h = new QHBoxLayout;
    v->addLayout(h);
    v->addStretch();

    QVector<QString> level_names {"Загадочный Лес", "Живая роща", "Водопад смерти", "Раскаленная долина"};
    for (int i=0;i<4;++i) {
        QToolButton* btn = new QToolButton(w);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setIcon(QPixmap(QString(":/images/background_level_%1.png").arg(i+1)));
        btn->setIconSize(QSize(320, 320));
        btn->setText(QString(level_names[i]));
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setCheckable(false);
        btn->setMinimumSize(240,600);

        h->addWidget(btn,1);
        levelBtns_[i] = btn;
        connect(btn,&QToolButton::clicked,this,[this,i]{ startLevel(i+1); });
    }

    QPushButton* levelSelectBack = new QPushButton("НАЗАД",w);
    levelSelectBack->setObjectName("levelSelectBackBtn");
    levelSelectBack->setFixedSize(300,80);
    v->addWidget(levelSelectBack,0,Qt::AlignLeft);

    applyDarkStyle(w);
    refreshButtons();
    return w;
}

void MainWindow::applyDarkStyle(QWidget* root)
{
    root->setStyleSheet(root->styleSheet() +
                        " *{color:#f0f0f0;}"
                        " QToolTip{color:#f0f0f0;background:#404040;}");
}

void MainWindow::refreshButtons()
{
    for (int i=0;i<4;++i)
        levelBtns_[i]->setEnabled(i < unlockedLevel_);
}

void MainWindow::startLevel(int level)
{
    if (game_) {
        stack_->removeWidget(game_);
        delete game_;
    }
    game_ = new GameWidget(level,this);
    stack_->addWidget(game_);
    stack_->setCurrentWidget(game_);
    connect(game_, &GameWidget::levelCompleted, this, &MainWindow::onLevelCompleted);
    connect(game_, &GameWidget::levelFailed, this, &MainWindow::onLevelFailed);
}

void MainWindow::onLevelCompleted(int lvl)
{
    if (lvl == 4) {
        QMessageBox box(this);
        box.setWindowTitle("Победа!");
        box.setIconPixmap(QPixmap(":/images/victory.png"));
        box.exec();

        unlockedLevel_ = 4;
    }
    else {
        QMessageBox::information(this, "Уровень пройден", QString("Уровень %1 пройден!").arg(lvl));
        if (unlockedLevel_ < lvl + 1) ++unlockedLevel_;
    }

    refreshButtons();
    stack_->setCurrentWidget(levelSelect_);
}

void MainWindow::onLevelFailed()
{
    if (game_) {
        stack_->removeWidget(game_);
        game_->deleteLater();
        game_ = nullptr;
    }

    refreshButtons();
    stack_->setCurrentWidget(levelSelect_);
}
