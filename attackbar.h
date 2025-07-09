#ifndef ATTACKBAR_H
#define ATTACKBAR_H
#include <QWidget>
class QTimer;
class AttackBar: public QWidget{
    Q_OBJECT
public:
    explicit AttackBar(QWidget* parent=nullptr);
    void start();
public slots:
    void handleClick();
signals:
    void stopped(int power);
protected:
    void mousePressEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;
private:
    QTimer* timer_; int pos_; int dir_; bool running_;
};
#endif
