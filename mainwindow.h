#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
class QStackedWidget;
class GameWidget;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
private:
    QWidget* createMenu();
    QWidget* createRules();
    void applyDarkStyle(QWidget* w);
    QStackedWidget* stack_;
    GameWidget* game_;

    QFont fantasyFont;
};
#endif
