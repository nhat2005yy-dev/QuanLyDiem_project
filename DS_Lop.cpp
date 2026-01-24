#include "DS_Lop.h"
#include "ui_DS_Lop.h"

#include <QRegularExpression>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <cstring>

using namespace std;

DS_Lop::DS_Lop(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DS_Lop)
{
    ui->setupUi(this);
    ui->tableLop->setEditTriggers(QAbstractItemView::NoEditTriggers);
    n = 0;

    // CHẶN DẤU CÁCH + ÉP IN HOA MÃ LỚP
    connect(ui->editMALOP, &QLineEdit::textChanged,
            this, [=](QString text){
                QString fixed = text.toUpper();
                fixed.remove(' ');
                fixed.remove(QRegularExpression("[^A-Z0-9\\-]"));
                if (fixed != ui->editMALOP->text())
                    ui->editMALOP->setText(fixed);
            });

    LoadDSLop("dslopsv.txt");
    RefreshTable();
}

DS_Lop::~DS_Lop()
{
    SaveDSLop("dslopsv.txt");
    for(int i=0;i<n;i++) delete dslop[i];
    delete ui;
}

int DS_Lop::TimLop(const char* malop)
{
    for(int i=0;i<n;i++)
        if(strcmp(dslop[i]->MALOP, malop)==0) return i;
    return -1;
}

void DS_Lop::LoadDSLop(const char* filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);

    while(!in.atEnd() && n<MAX_LOP)
    {
        QString line = in.readLine();
        QStringList parts = line.split("|");
        if(parts.size()<3) continue;

        LopSV* lop = new LopSV;
        strncpy(lop->MALOP, parts[0].toLatin1().constData(), 15);
        lop->MALOP[15] = '\0';

        strncpy(lop->TENLOP, parts[1].toUtf8().constData(), 50);
        lop->TENLOP[50] = '\0';

        strncpy(lop->NAMHOC, parts[2].toLatin1().constData(), 9);
        lop->NAMHOC[9] = '\0';
        lop->FirstSV = nullptr;
        dslop[n++] = lop;
    }
    file.close();
}

void DS_Lop::SaveDSLop(const char* filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    for(int i=0;i<n;i++)
        out << dslop[i]->MALOP << "|" << dslop[i]->TENLOP << "|" << dslop[i]->NAMHOC << "\n";
    file.close();
}

void DS_Lop::RefreshTable()
{
    ui->tableLop->clearContents();
    ui->tableLop->setRowCount(n);

    for(int i=0;i<n;i++)
    {
        ui->tableLop->setItem(i,0,new QTableWidgetItem(dslop[i]->MALOP));
        ui->tableLop->setItem(i,1,new QTableWidgetItem(dslop[i]->TENLOP));
        ui->tableLop->setItem(i,2,new QTableWidgetItem(dslop[i]->NAMHOC));
    }
}

void DS_Lop::on_btnThem_clicked()
{
    if(n>=MAX_LOP) { QMessageBox::warning(this,"Thông báo","Đạt tối đa số lớp"); return; }

    QString qmalop = ui->editMALOP->text().trimmed().toUpper(); // ép chữ in hoa
    QString qtenlop = ui->editTENLOP->text().trimmed();
    QString qnamhoc = ui->editNAMHOC->text().trimmed();

    if(qmalop.isEmpty() || qtenlop.isEmpty() || qnamhoc.isEmpty()) {
        QMessageBox::warning(this, "Thông báo", "Không được bỏ trống dữ liệu!");
        return;
    }

    // kiểm tra năm học chỉ chứa số
    if(!qnamhoc.contains(QRegularExpression("^[0-9]+$"))) {
        QMessageBox::warning(this,"Thông báo","Năm học phải là số!");
        return;
    }

    if(TimLop(qmalop.toStdString().c_str())>=0)
    {
        QMessageBox::warning(this,"Thông báo","Mã lớp đã tồn tại");
        return;
    }

    for(int i = 0; i < n; i++) {
        if(strcmp(dslop[i]->TENLOP, qtenlop.toStdString().c_str()) == 0 &&
            strcmp(dslop[i]->NAMHOC, qnamhoc.toStdString().c_str()) == 0)
        {
            QMessageBox::warning(this, "Thông báo", "Tên lớp và năm học đã tồn tại");
            return;
        }
    }

    LopSV* lop = new LopSV;
    strncpy(lop->MALOP,qmalop.toStdString().c_str(),15);
    strncpy(lop->TENLOP,qtenlop.toStdString().c_str(),50);
    strncpy(lop->NAMHOC,qnamhoc.toStdString().c_str(),9);
    lop->FirstSV = nullptr;

    dslop[n++] = lop;
    RefreshTable();
    SaveDSLop("dslopsv.txt");
}

