#include "board.h"
#include "ui_board.h"
#include "peg.h"
#include "mainwindow.h"
#include "globals.h"
#include "choosewinners.h"
#include "ratingsystem.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include<QVBoxLayout>
#include <QtDebug>
#include <QGridLayout>
#include<QtWidgets>
#include <QDialog>
#include <algorithm>
#include <vector>

using namespace std;
Board::Board(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Board),
    court1(new QVBoxLayout),
    court2(new QVBoxLayout),
    court3(new QVBoxLayout),
    court4(new QVBoxLayout)
{
    ui->setupUi(this);
    //set layouts for courts
    ui->frame_court1->setLayout(court1);
    ui->frame_court2->setLayout(court2);
    ui->frame_court3->setLayout(court3);
    ui->frame_court4->setLayout(court4);

    //put pegs to top of court
    court1->setAlignment(Qt::AlignTop);
    court2->setAlignment(Qt::AlignTop);
    court3->setAlignment(Qt::AlignTop);
    court4->setAlignment(Qt::AlignTop);

    update(); //needed to fix geometry of frames
}

Board::~Board()
{
    delete ui;
    delete court1;
    delete court2;
    delete court3;
    delete court4;
}


//Drag and Drop
// ///////////////////////////////////////////////

void Board::dragEnterEvent(QDragEnterEvent *event)
//accepts any drag event request
{
    event->setDropAction(Qt::MoveAction);
    event->acceptProposedAction();
}

void Board::dropEvent(QDropEvent *event)
//drop peg correctly
{
    //get data from where drag.exec called
    const QMimeData *mime = event->mimeData();
    QString text;
    int index;
    double mean;
    double deviation;
    double volitility;
    QByteArray data = mime->data(QStringLiteral("application/x-pegboard"));
    QDataStream dataStream(&data, QIODevice::ReadOnly);
    dataStream >> text >> index >> mean >> deviation >> volitility;

    // create new peg with correct data
    Peg *newpeg = new Peg();
    newpeg->setName(text);
    newpeg->setIndex(index);
    newpeg->glicko_setRating(mean, deviation, volitility);

    for (int i = 0; i < ui->gridLayout->count(); ++i)
    {
      QWidget *frame = ui->gridLayout->itemAt(i)->widget();
      if(qobject_cast<QFrame*>(frame))  //returns nullpltr if widget does not inherit from QFrame
      {
              //add peg to the frame the curser is at
              if(frame->geometry().contains(event->pos()))
              {
                  frame->layout()->addWidget(newpeg);
              }
      }
    }



    //finish drag event
    event->setDropAction(Qt::MoveAction);
    event->acceptProposedAction();
}

void Board::dragMoveEvent(QDragMoveEvent *event)
//checks whether drop is allowed on cursor
{
    //Checks if cursor is on a frame we can drop in
    if(event->answerRect().intersects(ui->frame_court1->geometry())
            | event->answerRect().intersects(ui->frame_court2->geometry())
            | event->answerRect().intersects(ui->frame_court3->geometry())
            | event->answerRect().intersects(ui->frame_court4->geometry()))
        {       
            event->setDropAction(Qt::MoveAction);
            event->acceptProposedAction();

        }
    //if not, tell event we can't drop here
    else event->ignore();

}



//Games Finished
// //////////////////////////////////////////////

void Board::gameFinished(QVBoxLayout *gamelayout)
//puts pegs back in main window scroll area in correct order and updates ratings. gamelayout = e.g court1
{
    //create popup to choose winners
    ChooseWinners *dialog = new ChooseWinners;
    int playersingame = gamelayout->count();

    //give indexes of pegs in game to dialog
    for(int j=0; j < playersingame; j++ )
    {
        QLayoutItem *child1;
        if((child1 = gamelayout->takeAt(j)) != 0)
        {
            Peg *peg = static_cast<Peg*>(child1->widget());
            QString text = peg->getName();
            int index = peg->getIndex();
            dialog->addToList(text, index);

            gamelayout->insertWidget(j, peg); //put pegs back where they came from
        }

    }

    //open choose winners
    dialog->exec();


    //put winners back first
    vector<Peg*> pegs;


    int j =0;
    while(j < playersingame)
    {
        QLayoutItem *child = gamelayout->takeAt(j);
        if(child != 0)
        {
            Peg *peg = static_cast<Peg*>(child->widget());
            if(dialog->getIds().contains(peg->getIndex()))
            {
                //update anything about winners here through peg
                pegs.insert(pegs.begin(), peg);
                main_window->addToScrollArea(peg);
                delete child;
            }
            else
            {
                //update anything about losers here through peg
                pegs.push_back(peg);
                gamelayout->insertWidget(j, peg);
                delete child;
                j++;

            }
        }
        else j++;
    }

    if (pegs.size() == 4){
        updateRating(pegs[0], pegs[1], pegs[2], pegs[3]);
        glicko_updateMatchQueue(pegs[0], pegs[1], pegs[2], pegs[3]);

    }

    //put remaining pegs back
    QLayoutItem *child;
    while ((child = gamelayout->takeAt(0)) != 0)
    {

        main_window->addToScrollArea(child->widget());
        delete child;
    }

}

//court finished buttons
void Board::on_pushButton_clicked(){
    gameFinished(court1);
}

void Board::on_pushButton_2_clicked()
{
    gameFinished(court2);
}

void Board::on_pushButton_3_clicked()
{
    gameFinished(court3);
}

void Board::on_pushButton_4_clicked()
{
    gameFinished(court4);
}
