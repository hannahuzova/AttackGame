#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <array>

class QStackedWidget;
class GameWidget;
class QToolButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void refreshButtons();
    void startLevel(int level);
    void onLevelCompleted(int level);

private:
    QWidget*  createMenu();
    QWidget*  createRules();
    QWidget*  createLevelSelect();
    void      applyDarkStyle(QWidget* root);

    QStackedWidget* stack_          = nullptr;
    QWidget*        rules_          = nullptr;
    QWidget*        levelSelect_    = nullptr;
    GameWidget*     game_           = nullptr;

    std::array<QToolButton*,4> levelBtns_{};
    int unlockedLevel_ = 1;
};
#endif
