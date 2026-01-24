#ifndef THEMLOPTINCHI_H
#define THEMLOPTINCHI_H

#include <QDialog>
#include "danhsachloptinchi.h"  // để biết kiểu LopTinChi


namespace Ui {
class ThemLopTinChi;
}

class ThemLopTinChi : public QDialog
{
    Q_OBJECT

public:
    explicit ThemLopTinChi(QWidget *parent = nullptr);
    ~ThemLopTinChi();

    void setEditMode(const LopTinChi &ltc);

private slots:
    void on_btnLuu_clicked();
    void on_btnHuy_clicked();

private:
    Ui::ThemLopTinChi *ui;
};

#endif // THEMLOPTINCHI_H
