#ifndef DANHSACHMONHOC_H
#define DANHSACHMONHOC_H

#include <QDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QTextStream>
#include <cstring>

// Cấu trúc Môn học
struct MonHoc {
    char MAMH[11];
    char TENMH[51];
    int STCLT;
    int STCTH;
};

// Node Cây AVL
struct nodeMH {
    MonHoc mh;
    nodeMH* left;
    nodeMH* right;
    int height;
};

typedef nodeMH* TreeMH;

namespace Ui { class DanhSachMonHoc; }

class DanhSachMonHoc : public QDialog
{
    Q_OBJECT

public:
    explicit DanhSachMonHoc(QWidget *parent = nullptr);
    ~DanhSachMonHoc();

    // === CÁC HÀM AVL ===
    int height(TreeMH N);
    int max(int a, int b);
    TreeMH newNode(MonHoc mh);
    TreeMH rightRotate(TreeMH y);
    TreeMH leftRotate(TreeMH x);
    int getBalance(TreeMH N);
    TreeMH insertNode(TreeMH node, MonHoc mh);

    // Thêm const char* để tránh lỗi ép kiểu
    TreeMH deleteNode(TreeMH root, const char* mamh);
    TreeMH searchNode(TreeMH node, const char* mamh);

    TreeMH minValueNode(TreeMH node);

    // === CÁC HÀM HỖ TRỢ ===
    void FreeTree(TreeMH &node);
    int countNodes(TreeMH node);
    void treeToArray(TreeMH node, MonHoc* arr, int &index);
    void loadFromFile();
    void saveToFileRecursive(TreeMH node, QTextStream &out);
    void SaveData();
    QString m_maMhDangChon;

    // === HIỂN THỊ ===
    void HienThiDanhSach();

private slots:
    void on_btnThemMH_clicked();
    void on_btnSuaMH_clicked();
    void on_btnXoaMH_clicked();
    void on_btnQuayLai_clicked();
    void on_tblMonHoc_cellClicked(int row, int column);

private:
    Ui::DanhSachMonHoc *ui;
    TreeMH root = nullptr;
};

#endif // DANHSACHMONHOC_H
