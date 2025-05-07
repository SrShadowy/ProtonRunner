#ifndef CUSTOMPROTON_H
#define CUSTOMPROTON_H

#include <QDialog>

namespace Ui {
class customproton;
}

class customproton : public QDialog
{
    Q_OBJECT

public:
    explicit customproton(QWidget *parent = nullptr);
    ~customproton();

private slots:
    void on_customproton_accepted();

private:
    Ui::customproton *ui;
};

#endif // CUSTOMPROTON_H
