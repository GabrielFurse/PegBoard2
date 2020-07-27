#ifndef PEG_H
#define PEG_H

#include "mainwindow.h" //need for mapdataobj struct - change this?
#include "ratingicon.h"

#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <qframe.h>


namespace Ui {
class Peg;
}


class Peg : public QFrame
{
    Q_OBJECT

public:
    explicit Peg(QWidget *parent = nullptr);
    ~Peg();

    void setName(QString);
    void setIndex(int);
    QString getName();
    int getIndex();

    //elo
    void setRating(double x);
    double getRating();

    //glicko
    struct glickoRating_s{
        //glicko rating of a player data type
        double mean;
        double deviation;
        double volitility;
    };

    struct glicko_matchInfo{
        //glicko match info data type
        Peg::glickoRating_s compositeteam, compositeopponent;
        double score;
    };

    void glicko_setRating(double mean, double deviation, double volitility);
    glickoRating_s glicko_getRating();
    void glicko_addToMatchQueue(glicko_matchInfo matchInfo);
    std::vector<glicko_matchInfo> glicko_getMatchQueue();
    void glicko_clearMatchQueue();

    //data
    void setData(MainWindow::mapDataObj);
    MainWindow::mapDataObj getData();
    void addPegToMap();

    //clicked
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    //right click menu
    void deletePlayer();
    void removePlayer();
    void showContextMenu(const QPoint &pos);

    //buttons
    void on_pushButton_test_clicked();


private:
    Ui::Peg *ui;
    int index;
    double rating;
    glickoRating_s glickoRating;
    std::vector<glicko_matchInfo> glicko_matchQueue;
    QByteArray data;
    QHBoxLayout *ratingframe_layout;
    Ratingicon *ratingicon;

};



#endif // PEG_H
