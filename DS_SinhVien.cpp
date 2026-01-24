#include "DS_SinhVien.h"
#include "DS_Lop.h"
#include "ui_DS_SinhVien.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <cstring>

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>

using namespace std;

DS_SV::DS_SV(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DS_SV)
{
    ui->setupUi(this);
    FirstSV = nullptr;
    CurrentMALOP[0] = '\0';
    nLop = 0; // số lớp hiện có

    // load danh sách lớp từ file
    LoadDSLop("dslopsv.txt");

    ui->tableSV->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableSV->setSelectionBehavior(QAbstractItemView::SelectRows);

    // gắn nút Enter
    connect(ui->editMALOP, &QLineEdit::returnPressed, this, &DS_SV::onBtnTimKiemClicked);
    connect(ui->btnTimKiem, &QPushButton::clicked, this, &DS_SV::onBtnTimKiemClicked);
    connect(ui->btnXemLop, &QPushButton::clicked, this, &DS_SV::onBtnXemLop_clicked);

}

//dọn dẹp bộ nhớ cho sv đã bị xóa
DS_SV::~DS_SV() {
    PTRSV p = FirstSV;
    while(p) { PTRSV tmp = p; p = p->next; delete tmp; }
    delete ui;
}

//đọc danh sách lớp từ file txt
void DS_SV::LoadDSLop(const char* filename) {
    nLop = 0;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    while(!in.atEnd() && nLop < 10000) {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if(parts.size() < 3) continue;

        // [QUAN TRỌNG] Kiểm tra nếu lớp đã tồn tại thì KHÔNG thêm nữa
        QString maLop = parts[0].trimmed().toUpper();
        bool isDuplicate = false;
        for(int i = 0; i < nLop; i++) {
            if(strcmp(dslop[i].MALOP, maLop.toStdString().c_str()) == 0) {
                isDuplicate = true;
                break;
            }
        }
        if(isDuplicate) continue;

        // Thêm lớp vào mảng
        strncpy(dslop[nLop].MALOP, maLop.toStdString().c_str(), 15); dslop[nLop].MALOP[15] = '\0';
        strncpy(dslop[nLop].TENLOP, parts[1].toStdString().c_str(), 50); dslop[nLop].TENLOP[50] = '\0';
        strncpy(dslop[nLop].NAMHOC, parts[2].toStdString().c_str(), 9); dslop[nLop].NAMHOC[9] = '\0';
        nLop++;
    }
    file.close();
}

int DS_SV::TimLop(const char* malop)
{
    for(int i=0; i<nLop; i++)
        if(strcmp(dslop[i].MALOP, malop) == 0)
            return i;//nếu tìm thấy trả về mã index của lớp đó
    return -1;//không tìm thấy trả về -1
}


// Tìm sinh viên theo MASV
PTRSV DS_SV::TimSV(const char* masv)
{
    PTRSV p = FirstSV;
    while(p)
    {
        if(strcmp(p->sv.MASV, masv)==0)//dùng con trỏ so sánh strcmp với mỗi mã sv của node
            return p;//nếu tìm thấy thì trả về p
        p = p->next;
    }
    return nullptr;//nếu không tìm thấy thì rỗng
}

// hàm của nút tìm kiếm
void DS_SV::onBtnTimKiemClicked()
{
    QString malop = ui->editMALOP->text().trimmed().toUpper();
    if (malop.isEmpty()) return;
    int idx = TimLop(malop.toStdString().c_str());
    if(idx == -1) {//nếu idx=-1 là không có lớp
        QMessageBox::warning(this, "Thông báo", "Không tìm thấy lớp!");
        return;
    }

    //nếu tìm thấy thì hiện các thông tin dưới đây của lớp đó
    // hiển thị thông tin lớp
    ui->lblMALOP->setText(QString("Mã lớp: %1").arg(dslop[idx].MALOP));
    ui->lblTENLOP->setText(QString("Tên lớp: %1").arg(dslop[idx].TENLOP));
    ui->lblNAMHOC->setText(QString("Năm học: %1").arg(dslop[idx].NAMHOC));

    // load sinh viên của lớp
    LoadDSSV("sinhvien.txt", dslop[idx].MALOP);

    ui->editMALOP->clear();
}

