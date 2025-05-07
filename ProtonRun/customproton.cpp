#include "customproton.h"
#include "ui_customproton.h"
#include "configpath.h"

customproton::customproton(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::customproton)
{
    ui->setupUi(this);
}

customproton::~customproton()
{
    delete ui;
}

void customproton::on_customproton_accepted()
{
    ConfigPath::get()->SyncUi(ui);
    ConfigPath::get()->SaveSettings();

}

