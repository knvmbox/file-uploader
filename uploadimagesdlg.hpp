#ifndef UPLOADIMAGESDLG_HPP
#define UPLOADIMAGESDLG_HPP

#include <string>
#include <utility>

#include <QDialog>
#include <QStringList>

#include <common/logger/logger.hpp>

#include "settings.hpp"
#include "utils/imageban.hpp"


namespace Ui {
class UploadImagesDlg;
}

class UploadImagesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UploadImagesDlg(
        std::string secretKey,
        std::string thumbSecretKey,
        QWidget *parent = nullptr
    );
    ~UploadImagesDlg();

    std::pair<imageban::album_t, imageban::album_t> album();

private:
    void addAlbum(const QString&);
    void albumsSelected(const QString&, const QStringList&);
    void fillAlbumsBox();
    void showNewAlbumsWidgets(bool);

private:
    Ui::UploadImagesDlg *ui;
    std::shared_ptr<common::Logger> m_logger;
    imageban::ImageBan m_imgImageBan;
    imageban::ImageBan m_thumbImageBan;
};

#endif // UPLOADIMAGESDLG_HPP