//dùng để clear các node trong dslk
void DS_SV::ClearDSSV()
{
    PTRSV p = FirstSV;
    while (p) {
        PTRSV tmp = p;
        p = p->next;
        delete tmp;
    }
    FirstSV = nullptr;//sau khi xóa hết cho về rỗng để tránh crash
}

//load dssv lên bảng
void DS_SV::LoadDSSV(const char* filename, const char* malop)
{
    ClearDSSV();// gọi hàm cleardssv ở trên

    strncpy(CurrentMALOP, malop, 15);
    CurrentMALOP[15] = '\0';

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QString qMalop = QString::fromUtf8(malop); // chuyển 1 lần

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if (parts.size() < 7) continue;

        // SO SÁNH ĐÚNG KIỂU
        if (parts[0].trimmed() != qMalop) continue;

        // TRÁNH TRÙNG MÃ SV
        QByteArray masvUtf8 = parts[1].toUtf8();
        if (TimSV(masvUtf8.constData())) continue;//gọi hàm timSV ở trên/ nếu đã có sv đó thì continue để tránh trùng

        PTRSV node = new nodeSV;

        strncpy(node->sv.MASV, masvUtf8.constData(), 15);
        node->sv.MASV[15] = '\0';

        strncpy(node->sv.HO, parts[2].toUtf8().constData(), 50);
        node->sv.HO[50] = '\0';

        strncpy(node->sv.TEN, parts[3].toUtf8().constData(), 10);
        node->sv.TEN[10] = '\0';

        strncpy(node->sv.PHAI, parts[4].toUtf8().constData(), 9);
        node->sv.PHAI[9] = '\0';

        strncpy(node->sv.SODT, parts[5].toUtf8().constData(), 10);
        node->sv.SODT[10] = '\0';

        strncpy(node->sv.NAMNHAPHOC, parts[6].toUtf8().constData(), 9);
        node->sv.NAMNHAPHOC[9] = '\0';

        node->next = FirstSV;
        FirstSV = node;
    }

    file.close();
    RefreshTable();
}

//hàm ktra sv có chưa
bool TonTaiSVToanHeThong(const char* filename, const char* masv)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;//không mở được file trả về false

    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if(parts.size() >= 2)
        {
            if(parts[1].trimmed() == masv)
            {
                file.close();
                return true;//nếu tìm thấy trả về true, sinh viên đó đã tồn tại rồi
            }
        }
    }
    file.close();
    return false;
}


// Lưu sinh viên vào file
void DS_SV::SaveDSSV(const char* filename, const char* malop)
{
    QStringList allLines;
    QFile fileIn(filename);
    if(fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fileIn);
        while(!in.atEnd()) {
            QString line = in.readLine();
            //kiểm tra xem dòng này có thuộc về lớp đang xử lý không
            if(!line.startsWith(QString(malop) + "|")) allLines.append(line);
        }
        fileIn.close();
    }
    //sắp xếp, đóng gói lại dữ liệu trên ram
    PTRSV p = FirstSV;
    while(p) {
        QString line = QString("%1|%2|%3|%4|%5|%6|%7")
        .arg(malop).arg(p->sv.MASV).arg(p->sv.HO).arg(p->sv.TEN)
            .arg(p->sv.PHAI).arg(p->sv.SODT).arg(p->sv.NAMNHAPHOC);
        allLines.append(line);
        p = p->next;
    }
    //ghi đè vào file
    QFile fileOut(filename);
    if(fileOut.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&fileOut);
        for(const QString& l : allLines) out << l << "\n";
        fileOut.close();
    }
}


// Cập nhật TableWidget,xóa dữ liệu trên bảng và đổ dữ liệu mới vào
void DS_SV::RefreshTable()
{
    ui->tableSV->clearContents();

    // Đếm số sinh viên
    int count = 0;
    for(PTRSV p = FirstSV; p != nullptr; p = p->next)
        count++;

    ui->tableSV->setRowCount(count);

    int row = 0;
    for(PTRSV p = FirstSV; p != nullptr; p = p->next)
    {
        ui->tableSV->setItem(row, 0, new QTableWidgetItem(p->sv.MASV));
        ui->tableSV->setItem(row, 1, new QTableWidgetItem(p->sv.HO));
        ui->tableSV->setItem(row, 2, new QTableWidgetItem(p->sv.TEN));
        ui->tableSV->setItem(row, 3, new QTableWidgetItem(p->sv.PHAI));
        ui->tableSV->setItem(row, 4, new QTableWidgetItem(p->sv.SODT));
        ui->tableSV->setItem(row, 5, new QTableWidgetItem(p->sv.NAMNHAPHOC));
        row++;
    }
}

