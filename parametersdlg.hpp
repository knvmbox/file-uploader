#ifndef PARAMETERSDLG_HPP
#define PARAMETERSDLG_HPP

#include <QDialog>


namespace Ui {
class ParametersDlg;
}

class ParametersDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ParametersDlg(QWidget *parent = nullptr);
    ~ParametersDlg();

private:
    Ui::ParametersDlg *ui;
};

#endif // PARAMETERSDLG_HPP
