#include "quanlyloptinchi.h"
#include "ui_quanlyloptinchi.h"
#include "themloptinchi.h"
#include "danhsachdangky.h"
#include "indssvloptinchi.h"

#include <QMessageBox>
#include <QIntValidator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

QuanLyLopTinChi::QuanLyLopTinChi(QWidget *parent)
    : QDialog(parent), ui(new Ui::QuanLyLopTinChi)
{
    ui->setupUi(this);

    //================4lineedit================
    connect(ui->editNienKhoa, &QLineEdit::textChanged,
            this, &QuanLyLopTinChi::HienThiDanhSachLoc);
    connect(ui->editHocKy, &QLineEdit::textChanged,
            this, &QuanLyLopTinChi::HienThiDanhSachLoc);
    connect(ui->editMaMon, &QLineEdit::textChanged,
            this, &QuanLyLopTinChi::HienThiDanhSachLoc);
    connect(ui->editNhom, &QLineEdit::textChanged,
            this, &QuanLyLopTinChi::HienThiDanhSachLoc);
    //==================================

    // set column
    ui->banghienthi->setColumnCount(8);
    ui->banghienthi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->banghienthi->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QRegularExpression reNienKhoa("^\\d{4}-\\d{4}$");
    ui->editNienKhoa->setValidator(new QRegularExpressionValidator(reNienKhoa, this));
    ui->editHocKy->setValidator(new QIntValidator(0, 10, this));

    XoaDanhSachLopTinChi(dsLopTinChi);
    DocDanhSachLopTinChi(dsLopTinChi);
    DanhSachDangKy temp;
    temp.LoadFileDangKy();//đọc file

    HienThiDanhSach();
}

QuanLyLopTinChi::~QuanLyLopTinChi() { delete ui; }

void QuanLyLopTinChi::on_btnThem_clicked() {
    ThemLopTinChi *event = new ThemLopTinChi(this);
    event->show();
    this->hide();
}

void QuanLyLopTinChi::on_btnQuayLai_clicked() {
    this->close();
    if(parentWidget()) parentWidget()->show();
}

void QuanLyLopTinChi::on_btnSua_clicked()
{
    int row = ui->banghienthi->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn lớp cần sửa!");
        return;
    }

    // Lấy MALOPTC
    int maLTC = ui->banghienthi->item(row, 0)->text().toInt();

    // Tìm lớp trong danh sách
    PTRLTC p = dsLopTinChi;
    while (p && p->ltc.MALOPTC != maLTC)
        p = p->next;

    if (!p) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy lớp tín chỉ!");
        return;
    }

    // Mở form sửa
    ThemLopTinChi *form = new ThemLopTinChi(this);

    form->setEditMode(p->ltc);

    form->exec();
}



