#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QVBoxLayout>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addToScrollArea(QWidget *widget);
    void hidePlayer(int index);
    void addPeg();

    //data
    struct mapDataObj{
        int index;
        QString name;
        double mean;
        double deviation;
        double volitility;
    };
    std::map<const int, mapDataObj> getMap();
    void addToMap(int key,  mapDataObj data);
    void loadMap();
    void loadPegs();
    void removeFromMap(int key);
    void savePegs(bool update = true);

    //drag and drop
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;


private slots:
    void on_actionSave_triggered();

    void on_actionUpdate_Ratings_triggered();

    void on_actionAdd_Players_triggered();

    void on_actionAdd_new_player_triggered();

private:
    Ui::MainWindow *ui;
    QVBoxLayout *vlayout;
    QHBoxLayout *frame_board_layout;
    int currentIndex;
    std::map<const int, mapDataObj> savedmap;
    std::vector<int> playersInSession;
    std::vector<int> hiddenPlayers;

protected:
    void closeEvent(QCloseEvent *event) override;

};
#endif // MAINWINDOW_H
