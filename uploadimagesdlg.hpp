#ifndef UPLOADIMAGESDLG_HPP
#define UPLOADIMAGESDLG_HPP

#include <string>

#include <QDialog>

#include "settings.hpp"
#include "utils/imageban.hpp"


namespace Ui {
class UploadImagesDlg;
}

class UploadImagesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UploadImagesDlg(std::string secretKey, QWidget *parent = nullptr);
    ~UploadImagesDlg();

    imageban::album_t album();

private:
    void addAlbum(const QString&);
    void albumsSelected(const QString&, const QString&);
    void fillAlbumsBox();
    void showNewAlbumsWidgets(bool);

private:
    Ui::UploadImagesDlg *ui;
    imageban::ImageBan m_imageBan;
};

#endif // UPLOADIMAGESDLG_HPP
