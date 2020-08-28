#include "peg.h"
#include "ui_peg.h"
#include "mainwindow.h"
#include "globals.h"
#include "ratingsystem.h"

#include <QDebug>
#include <QByteArray>
#include<QByteArrayData>
#include <QSaveFile>
#include <QFile>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QMenu>
#include <QDialog>
#include <QMessageBox>
#include "ratingicon.h"

Peg::Peg(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Peg),
    ratingframe_layout(new QHBoxLayout),
    ratingicon(new Ratingicon)


{
    ui->setupUi(this);



    index = -2;
    rating = 1000;
    setRating(1000);
    setAttribute(Qt::WA_DeleteOnClose);
    //values for new player
    glicko_setRating(0, 2.015, 0.06);

    //style
    this->setFrameStyle(QFrame::Box);
    this->setFrameShadow(QFrame::Sunken);

    ui->ratingFrame->setLayout(ratingframe_layout);
    ratingframe_layout->addWidget(ratingicon);
    ratingframe_layout->setAlignment(Qt::AlignVCenter);
    //set up right click menu
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
}

Peg::~Peg()
{
    delete ui;
    delete ratingframe_layout;
    delete ratingicon;
}

void Peg::showContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);

   QAction action1("Delete", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(deletePlayer()));
   contextMenu.addAction(&action1);

   QAction action2("Remove Player", this);
   connect(&action2, SIGNAL(triggered()), this, SLOT(removePlayer()));
   contextMenu.addAction(&action2);

   contextMenu.exec(mapToGlobal(pos));

}
//Access members
// /////////////////////////////////

void Peg::setIndex(int x)
{
    index = x;
    ui->label_rating->setText(QString::number(x)); //temp for testing
}

int Peg::getIndex(){
    return index;
}

void Peg::setName(QString name){

    ui->label_name->setText(name);
}

QString Peg::getName(){
    return ui->label_name->text();
}

void Peg::setRating(double x)
//elo rating
{
    rating = x;
    ui->label_rating->setText(QString::number(x));
}

double Peg::getRating()
//elo rating
{
    return rating;
}



//glicko functions

void Peg::glicko_setRating(double mean, double deviation, double volitility)
//set glicko rating to glickoRating struct
{
    glickoRating.mean = mean;
    glickoRating.deviation = deviation;
    glickoRating.volitility = volitility;

    ratingicon->setRating(mean, deviation);
}

Peg::glickoRating_s Peg::glicko_getRating()
//return glickoRating struct
{
    return Peg::glickoRating;
}

void Peg::glicko_addToMatchQueue(glicko_matchInfo matchInfo)
//add match info struct to the match queue to update at end of rating period
{
    Peg::glicko_matchQueue.push_back(matchInfo);
}

std::vector<Peg::glicko_matchInfo> Peg::glicko_getMatchQueue()
//returns match queue
{
    return Peg::glicko_matchQueue;
}

void Peg::glicko_clearMatchQueue()
//clears match queue
{
    glicko_matchQueue = {};
}



//saving data

//data needed to save/initialize:
// - index
// - name
// - glicko_rating struct


void Peg::setData(MainWindow::mapDataObj data)
//sets index, name, glickRaing
{
    setIndex(data.index);
    setName(data.name);
    glicko_setRating(data.mean, data.deviation, data.volitility);
}

MainWindow::mapDataObj Peg::getData()
//returns index, name and glickoRating of this
{
    MainWindow::mapDataObj outdata;

    outdata.index = getIndex();
    outdata.name = getName();
    outdata.mean = glickoRating.mean;
    outdata.deviation = glickoRating.deviation;
    outdata.volitility = glickoRating.volitility;
    return outdata;
}

void Peg::addPegToMap()
//adds data to mainwindow's savedmap
{
    main_window->addToMap(index, this->getData());
}



//mouse event
void Peg::mousePressEvent(QMouseEvent *event){
//    if(event->button() == Qt::RightButton)
//    {
//        qDebug() << "clicked";
//    }

    if(event->button() == Qt::LeftButton){
       //start drag

        QDrag *drag = new QDrag(this);

        //create data
        QMimeData *mimeData = new QMimeData;
        QByteArray data;
        QDataStream dataStream(&data, QIODevice::WriteOnly);
        dataStream << this->getName() << this->getIndex() << this->glicko_getRating().mean << this->glicko_getRating().deviation << this->glicko_getRating().volitility;
        mimeData->setData(QStringLiteral("application/x-pegboard"), data);
        drag->setMimeData(mimeData);

        Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
        if (dropAction == Qt::MoveAction)
        {
            //copy is created on drop so wish to close this
            this->close();
        }
    }
}



//slots

//right click menu

void Peg::deletePlayer()
//deletes this and data from mainwindow.savedmap
{
    //double check if user is sure
    QMessageBox *dialog = new QMessageBox;
    dialog->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    dialog->setDefaultButton(QMessageBox::Yes);
    dialog->setText("This will permenantly remove this player. Do you want to continue?");

    int answer = dialog->exec();

    if(answer == QMessageBox::Yes){
        main_window->removeFromMap(index);
        this->deleteLater();
    }

}

void Peg::removePlayer()
//hides this
{
    this->hide();
    main_window->hidePlayer(this->getIndex()); //tells main window this is hidden
}