//clear thông tin trên các ô sau khi thêm sinh viên
void DS_SV::ClearInput()
{
    ui->editMASV->clear();
    ui->editHO->clear();
    ui->editTEN->clear();
    ui->editPHAI->clear();
    ui->editSODT->clear();
    ui->editNAMNHAP->clear();
}


//chuẩn hóa tên sinh viên
QString ChuanHoaTen(const QString& s)
{
    QString str = s.trimmed().toLower();

    QStringList words = str.split(QRegularExpression("\\s+"),
                                  Qt::SkipEmptyParts);//cắt chuỗi thành các từ

    for (int i = 0; i < words.size(); i++) {
        words[i][0] = words[i][0].toUpper();
    }

    return words.join(" ");//nối lại các từ với nhau bằng dấu cách
}


void DS_SV::on_btnThem_clicked()
{
    if (strlen(CurrentMALOP) == 0) {
        QMessageBox::warning(this, "Lỗi", "Chưa chọn lớp!");
        return;
    }

    QString masv = ui->editMASV->text().trimmed().toUpper();
    QString ho  = ChuanHoaTen(ui->editHO->text());
    QString ten = ChuanHoaTen(ui->editTEN->text());
    QString phai = ui->editPHAI->text().trimmed();
    QString sdt  = ui->editSODT->text().trimmed();
    QString nam  = ui->editNAMNHAP->text().trimmed();

    // ===== NGUYÊN TẮC 1 =====
    // Không nhập gì cả hoặc thiếu MASV
    if (masv.isEmpty()) {
        QMessageBox::warning(this, "Lỗi",
                             "Vui lòng nhập đầy đủ thông tin sinh viên!");
        return;
    }

    // ===== NGUYÊN TẮC 2 =====
    // Có MASV → ƯU TIÊN CHECK TRÙNG
    if (TonTaiSVToanHeThong("sinhvien.txt", masv.toStdString().c_str())
        || TimSV(masv.toStdString().c_str()))
    {
        QMessageBox::warning(this, "Lỗi",
                             "Mã sinh viên đã tồn tại!");
        return;
    }

    // ===== NGUYÊN TẮC 3 =====
    // MASV KHÔNG TRÙNG → MỚI CHECK THIẾU
    if (ho.isEmpty() || ten.isEmpty() || phai.isEmpty()
        || sdt.isEmpty() || nam.isEmpty())
    {
        QMessageBox::warning(this, "Lỗi",
                             "Vui lòng nhập đầy đủ thông tin sinh viên!");
        return;
    }

    // ===== KIỂM TRA PHÁI =====
    phai = phai.toLower();
    if (phai != "nam" && phai != "nữ" && phai != "nu") {
        QMessageBox::warning(this, "Lỗi",
                             "Phái chỉ được là Nam hoặc Nữ");
        return;
    }
    phai = (phai == "nam") ? "Nam" : "Nữ";

    // ===== THÊM SINH VIÊN =====
    PTRSV node = new nodeSV;
    strncpy(node->sv.MASV, masv.toStdString().c_str(), 15); node->sv.MASV[15] = '\0';
    strncpy(node->sv.HO, ho.toStdString().c_str(), 50); node->sv.HO[50] = '\0';
    strncpy(node->sv.TEN, ten.toStdString().c_str(), 10); node->sv.TEN[10] = '\0';
    strncpy(node->sv.PHAI, phai.toStdString().c_str(), 9); node->sv.PHAI[9] = '\0';
    strncpy(node->sv.SODT, sdt.toStdString().c_str(), 10); node->sv.SODT[10] = '\0';
    strncpy(node->sv.NAMNHAPHOC, nam.toStdString().c_str(), 9); node->sv.NAMNHAPHOC[9] = '\0';

    node->next = FirstSV;
    FirstSV = node;

    SaveDSSV("sinhvien.txt", CurrentMALOP);
    RefreshTable();
    ClearInput();
}

