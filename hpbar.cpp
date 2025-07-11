#include "hpbar.h"
#include <QPainter>

HPBar::HPBar(int maximum, QWidget* parent)
    : QWidget(parent), max_(maximum), val_(maximum)
{
    setFixedHeight(25);
    setMinimumWidth(120);
}

void HPBar::setValue(int v)
{
    val_ = qBound(0, v, max_);
    update();
}

void HPBar::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect frame = rect().adjusted(2, 2, -2, -2);
    p.setBrush(QColor(50, 50, 50));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(frame, 4, 4);

    if (val_ > 0) {
        const double ratio = double(val_) / max_;
        QRect fill(frame.left(), frame.top(),
                   int(frame.width() * ratio),
                   frame.height());
        QLinearGradient g(fill.topLeft(), fill.topRight());
        g.setColorAt(0, QColor(200, 40, 40));
        g.setColorAt(1, QColor(120,  0,  0));
        p.setBrush(g);
        p.drawRoundedRect(fill, 4, 4);
    }

    p.setPen(Qt::white);
    p.drawText(frame, Qt::AlignCenter,
               QString("%1 / %2").arg(val_).arg(max_));
}
