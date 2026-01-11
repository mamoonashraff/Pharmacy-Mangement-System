#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Medicine Structure
struct Medicine {
    int id;
    QString name;
    QString company;
    int price;
    int quantity;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Login Page Slots
    void on_adminLoginBtn_clicked();
    void on_staffLoginBtn_clicked();

    // Admin Page Slots
    void on_addMedicineBtn_clicked();
    void on_refreshAdminBtn_clicked();
    void on_updateBtn_clicked();
    void on_deleteBtn_clicked();
    void on_adminLogoutBtn_clicked();
    void on_searchAdminBtn_clicked();

    // Staff Page Slots
    void on_sellBtn_clicked();
    void on_refreshStaffBtn_clicked();
    void on_staffLogoutBtn_clicked();
    void on_searchStaffBtn_clicked();

private:
    Ui::MainWindow *ui;

    // Data Storage
    QVector<Medicine> medicines;

    // Helper Functions
    void loadData();
    void saveData();
    void refreshAdminTable();
    void refreshStaffTable();
    bool checkPassword(const QString &userType);
    int findMedicineById(int id);
    int findMedicineByName(const QString &name);
    void clearAdminInputs();
};

#endif // MAINWINDOW_H
