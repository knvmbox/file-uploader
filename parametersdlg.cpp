#include "parametersdlg.hpp"
#include "ui_parametersdlg.h"


//-----------------------------------------------------------------------------
ParametersDlg::ParametersDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersDlg)
{
    ui->setupUi(this);

    ui->secretKeyEdit->setText(m_settings.secretKey().c_str());
    ui->thumbSecretKeyEdit->setText(m_settings.thumbSecretKey().c_str());

    ui->thumbsSizeBox->setValue(m_settings.thumbSize());
}

//-----------------------------------------------------------------------------
ParametersDlg::~ParametersDlg()
{
    m_settings.setSecretKey(ui->secretKeyEdit->text().toStdString());
    m_settings.setThumbSecretKey(ui->thumbSecretKeyEdit->text().toStdString());

    m_settings.setThumbSize(ui->thumbsSizeBox->value());

    delete ui;
}
