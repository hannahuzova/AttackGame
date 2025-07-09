#include "attackbar.h"
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>

AttackBar::AttackBar(QWidget* parent): QWidget(parent), pos_(0), dir_(1), running_(false){
    setFixedHeight(40);
    timer_=new QTimer(this);
    connect(timer_, &QTimer::timeout, this, [this]{ if(!running_) return; pos_+=dir_*2; if(pos_>=100){pos_=100;dir_=-1;} if(pos_<=0){pos_=0;dir_=1;} update(); });
}
void AttackBar::handleClick() {
    if (running_) {
        running_ = false;
        emit stopped(pos_);
    }
}
void AttackBar::start(){ running_=true; timer_->start(30);}
void AttackBar::mousePressEvent(QMouseEvent*){ if(running_){ running_=false; emit stopped(pos_);} }
void AttackBar::paintEvent(QPaintEvent*){
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
    QRect bar=rect().adjusted(5,5,-5,-5);
    p.setBrush(QColor(50,50,50)); p.drawRoundedRect(bar,4,4);
    QLinearGradient g(bar.topLeft(), bar.topRight()); g.setColorAt(0,QColor(60,60,160)); g.setColorAt(1,QColor(200,100,40));
    p.setBrush(g);
    int ind=bar.left()+(bar.width()*pos_)/100;
    QRect fill(bar.left(),bar.top(),ind-bar.left(),bar.height());
    p.drawRoundedRect(fill,4,4);
    p.setPen(QPen(Qt::white,2)); p.drawLine(ind,bar.top(),ind,bar.bottom());
}