void QuanLyLopTinChi::on_btnXoa_clicked() {
    int row = ui->banghienthi->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn lớp cần xóa!");
        return;
    }

    int maLTC = ui->banghienthi->item(row,0)->text().toInt();

    PTRLTC p = dsLopTinChi, prev = nullptr;
    while (p && p->ltc.MALOPTC != maLTC) {
        prev = p;
        p = p->next;
    }

    if (!p) return;

    // ràng buộc nếu có sv đã đk thì không được hủy
    if (p->ltc.dssvdk != nullptr) {
        QMessageBox::warning(this,
                             "Không thể xóa",
                             "Lớp đã có sinh viên đăng ký.\n"
                             "Không được phép xóa lớp này!");
        return;
    }

    if (QMessageBox::question(this, "Xác nhận",
                              "Bạn có chắc muốn xóa lớp này?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    if (prev == nullptr) dsLopTinChi = p->next;
    else prev->next = p->next;

    delete p;
    LuuDanhSachLopTinChi(dsLopTinChi);
    HienThiDanhSach();
}

//đổ dữ liệu lên bảng
void QuanLyLopTinChi::HienThiDanhSach() {
    ui->banghienthi->setRowCount(0);
    DocDanhSachMonHoc(); // đọc tên môn học

    PTRLTC p = dsLopTinChi;
    int rowCount = 0;
    while (p) { rowCount++; p = p->next; }//đếm xem có bao nhiêu dòng
    ui->banghienthi->setRowCount(rowCount);//sau đó mới set dòng để tránh mở rộng bảng liên tục

    p = dsLopTinChi;
    int row = 0;
    while (p) {
        // Cột 0: Mã lớp TC
        ui->banghienthi->setItem(row, 0, new QTableWidgetItem(
                                             QString("%1").arg(p->ltc.MALOPTC, 3, 10, QChar('0'))
                                             ));

        // Cột 1: Mã môn
        ui->banghienthi->setItem(row, 1, new QTableWidgetItem(p->ltc.MAMH));

        // Cột 2: Tên môn học
        QString tenMH = dsMonHoc.value(p->ltc.MAMH, "Chưa có tên môn");
        ui->banghienthi->setItem(row, 2, new QTableWidgetItem(tenMH));

        // Cột 3: Niên khóa
        ui->banghienthi->setItem(row, 3, new QTableWidgetItem(p->ltc.NienKhoa));

        // Cột 4: Học kỳ
        ui->banghienthi->setItem(row, 4, new QTableWidgetItem(QString::number(p->ltc.Hocky)));

        // Cột 5: Nhóm
        ui->banghienthi->setItem(row, 5, new QTableWidgetItem(QString::number(p->ltc.Nhom)));

        // Cột 6: SV min
        ui->banghienthi->setItem(row, 6, new QTableWidgetItem(QString::number(p->ltc.sosvmin)));

        // Cột 7: SV max
        ui->banghienthi->setItem(row, 7, new QTableWidgetItem(QString::number(p->ltc.sosvmax)));

        row++;
        p = p->next;
    }

    // đảm bảo cột MALOPTC hiện
    ui->banghienthi->setColumnHidden(0, false);
}

//dùng để lọc theo các tham chiếu niên khóa, học kỳ, nhóm, mã môn
void QuanLyLopTinChi::HienThiDanhSachLoc()
{
    DocDanhSachMonHoc();

    QString nienKhoa = ui->editNienKhoa->text().trimmed();
    QString maMH     = ui->editMaMon->text().trimmed().toUpper();
    QString hocKyStr = ui->editHocKy->text().trimmed();
    QString nhomStr  = ui->editNhom->text().trimmed();

    // Không nhập gì → hiện toàn bộ
    if (nienKhoa.isEmpty() && maMH.isEmpty() &&
        hocKyStr.isEmpty() && nhomStr.isEmpty())
    {
        HienThiDanhSach();
        return;
    }

    int hocKy = hocKyStr.isEmpty() ? -1 : hocKyStr.toInt();
    int nhom  = nhomStr.isEmpty()  ? -1 : nhomStr.toInt();

    ui->banghienthi->clearContents();
    ui->banghienthi->setRowCount(0);

    PTRLTC p = dsLopTinChi;
    int row = 0;

    while (p)
    {
        bool ok = true;

        if (!nienKhoa.isEmpty() && p->ltc.NienKhoa != nienKhoa) ok = false;
        if (!maMH.isEmpty()     && p->ltc.MAMH    != maMH)     ok = false;
        if (hocKy != -1         && p->ltc.Hocky   != hocKy)   ok = false;
        if (nhom  != -1         && p->ltc.Nhom    != nhom)    ok = false;

        if (ok)
        {
            ui->banghienthi->insertRow(row);

            ui->banghienthi->setItem(row, 0,
                                     new QTableWidgetItem(QString("%1").arg(p->ltc.MALOPTC,3,10,QChar('0'))));
            ui->banghienthi->setItem(row, 1, new QTableWidgetItem(p->ltc.MAMH));
            ui->banghienthi->setItem(row, 2,
                                     new QTableWidgetItem(dsMonHoc.value(p->ltc.MAMH,"Chưa có tên")));
            ui->banghienthi->setItem(row, 3, new QTableWidgetItem(p->ltc.NienKhoa));
            ui->banghienthi->setItem(row, 4, new QTableWidgetItem(QString::number(p->ltc.Hocky)));
            ui->banghienthi->setItem(row, 5, new QTableWidgetItem(QString::number(p->ltc.Nhom)));
            ui->banghienthi->setItem(row, 6, new QTableWidgetItem(QString::number(p->ltc.sosvmin)));
            ui->banghienthi->setItem(row, 7, new QTableWidgetItem(QString::number(p->ltc.sosvmax)));

            row++;
        }

        p = p->next;
    }
}



void QuanLyLopTinChi::on_btnIn_clicked()
{
    InDSSVLopTinChi *w = new InDSSVLopTinChi(this);
    w->show();
    this->hide();
}
