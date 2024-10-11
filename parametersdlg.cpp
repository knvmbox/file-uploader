#include "model/imagebanconfigviewmodel.hpp"
#include "parametersdlg.hpp"
#include "settings.hpp"
#include "ui_parametersdlg.h"


//-----------------------------------------------------------------------------
ParametersDlg::ParametersDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParametersDlg)
{
    ui->setupUi(this);

    connect(ui->closeBtn, &QPushButton::clicked, [this](bool){
        accept();
    });
    connect(ui->paramsList, &QListWidget::currentRowChanged, [this](int row){
        ui->stackedWidget->setCurrentIndex(row);
    });

    Settings settings;
    ui->imageSizeBox->setValue(settings.imageSize());

    ui->imagebanListView->setModel(new ImagebanConfigViewModel{this});
    ui->imagebanListView->verticalHeader()->setDefaultSectionSize(24);
    ui->imagebanListView->horizontalHeader()->resizeSection(0, 250);
    ui->imagebanListView->horizontalHeader()->setStretchLastSection(true);
}

//-----------------------------------------------------------------------------
ParametersDlg::~ParametersDlg()
{
    Settings settings;
    settings.setImageSize(ui->imageSizeBox->value());

    delete ui;
}
