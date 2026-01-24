#include "indssvloptinchi.h"
#include "ui_indssvloptinchi.h"
#include <QHeaderView>

InDSSVLopTinChi::InDSSVLopTinChi(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InDSSVLopTinChi)
{
    ui->setupUi(this);

    ui->tableDSSV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//tự chia đều độ rộng cột

    HienThiTatCa();
}

InDSSVLopTinChi::~InDSSVLopTinChi()
{
    delete ui;
}

void InDSSVLopTinChi::on_Quaylai_clicked()
{
    this->close();
    parentWidget()->show();
}

void InDSSVLopTinChi::HienThiTatCa()
{
    ui->tableDSSV->setRowCount(0);
    int stt = 1;

    for (PTRLTC p = dsLopTinChi; p != nullptr; p = p->next)
    {
        if (p->ltc.huylop) continue;

        for (nodeDK* dk = p->ltc.dssvdk; dk != nullptr; dk = dk->next)
        {
            if (p->ltc.huylop) continue; //nếu đã hủy lớp thì bỏ qua

            int row = ui->tableDSSV->rowCount();
            ui->tableDSSV->insertRow(row);

            ui->tableDSSV->setItem(row, 0,
                                   new QTableWidgetItem(QString::number(p->ltc.MALOPTC)));

            ui->tableDSSV->setItem(row, 1,
                                   new QTableWidgetItem(dk->dk.MASV));

            ui->tableDSSV->setItem(row, 2,
                                   new QTableWidgetItem(p->ltc.MAMH));

            ui->tableDSSV->setItem(row, 3,
                                   new QTableWidgetItem(p->ltc.TENMH));

            ui->tableDSSV->setItem(row, 4,
                                   new QTableWidgetItem(p->ltc.NienKhoa));

            ui->tableDSSV->setItem(row, 5,
                                   new QTableWidgetItem(QString::number(p->ltc.Hocky)));

            ui->tableDSSV->setItem(row, 6,
                                   new QTableWidgetItem(QString::number(p->ltc.Nhom)));
        }
    }
}


void InDSSVLopTinChi::LocDanhSach()
{
    QString nienKhoa = ui->EditNienKhoa->text().trimmed();
    QString maMH     = ui->EditMaMon->text().trimmed().toUpper();
    QString nhomTxt  = ui->EditNhom->text().trimmed();
    QString hocKyTxt = ui->EditHocKy->text().trimmed();

    int nhom  = nhomTxt.isEmpty() ? -1 : nhomTxt.toInt();
    int hocKy = hocKyTxt.isEmpty() ? -1 : hocKyTxt.toInt();

    //xóa các dòng trắng khi lọc bảng
    ui->tableDSSV->clearContents();
    ui->tableDSSV->setRowCount(0);

    for (PTRLTC p = dsLopTinChi; p != nullptr; p = p->next)
    {
        if (!nienKhoa.isEmpty() && p->ltc.NienKhoa != nienKhoa) continue;
        if (!maMH.isEmpty()     && p->ltc.MAMH != maMH) continue;
        if (nhom != -1          && p->ltc.Nhom != nhom) continue;
        if (hocKy != -1         && p->ltc.Hocky != hocKy) continue;
        if (p->ltc.huylop) continue;

        for (nodeDK* dk = p->ltc.dssvdk; dk != nullptr; dk = dk->next)
        {
            if (dk->dk.huydangky) continue; //nếu sv đã hủy đăng ký thì bỏ qua

            int row = ui->tableDSSV->rowCount();
            ui->tableDSSV->insertRow(row);

            ui->tableDSSV->setItem(row, 0,
                                   new QTableWidgetItem(QString::number(p->ltc.MALOPTC)));

            ui->tableDSSV->setItem(row, 1,
                                   new QTableWidgetItem(dk->dk.MASV));

            ui->tableDSSV->setItem(row, 2,
                                   new QTableWidgetItem(p->ltc.MAMH));

            ui->tableDSSV->setItem(row, 3,
                                   new QTableWidgetItem(p->ltc.TENMH));

            ui->tableDSSV->setItem(row, 4,
                                   new QTableWidgetItem(p->ltc.NienKhoa));

            ui->tableDSSV->setItem(row, 5,
                                   new QTableWidgetItem(QString::number(p->ltc.Hocky)));

            ui->tableDSSV->setItem(row, 6,
                                   new QTableWidgetItem(QString::number(p->ltc.Nhom)));
        }
    }
}

//4 thanh lọc
void InDSSVLopTinChi::on_EditNienKhoa_textChanged(const QString &arg1)
{
    LocDanhSach();
}


void InDSSVLopTinChi::on_EditHocKy_textChanged(const QString &arg1)
{
    LocDanhSach();
}


void InDSSVLopTinChi::on_EditNhom_textChanged(const QString &arg1)
{
    LocDanhSach();
}


void InDSSVLopTinChi::on_EditMaMon_textChanged(const QString &arg1)
{
    LocDanhSach();
}

