#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <QLabel>

class Projectile : public QLabel {
    Q_OBJECT
public:
    explicit Projectile(const QPixmap& pm, QWidget* parent = nullptr)
        : QLabel(parent)
    {
        setPixmap(pm);
        setScaledContents(true);
        setFixedSize(pm.size()/5);
        setAttribute(Qt::WA_TranslucentBackground);
    }
};
#endif
