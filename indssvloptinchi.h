#ifndef INDSSVLOPTINCHI_H
#define INDSSVLOPTINCHI_H

#include <QDialog>
#include <QTableWidgetItem>

#include "danhsachloptinchi.h"   // LopTinChi, PTRLTC, dsLopTinChi
#include "danhsachdangky.h"     //nodeDK

namespace Ui {
class InDSSVLopTinChi;
}

class InDSSVLopTinChi : public QDialog
{
    Q_OBJECT

public:
    explicit InDSSVLopTinChi(QWidget *parent = nullptr);
    ~InDSSVLopTinChi();

private slots:
    void on_Quaylai_clicked();

    void on_EditNienKhoa_textChanged(const QString &arg1);

    void on_EditHocKy_textChanged(const QString &arg1);

    void on_EditNhom_textChanged(const QString &arg1);

    void on_EditMaMon_textChanged(const QString &arg1);

private:
    Ui::InDSSVLopTinChi *ui;
    LopTinChi* m_ltc;

    void HienThiTatCa();
    void LocDanhSach();
};

#endif // INDSSVLOPTINCHI_H
