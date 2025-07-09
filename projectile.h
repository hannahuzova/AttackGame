#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <QLabel>

class Projectile: public QLabel{
    Q_OBJECT
public:
    explicit Projectile(const QPixmap& pm, QWidget* parent=nullptr)
        : QLabel(parent)
    {
        setPixmap(pm);
        setScaledContents(true);
        setFixedSize(80, 80);        // aura size enlarged
    }
};
#endif