// Xóa sinh viên
void DS_SV::on_btnXoa_clicked() {
    int row = ui->tableSV->currentRow();
    if(row < 0) return;
    PTRSV p = FirstSV, prev = nullptr;
    int k=0;
    while(p && k < row) {
        prev = p;
        p = p->next;
        k++;
    }
    if(p) {
        if(prev) prev->next = p->next;
        else FirstSV = p->next;
        delete p;
        SaveDSSV("sinhvien.txt", CurrentMALOP);
        RefreshTable();
        ClearInput();
        ui->tableSV->clearSelection();//bỏ chọn dòng đó sau khi xóa
    }

}
// Sửa sinh viên
void DS_SV::on_btnSua_clicked() {
    int row = ui->tableSV->currentRow();
    if(row < 0) return;

    PTRSV p = FirstSV;
    int k = 0;
    while(p && k < row) { p = p->next; k++; }

    if(p) {
        QString ho  = ChuanHoaTen(ui->editHO->text());
        QString ten = ChuanHoaTen(ui->editTEN->text());
        strncpy(p->sv.HO, ho.toStdString().c_str(), 50);
        p->sv.HO[50] = '\0';
        strncpy(p->sv.TEN, ten.toStdString().c_str(), 10);
        p->sv.TEN[10] = '\0';


        QString phai = ui->editPHAI->text().trimmed();
        if(phai != "Nam" && phai != "Nữ" && phai != "Nu") {
            QMessageBox::warning(this, "Lỗi", "Phái chỉ được là Nam hoặc Nữ");
            return;
        }
        strncpy(p->sv.PHAI, phai.toStdString().c_str(), 9); p->sv.PHAI[9] = '\0';

        strncpy(p->sv.SODT, ui->editSODT->text().toStdString().c_str(), 10); p->sv.SODT[10] = '\0';
        strncpy(p->sv.NAMNHAPHOC, ui->editNAMNHAP->text().toStdString().c_str(), 9); p->sv.NAMNHAPHOC[9] = '\0';

        SaveDSSV("sinhvien.txt", CurrentMALOP);
        RefreshTable();
        ClearInput();//gọi hàm clear ở trên
    }
}

// Khi click vào table của dòng nào thì đổ thông tin của dòng đó lên các ô
void DS_SV::on_tableSV_cellClicked(int row, int column)
{
    PTRSV p = FirstSV;
    int r = 0;
    while(p && r < row) { p = p->next; r++; }
    if(!p) return;

    ui->editMASV->setText(p->sv.MASV);
    ui->editHO->setText(p->sv.HO);
    ui->editTEN->setText(p->sv.TEN);
    ui->editPHAI->setText(p->sv.PHAI);
    ui->editSODT->setText(p->sv.SODT);
    ui->editNAMNHAP->setText(p->sv.NAMNHAPHOC);
}

void DS_SV::onBtnXemLop_clicked() {
    if(nLop == 0) {
        QMessageBox::information(this, "Thông báo", "Hiện chưa có lớp nào trong hệ thống!");
        return;
    }

    // 1. Tạo hộp thoại Dialog
    QDialog* dlg = new QDialog(this);
    dlg->setWindowTitle("Danh sách lớp học");
    dlg->resize(600, 400); // Kích thước cửa sổ xem

    QVBoxLayout* layout = new QVBoxLayout(dlg);

    // 2. Tạo bảng hiển thị
    QTableWidget* table = new QTableWidget(dlg);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Mã lớp", "Tên lớp", "Năm học"});
    table->setRowCount(nLop);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers); // Không cho sửa
    table->setSelectionBehavior(QAbstractItemView::SelectRows); // Chọn cả hàng

    // 3. Đổ dữ liệu từ mảng dslop vào bảng
    for(int i = 0; i < nLop; i++) {
        table->setItem(i, 0, new QTableWidgetItem(dslop[i].MALOP));
        table->setItem(i, 1, new QTableWidgetItem(dslop[i].TENLOP));
        table->setItem(i, 2, new QTableWidgetItem(dslop[i].NAMHOC));
    }

    // 4. Căn chỉnh bảng cho đẹp
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Cột Mã vừa khít chữ
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);          // Cột Tên giãn hết phần còn lại
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Cột Năm vừa khít

    layout->addWidget(table);
    dlg->setLayout(layout);

    // 5. Hiển thị Dialog
    dlg->exec();

    // Dọn dẹp bộ nhớ sau khi đóng
    delete dlg;
}

void DS_SV::on_btnBack_clicked()
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}
