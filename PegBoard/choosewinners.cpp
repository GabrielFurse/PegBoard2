#include "choosewinners.h"
#include "ui_choosewinners.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QAbstractItemView>
#include <QDebug>

//Dialog class for dialog used in Board::gameFinished()
ChooseWinners::ChooseWinners(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseWinners)
{
    ui->setupUi(this);
    ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection );
    setWindowTitle("Choose Winners" );
}

ChooseWinners::~ChooseWinners()
{
    delete ui;
}

void ChooseWinners::addToList(QString str, int id){
    QListWidgetItem *item = new QListWidgetItem(str , ui->listWidget);
    item->setData(Qt::UserRole, id);


}


QList<int> ChooseWinners::getIds(){
    QList<int> ids;
    foreach (QListWidgetItem* item, ui->listWidget->selectedItems())
       ids += item->data(Qt::UserRole).toInt();

    ids += -1;

    return ids;
}

void ChooseWinners::setTitle(QString title){
    setWindowTitle(title);
}
