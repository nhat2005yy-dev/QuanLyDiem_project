#ifndef DS_LOP_H
#define DS_LOP_H

#include <QDialog>

struct nodeSV;
typedef nodeSV* PTRSV;

struct LopSV {
    char MALOP[16];
    char TENLOP[51];
    char NAMHOC[10];
    PTRSV FirstSV;       // danh sách sinh viên của lớp
};

#define MAX_LOP 10000

namespace Ui {
class DS_Lop;
}

class DS_Lop : public QDialog
{
    Q_OBJECT

public:
    explicit DS_Lop(QWidget *parent = nullptr);
    ~DS_Lop();

    void LoadDSLop(const char* filename);
    void SaveDSLop(const char* filename);
    void RefreshTable();
    int TimLop(const char* malop);

private slots:
    void on_btnThem_clicked();
    void on_btnSua_clicked();
    void on_btnXoa_clicked();
    void on_tableLop_cellClicked(int row, int column);
    void on_btnBack_clicked();

private:
    Ui::DS_Lop *ui;
    LopSV* dslop[MAX_LOP];  // danh sách lớp sinh viên
    int n = 0;              // số lớp hiện có
};

#endif // DS_LOP_H
