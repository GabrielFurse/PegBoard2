#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "peg.h"
#include "board.h"
#include "ratingsystem.h"
#include "choosewinners.h"

#include <QMouseEvent>
#include <QDrag>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QtWidgets>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include <QInputDialog>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      vlayout(new QVBoxLayout),
      frame_board_layout(new QHBoxLayout)
{

    //Peg list

    ui->setupUi(this);
    ui->frame->setLayout(vlayout);
    ui->scrollArea->setWidget(ui->frame);
    vlayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    //Board

    ui->frame_board->setLayout(frame_board_layout);
    Board *board = new Board();
    frame_board_layout->addWidget(board);

    //load save
    loadMap();
    //loadPegs();

    update(); //for gemotry of vlayout
}

MainWindow::~MainWindow()
{
    delete ui;
    delete vlayout;
    delete frame_board_layout;

}

//overrides
// ////////////////////////////////////////////

void MainWindow::closeEvent(QCloseEvent *event)
//ask if user wants to save on close
    {
    QMessageBox *dialog = new QMessageBox;
    dialog->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    dialog->setDefaultButton(QMessageBox::Yes);
    dialog->setText("Do you want to save and update current players?");

    int answer = dialog->exec();

    switch (answer){
    case QMessageBox::Yes:
        savePegs(); //also updates glicko ratings
        break;
    case QMessageBox::No:
        //not saving
        break;

    case QMessageBox::Cancel:
        event->ignore();
        return;

    }

    event->accept();
}





//access to members
// /////////////////////////////////////////////

void MainWindow::addToScrollArea(QWidget *widget)
//adds widget to the scroll area
{
    vlayout->addWidget(widget);
}

void MainWindow::hidePlayer(int index)
//adds index to hiddenPlayers
{
    hiddenPlayers.push_back(index);
}

void MainWindow::addToMap(int key, mapDataObj data)
//adds data to savedmap. will replace if key already exists
{
    std::pair<std::map<int, mapDataObj>::iterator,bool> ret;
    ret = savedmap.insert({key, data});
    if(ret.second == false){
        savedmap[key] = data; //TEST THIS
     }

}

std::map<const int, MainWindow::mapDataObj> MainWindow::getMap()
//returns savedmap
{
    return savedmap;
}

void MainWindow::removeFromMap(int key)
//removes data from map with index key
{
    savedmap.erase(savedmap.find(key));
}



//saving and loading
// //////////////////////////////////

void MainWindow::addPeg()
{
    QString newname = QInputDialog::getText(this, "New Peg", "Name:");
    if(newname == 0){
        return;
    }
    currentIndex += 1;
    Peg *peg = new Peg(this);

    peg->setIndex(currentIndex);
    peg->setName(newname);

    //qDebug() << currentIndex;
    vlayout->addWidget(peg);
    //save peg to map
    savedmap.insert({peg->getIndex(),peg->getData()});
    playersInSession.push_back(peg->getIndex());
}

void MainWindow::savePegs( bool update)
//Saves all pegs. rewrites current save. This is where glicko rating update happens! update defaults to true.
{
    //update data in savedmap as if pegs did not complete, this is overwritten by any which did compete by addToMap()
    for(auto &x : savedmap){
        double newdeviation = sqrt(pow(x.second.deviation,2) + pow(x.second.volitility, 2));
        x.second.deviation = std::min(newdeviation, 2.015);
    }


    //take pegs from scroll area, update the ratings, and add them to savedmap
    int playersingame = vlayout->count();
    for(int j=0; j < playersingame; j++ )
    {
        QLayoutItem *child1;
        if((child1 = vlayout->takeAt(j)) != 0)
        {
            Peg *peg = static_cast<Peg*>(child1->widget());
            if(update){ glicko_updatePeg(peg);} //update ratings

            peg->addPegToMap();
            vlayout->insertWidget(j, peg); //put pegs back where they came from
        }
    }

    //save savedmap to file
    QFile file("C:/Users/gabri/desktop/pegdata.txt");
    file.open(QIODevice::WriteOnly);
    QTextStream datastream(&file);
    datastream << int(savedmap.size()) << Qt::endl;

    for(auto const& x : savedmap){
        datastream << x.second.index << ' ' << x.second.name << ' '<< x.second.mean << ' '<< x.second.deviation<< ' ' << x.second.volitility<< ' ' << Qt::endl;
    }

    file.close();
}

