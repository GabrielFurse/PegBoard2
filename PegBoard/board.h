#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QVBoxLayout>

namespace Ui {
class Board;
}

class Board : public QWidget
{
    Q_OBJECT

public:
    explicit Board(QWidget *parent = nullptr);
    ~Board();
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void gameFinished(QVBoxLayout *gamelayout);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Board *ui;
    QVBoxLayout *court1;
    QVBoxLayout *court2;
    QVBoxLayout *court3;
    QVBoxLayout *court4;
};

#endif // BOARD_H
