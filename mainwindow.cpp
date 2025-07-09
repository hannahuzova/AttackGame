#include "mainwindow.h"
#include <QStackedWidget>
#include <QPushButton>
#include <QFontDatabase>
#include <QVBoxLayout>
#include <QLabel>
#include "gamewidget.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    // Фиксированный размер окна
    setFixedSize(1280, 720);

    // Загрузка шрифта
    int fontId = QFontDatabase::addApplicationFont(":/fonts/fritzquadratacyrillic.ttf");
    if (fontId == -1) {
        qWarning() << "Failed to load font! Using fallback";
    }

    stack_ = new QStackedWidget(this);
    setCentralWidget(stack_);

    QWidget* menu = createMenu();
    QWidget* rules = createRules();
    game_ = new GameWidget;

    stack_->addWidget(menu);
    stack_->addWidget(rules);
    stack_->addWidget(game_);

    connect(menu->findChild<QPushButton*>("startBtn"), &QPushButton::clicked, [this]{stack_->setCurrentIndex(1);});
    connect(rules->findChild<QPushButton*>("playBtn"), &QPushButton::clicked, [this]{stack_->setCurrentIndex(2);});
}

QWidget* MainWindow::createMenu(){
    QWidget* w=new QWidget; w->setObjectName("menuScreen");
    QVBoxLayout* l=new QVBoxLayout(w);
    l->addStretch();
    QLabel* t=new QLabel("АТАКА",w);
    t->setStyleSheet("font-size:48px;color:white; font-family: 'Fritz Quadrata Cyrillic', 'Friz Quadrata', sans-serif;");
    t->setAlignment(Qt::AlignCenter);
    l->addWidget(t);
    QPushButton* b=new QPushButton("НАЧАТЬ",w);
    b->setStyleSheet(R"(
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
    b->setObjectName("startBtn");
    b->setFixedSize(250,80);
    l->addWidget(b,0,Qt::AlignHCenter);
    l->addStretch();
    applyDarkStyle(w);
    return w;
}

QWidget* MainWindow::createRules() {
    QWidget* w = new QWidget;
    w->setObjectName("rulesScreen");

    // Главный макет с отступами
    QVBoxLayout* l = new QVBoxLayout(w);
    l->setContentsMargins(50, 30, 50, 30); // Отступы от краёв
    l->setSpacing(20);

    // Текст правил с улучшенным форматированием
    QLabel* txt = new QLabel(
        "<html>"
        "<head>"
        "<style>"
        "body {"
        "   color: #f0e6d2;"                  // Кремовый текст (лучше читается на тёмном)
        "   font-family: 'Fritz Quadrata Cyrillic', 'Friz Quadrata', sans-serif;"
                              "   font-size: 28px;"                 // Крупнее базовый текст
                              "   text-align: center;"
                              "   line-height: 1.5;"                // Межстрочный интервал
                              "}"
                              "h1 {"
                              "   color: #d4af37;"                  // Золотой заголовок
                              "   font-size: 32px;"                 // Большой заголовок
                              "   font-weight: bold;"
                              "   text-shadow: 2px 2px 4px #000;"
                              "   margin-bottom: 20px;"
                              "}"
                              "ul {"
                              "   text-align: left;"
                              "   padding-left: 40px;"
                              "   list-style-type: none;"           // Убираем стандартные маркеры
                              "}"
                              "li {"
                              "   margin-bottom: 12px;"
                              "   position: relative;"
                              "}"
                              "li:before {"
                              "   content: '✦';"                   // Фэнтезийный маркер
                              "   color: #d4af37;"
                              "   position: absolute;"
                              "   left: -20px;"
                              "}"
                              ".element {"
                              "   font-size: 24px;"                // Крупные эмодзи стихий
                              "}"
                              "</style>"
                              "</head>"
                              "<body>"
                              "<h1>✨ ПРАВИЛА БИТВ ✨</h1>"
                              "<ul>"
                              "<li>Некоторые персонажи сильнее других, в зависимости от противника, с которым вы сталкиваетесь.</li>"
                              "<li>Каждый персонаж будет реагировать по-разному на атаки противника!</li>"
                              "<li>Обращайте внимание на стихию оппонента — она главный ключ к победе над злом.</li>"
                              "<li>Потерявший силу персонаж покидает битву.</li>"
                              "<li>Вода побеждает огонь, огонь побеждает воздух, воздух побеждает землю, а земля побеждает воду.</li>"
                              "<li><b>Цикл стихий:</b>"
                              "   <span class='element'>💧</span> > "
                              "   <span class='element'>🔥</span> > "
                              "   <span class='element'>🌪️</span> > "
                              "   <span class='element'>🌍</span> > "
                              "   <span class='element'>💧</span>"
                              "</li>"
                              "</ul>"
                              "</body>"
                              "</html>"
        );

    txt->setAlignment(Qt::AlignCenter);
    txt->setWordWrap(true);
    l->addWidget(txt);

    // Стилизованная кнопка
    QPushButton* p = new QPushButton("⚔️ В БОЙ!", w);
    p->setObjectName("playBtn");
    p->setFixedSize(300, 80);
    p->setStyleSheet(R"(
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

    l->addWidget(p, 0, Qt::AlignHCenter);
    l->addStretch();

    applyDarkStyle(w);
    return w;
}

void MainWindow::applyDarkStyle(QWidget* w){
    w->setStyleSheet(R"(
        QWidget#menuScreen, QWidget#rulesScreen{
            background-image:url(:/images/background_menu.png);
            background-repeat:no-repeat;
            background-position:center;
            background-size:cover;
        }
        QPushButton{
            color:white;
            background:rgba(0,0,0,150);
            border:2px solid #666;
            border-radius:8px;
            font-size:28px;
            padding:6px 12px;
        }
        QPushButton:hover{background:rgba(100,100,100,150);}
    )");
}