void MainWindow::loadMap()
//initialize saved map with data from file
{
    currentIndex = 0; //when creating pegs insures index is unique

    //load from file
    QFile file("C:/Users/gabri/desktop/pegdata.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream datastream(&file);
    int size;
    datastream >> size;
    for(int i=0; i < size; i++){
        int newindex;
        QString newmame;
        double mean;
        double deviation;
        double volitility;
        datastream >> newindex >>newmame >> mean >> deviation >> volitility;
        mapDataObj newdata;
        newdata.index = newindex;
        newdata.name = newmame;
        newdata.mean = mean;
        newdata.deviation = deviation;
        newdata.volitility = volitility;

        savedmap.insert({newindex, newdata});

        if(newindex > currentIndex){
            currentIndex = newindex;
        }
    }
    file.close();

}

void MainWindow::loadPegs()
//asks user which pegs they want to add from savedmap, adds them to scrollarea
{
    //select players to add to scroll area can use choosewinners dialog class for this
    ChooseWinners *dialog = new ChooseWinners;

    for(auto const x : savedmap){

        Peg *peg = new(Peg);
        peg->setData(x.second);
        //check if already in session, only show as an option to add if not
        if(std::find(playersInSession.begin(), playersInSession.end(), peg->getIndex()) == playersInSession.end()){
            dialog->addToList(peg->getName(), peg->getIndex());

        }

        //check if player ha been removed and we wish to re add them
        if(std::find(hiddenPlayers.begin(), hiddenPlayers.end(), peg->getIndex()) != hiddenPlayers.end()){
            dialog->addToList(peg->getName(), peg->getIndex());
        }

        delete peg;
    }
    dialog->setTitle("Add Players to Session");
    dialog->exec();


    for(auto const x : savedmap){
        //do stuff with the players selected
        if(dialog->getIds().contains(x.first))
        {
            //deal with hidden players
            if((std::find(hiddenPlayers.begin(), hiddenPlayers.end(), x.first) != hiddenPlayers.end()))
            {
                int playersingame = vlayout->count();
                for(int j=0; j < int(playersingame); j++ )
                {
                   if(vlayout->itemAt(j) != 0)
                   {
                       Peg *peg = static_cast<Peg*>(vlayout->itemAt(j)->widget());
                      if(peg->getIndex() == x.first)
                      {
                          vlayout->itemAt(j)->widget()->show();
                          hiddenPlayers.erase(std::find(hiddenPlayers.begin(), hiddenPlayers.end(), x.first));
                      }
                   }
                }
            }
            //add players
            else
            {
                Peg *peg = new Peg;
                peg->setData(x.second);
                addToScrollArea(peg);
                playersInSession.push_back(peg->getIndex());
            }
        }
    }
}


//Ui stuff
// /////////////

//File menu

void MainWindow::on_actionSave_triggered()
//save
{
    savePegs(false); //don't update
}

void MainWindow::on_actionUpdate_Ratings_triggered()
//save and update
{
    savePegs(); //save and update
}

void MainWindow::on_actionAdd_Players_triggered()
//calls loadPegs()
{
    loadPegs();
}

void MainWindow::on_actionAdd_new_player_triggered()
{
    addPeg();
}



//Drag and Drop

void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    event->setDropAction(Qt::MoveAction);
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event){
    if(event->answerRect().intersects(ui->scrollArea->geometry()))
    {
        event->setDropAction(Qt::MoveAction);
        event->acceptProposedAction();
    }
    else event->ignore();
}


void MainWindow::dropEvent(QDropEvent *event){
    //get data from where drag.exec called
    const QMimeData *mime = event->mimeData();
    QString text;
    int index;
    double mean;
    double deviation;
    double volitility;
    Peg::glickoRating_s rating;
    QByteArray data = mime->data(QStringLiteral("application/x-pegboard"));
    QDataStream dataStream(&data, QIODevice::ReadOnly);
    dataStream >> text >> index >> mean >> deviation >> volitility;

    // create new peg with correct data
    Peg *newpeg = new Peg();
    newpeg->setName(text);
    newpeg->setIndex(index);
    newpeg->glicko_setRating(mean, deviation, volitility);
    int x = 0;
    for(int i=0; i < vlayout->count(); i++)
    {
        if(vlayout->itemAt(i)->geometry().contains(event->pos())){
            vlayout->insertWidget(i-1, newpeg);
            x=1;
        }
    }

    if(x == 0){
        vlayout->addWidget(newpeg);
    }

    //finish drag event
    event->setDropAction(Qt::MoveAction);
    event->acceptProposedAction();
}







