#include "themloptinchi.h"
#include "ui_themloptinchi.h"
#include "danhsachloptinchi.h"
#include "quanlyloptinchi.h"

#include <QMessageBox>
#include <QStringList>

bool isEdit = false;      // true nếu đang sửa
int maLTC_cu = 0;         // lưu MALOPTC gốc khi sửa
extern int MALOPTC_TIEPTHEO;

ThemLopTinChi::ThemLopTinChi(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ThemLopTinChi)
{
    ui->setupUi(this);

    ui->spinMin->setRange(20, 200);
    ui->spinMax->setRange(20, 200);

    ui->spinHocKy->setRange(1, 3);
    ui->spinNhom->setRange(1, 100);

    isEdit = false;
    ui->txtMaMH->setEnabled(true);
    ui->txtNienKhoa->setEnabled(true);
    ui->spinHocKy->setEnabled(true);
    ui->spinNhom->setEnabled(true);

}

void ThemLopTinChi::setEditMode(const LopTinChi &ltc) {

    isEdit = true;
    maLTC_cu = ltc.MALOPTC;

    ui->txtMaMH->setText(ltc.MAMH);
    ui->txtNienKhoa->setText(ltc.NienKhoa);
    ui->spinHocKy->setValue(ltc.Hocky);
    ui->spinNhom->setValue(ltc.Nhom);
    ui->spinMin->setValue(ltc.sosvmin);
    ui->spinMax->setValue(ltc.sosvmax);
    ui->checkHuy->setChecked(ltc.huylop);

    ui->txtMaMH->setEnabled(true);
    ui->txtNienKhoa->setEnabled(true);
    ui->spinHocKy->setEnabled(true);
    ui->spinNhom->setEnabled(true);
}


ThemLopTinChi::~ThemLopTinChi()
{
    delete ui;
}
//luu trong themloptinchi
void ThemLopTinChi::on_btnLuu_clicked()
{
    QString maMH = ui->txtMaMH->text().trimmed().toUpper();
    QString nienKhoa = ui->txtNienKhoa->text().trimmed();
    int hocKy = ui->spinHocKy->value();
    int nhom = ui->spinNhom->value();
    int min = ui->spinMin->value();
    int max = ui->spinMax->value();
    bool huyLop = ui->checkHuy->isChecked();

    if (maMH.isEmpty() || nienKhoa.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập đầy đủ thông tin lớp tín chỉ!");
        return;
    }

    if (min > max) {
        QMessageBox::warning(this, "Lỗi", "Số SV Min không được lớn hơn Max!");
        return;
    }

    QStringList years = nienKhoa.split("-"); // Tách chuỗi bằng dấu gạch ngang
    if (years.size() != 2) {
        QMessageBox::warning(this, "Lỗi", "Định dạng niên khóa sai! Vui lòng nhập: YYYY-YYYY (VD: 2024-2025)");
        return;
    }
    bool ok1, ok2;
    int namDau = years[0].toInt(&ok1);
    int namSau = years[1].toInt(&ok2);

    if (!ok1 || !ok2) {
        QMessageBox::warning(this, "Lỗi", "Niên khóa phải là số!");
        return;
    }
    if (namDau >= namSau) {
        QMessageBox::warning(this, "Lỗi", "Năm kết thúc phải lớn hơn năm bắt đầu!");
        return;
    }
    // (Tùy chọn) Kiểm tra khoảng cách năm, ví dụ chỉ được cách nhau 1 năm
    if (namSau - namDau > 1) {
        QMessageBox::warning(this, "Lỗi", "Niên khóa thường chỉ kéo dài 1 năm (VD: 2023-2024)!");
        return;
    }

    // Kiểm tra mã môn có tồn tại
    if (!dsMonHoc.contains(maMH)) {
        QMessageBox::warning(this, "Lỗi", QString("Mã môn học '%1' không tồn tại!").arg(maMH));
        return;
    }

    // Kiểm tra nhóm trùng (chỉ khi thêm mới)
    PTRLTC pCheck = dsLopTinChi;
    bool biTrung = false;
    while(pCheck) {
        // Nếu trùng thông tin định danh (Môn + NK + HK + Nhóm)
        if(pCheck->ltc.MAMH == maMH &&
            pCheck->ltc.NienKhoa == nienKhoa &&
            pCheck->ltc.Hocky == hocKy &&
            pCheck->ltc.Nhom == nhom &&
            !pCheck->ltc.huylop)
        {
            // Nếu đang thêm mới -> Chắc chắn trùng
            if (!isEdit) {
                biTrung = true;
                break;
            }
            // Nếu đang sửa -> Kiểm tra xem cái lớp bị trùng có phải chính là lớp mình đang sửa không?
            // Nếu MALOPTC khác nhau nghĩa là trùng với lớp khác -> Lỗi
            if (isEdit && pCheck->ltc.MALOPTC != maLTC_cu) {
                biTrung = true;
                break;
            }
        }
        pCheck = pCheck->next;
    }

    if (biTrung) {
        QMessageBox::warning(this, "Lỗi", "Lớp tín chỉ này (Môn, NK, HK, Nhóm) đã tồn tại!");
        return;
    }

    LopTinChi ltc;
    if(!isEdit){
        ltc.MALOPTC = MALOPTC_TIEPTHEO++;  // gán và tăng MALOPTC tiếp theo
        ltc.dssvdk = nullptr;
    } else {
        ltc.MALOPTC = maLTC_cu;    // giữ MALOPTC cũ khi sửa
        PTRLTC pOld = dsLopTinChi;
        while(pOld && pOld->ltc.MALOPTC != maLTC_cu) pOld = pOld->next;
        if(pOld) ltc.dssvdk = pOld->ltc.dssvdk;
        else ltc.dssvdk = nullptr;
    }


    ltc.MAMH = maMH;
    ltc.NienKhoa = nienKhoa;
    ltc.Hocky = hocKy;
    ltc.Nhom = nhom;
    ltc.sosvmin = min;
    ltc.sosvmax = max;
    ltc.huylop = huyLop;
    ltc.TENMH = dsMonHoc.value(maMH);
    //ltc.dssvdk = nullptr;
    if(isEdit){
        // Tìm node theo maLTC_cu và cập nhật thông tin
        PTRLTC p = dsLopTinChi;
        while(p){
            if(p->ltc.MALOPTC == maLTC_cu){
                p->ltc = ltc;   // cập nhật toàn bộ dữ liệu
                break;
            }
            p = p->next;
        }
    }else{
        // thêm mới
        ThemLopTinChiVaoDanhSach(dsLopTinChi, ltc);
    }

    // Lưu và thông báo
    LuuDanhSachLopTinChi(dsLopTinChi);
    QMessageBox::information(this, "Thành công", isEdit ? "Đã sửa lớp tín chỉ!" : "Đã thêm lớp tín chỉ mới!");

    if(parentWidget()) {
        parentWidget()->show();
        QuanLyLopTinChi *parentQL = qobject_cast<QuanLyLopTinChi*>(parentWidget());
        if(parentQL)
            parentQL->HienThiDanhSach();
    }

    this->close();
}

//huy chỉnh sửa lớp
void ThemLopTinChi::on_btnHuy_clicked()
{
    this->close();
    if(parentWidget())
        parentWidget()->show();
}
