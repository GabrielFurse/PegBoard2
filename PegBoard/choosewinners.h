#ifndef CHOOSEWINNERS_H
#define CHOOSEWINNERS_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ChooseWinners;
}

class ChooseWinners : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseWinners(QWidget *parent = nullptr);
    ~ChooseWinners();
     void addToList(QString str, int id);
     QList<int> getIds();
     void setTitle(QString title);


private:
    Ui::ChooseWinners *ui;
};

#endif // CHOOSEWINNERS_H
