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
        setScaledContents(true);        // ровно вся картинка-аура
        setFixedSize(pm.size());        // больше не обрезаем её до 80×80
        setAttribute(Qt::WA_TranslucentBackground);
    }
};
#endif
