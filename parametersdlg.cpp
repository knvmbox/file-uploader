#include "parametersdlg.hpp"
#include "ui_parametersdlg.h"


//-----------------------------------------------------------------------------
ParametersDlg::ParametersDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersDlg)
{
    ui->setupUi(this);

    ui->secretKeyEdit->setText(m_settings.secretKey().c_str());
}

//-----------------------------------------------------------------------------
ParametersDlg::~ParametersDlg()
{
    m_settings.setSecretKey(ui->secretKeyEdit->text().toStdString());

    delete ui;
}
