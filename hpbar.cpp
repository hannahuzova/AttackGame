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
    val_ = qMax(0, qMin(v, max_));
    update();
}

void HPBar::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect bar = rect().adjusted(2, 2, -2, -2);
    p.setBrush(QColor(50, 50, 50));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(bar, 4, 4);

    if (val_ > 0) {
        double ratio = static_cast<double>(val_) / max_;
        int w = static_cast<int>(bar.width() * ratio);
        QRect fill(bar.left(), bar.top(), w, bar.height());
        QLinearGradient g(fill.topLeft(), fill.topRight());
        g.setColorAt(0, QColor(200, 40, 40));
        g.setColorAt(1, QColor(120, 0, 0));
        p.setBrush(g);
        p.drawRoundedRect(fill, 4, 4);
    }

    p.setPen(Qt::white);
    p.drawText(bar, Qt::AlignCenter, QString::number(val_) + " / " + QString::number(max_));
}
