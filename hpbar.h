#ifndef HPBAR_H
#define HPBAR_H
#include <QWidget>

class HPBar : public QWidget {
    Q_OBJECT
public:
    explicit HPBar(int maximum = 500, QWidget* parent = nullptr);

    void setValue(int v);
    int value() const { return val_; }
    int maximum() const { return max_; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    int max_;
    int val_;
};
#endif
