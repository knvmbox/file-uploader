#include <QPushButton>

#include "uploadimagesdlg.hpp"
#include "ui_uploadimagesdlg.h"


//-----------------------------------------------------------------------------
UploadImagesDlg::UploadImagesDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadImagesDlg),
    m_imageBan{"cvqx64FaleJ7dYclfNATYfWDWI2bQ0AqT2i"} {
    ui->setupUi(this);

    showNewAlbumsWidgets(false);

    auto slot = [this](int index) {
        albumsSelected(
            ui->albumsBox->itemText(index),
            ui->albumsBox->itemData(index).toString()
        );
    };

    connect(ui->albumsBox, QOverload<int>::of(&QComboBox::currentIndexChanged), slot);
    connect(ui->addAlbumBtn, &QToolButton::clicked, [=]() {
        addAlbum(ui->albumNameEdit->text());
    });
    connect(ui->reloadAlbumsBtn, &QToolButton::clicked, [=]() {
        fillAlbumsBox();
    });

    fillAlbumsBox();
    slot(ui->albumsBox->currentIndex());
}

//-----------------------------------------------------------------------------
UploadImagesDlg::~UploadImagesDlg() {
    delete ui;
}

//-----------------------------------------------------------------------------
ImageBan::album_t UploadImagesDlg::album() {
    return {
        ui->albumsBox->currentData().toString().toStdString(),
        ui->albumsBox->currentText().toStdString()
    };
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::addAlbum(const QString &str) {
    auto album = m_imageBan.createAlbum(str.toStdString());
    fillAlbumsBox();

    ui->albumsBox->setCurrentText(album.name.c_str());
    ui->albumNameEdit->clear();
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::albumsSelected(const QString &name, const QString &id) {
    showNewAlbumsWidgets(id.isEmpty());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(id.isEmpty());
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::fillAlbumsBox() {
    ui->albumsBox->clear();
    auto list = m_imageBan.albumsList();

    for(const auto &item : list) {
        ui->albumsBox->addItem(
            item.name.c_str(),
            item.id.c_str()
        );
    }
    ui->albumsBox->addItem("Другой", "");
}

//-----------------------------------------------------------------------------
void UploadImagesDlg::showNewAlbumsWidgets(bool state) {
    ui->newAlbumLabel->setVisible(state);
    ui->albumNameEdit->setVisible(state);
    ui->addAlbumBtn->setVisible(state);
}