void DS_Lop::on_btnXoa_clicked()
{
    int row = ui->tableLop->currentRow();
    if(row<0 || row>=n) return;

    delete dslop[row];
    for(int i=row;i<n-1;i++) dslop[i]=dslop[i+1];
    n--;
    RefreshTable();
    SaveDSLop("dslopsv.txt");
}
void DS_Lop::on_btnSua_clicked()
{
    int row = ui->tableLop->currentRow();
    if(row < 0) {
        QMessageBox::warning(this, "Lỗi", "Hãy chọn 1 lớp để sửa.");
        return;
    }

    QString qmalop = ui->editMALOP->text().trimmed().toUpper();
    QString qtenlop = ui->editTENLOP->text().trimmed();
    QString qnamhoc = ui->editNAMHOC->text().trimmed();

    if(qmalop.isEmpty() || qtenlop.isEmpty() || qnamhoc.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Không được bỏ trống dữ liệu!");
        return;
    }

    if(!qnamhoc.contains(QRegularExpression("^[0-9]+$"))) {
        QMessageBox::warning(this, "Lỗi", "Năm học phải là số!");
        return;
    }

    // Kiểm tra trùng mã lớp
    for(int i = 0; i < n; i++) {
        if(i == row) continue;
        if(strcmp(dslop[i]->MALOP, qmalop.toStdString().c_str()) == 0) {
            QMessageBox::warning(this, "Lỗi", "Mã lớp đã tồn tại!");
            return;
        }
    }

    // Kiểm tra trùng tên lớp + năm học
    for(int i = 0; i < n; i++) {
        if(i == row) continue;
        if(strcmp(dslop[i]->TENLOP, qtenlop.toStdString().c_str()) == 0 &&
            strcmp(dslop[i]->NAMHOC, qnamhoc.toStdString().c_str()) == 0) {
            QMessageBox::warning(this, "Lỗi", "Tên lớp + Năm học đã tồn tại!");
            return;
        }
    }

    // Cập nhật dữ liệu
    strncpy(dslop[row]->MALOP, qmalop.toStdString().c_str(), 15);
    strncpy(dslop[row]->TENLOP, qtenlop.toStdString().c_str(), 50);
    strncpy(dslop[row]->NAMHOC, qnamhoc.toStdString().c_str(), 9);

    RefreshTable();
    SaveDSLop("dslopsv.txt");

    QMessageBox::information(this, "Thành công", "Sửa thông tin lớp thành công!");
}

void DS_Lop::on_tableLop_cellClicked(int row, int column)
{
    if(row < 0 || row >= n) return; // kiểm tra hợp lệ

    // Lấy dữ liệu từ dòng được click và đưa vào các lineedit
    ui->editMALOP->setText(dslop[row]->MALOP);
    ui->editTENLOP->setText(dslop[row]->TENLOP);
    ui->editNAMHOC->setText(dslop[row]->NAMHOC);
}

void DS_Lop::on_btnBack_clicked()
{
    this->close();
    if (parentWidget()) {
        parentWidget()->show();
    }
}

