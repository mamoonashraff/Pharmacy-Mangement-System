#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadData();

    ui->stackedWidget->setCurrentIndex(0);

    ui->adminTable->setColumnCount(5);
    ui->adminTable->setHorizontalHeaderLabels({"ID", "Name", "Company", "Price", "Quantity"});
    ui->adminTable->horizontalHeader()->setStretchLastSection(true);
    ui->adminTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->adminTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->staffTable->setColumnCount(5);
    ui->staffTable->setHorizontalHeaderLabels({"ID", "Name", "Company", "Price", "Stock"});
    ui->staffTable->horizontalHeader()->setStretchLastSection(true);
    ui->staffTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->staffTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    saveData();
    delete ui;
}

void MainWindow::loadData()
{
    QFile file("pharmacy_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|");

        if (parts.size() == 5) {
            Medicine med;
            med.id = parts[0].toInt();
            med.name = parts[1];
            med.company = parts[2];
            med.price = parts[3].toInt();
            med.quantity = parts[4].toInt();
            medicines.append(med);
        }
    }

    file.close();
}

void MainWindow::saveData()
{
    QFile file("pharmacy_data.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    for (const Medicine &med : medicines) {
        out << med.id << "|"
            << med.name << "|"
            << med.company << "|"
            << med.price << "|"
            << med.quantity << "\n";
    }

    file.close();
}

bool MainWindow::checkPassword(const QString &userType)
{
    QString password = ui->passwordInput->text();

    if (userType == "Admin" && password == "admin123") {
        ui->passwordInput->clear();
        return true;
    }
    else if (userType == "Staff" && password == "staff123") {
        ui->passwordInput->clear();
        return true;
    }
    else {
        QMessageBox::warning(this, "Error", "Wrong Password!");
        ui->passwordInput->clear();
        return false;
    }
}

void MainWindow::on_adminLoginBtn_clicked()
{
    if (checkPassword("Admin")) {
        ui->stackedWidget->setCurrentIndex(1);
        refreshAdminTable();
    }
}

void MainWindow::on_staffLoginBtn_clicked()
{
    if (checkPassword("Staff")) {
        ui->stackedWidget->setCurrentIndex(2);
        refreshStaffTable();
        ui->billDisplay->clear();
    }
}

void MainWindow::refreshAdminTable()
{
    ui->adminTable->setRowCount(0);

    for (int i = 0; i < medicines.size(); i++) {
        ui->adminTable->insertRow(i);
        ui->adminTable->setItem(i, 0, new QTableWidgetItem(QString::number(medicines[i].id)));
        ui->adminTable->setItem(i, 1, new QTableWidgetItem(medicines[i].name));
        ui->adminTable->setItem(i, 2, new QTableWidgetItem(medicines[i].company));
        ui->adminTable->setItem(i, 3, new QTableWidgetItem(QString::number(medicines[i].price)));
        ui->adminTable->setItem(i, 4, new QTableWidgetItem(QString::number(medicines[i].quantity)));
    }
}

void MainWindow::clearAdminInputs()
{
    ui->idInput->clear();
    ui->nameInput->clear();
    ui->companyInput->clear();
    ui->priceInput->clear();
    ui->qtyInput->clear();
}

int MainWindow::findMedicineById(int id)
{
    for (int i = 0; i < medicines.size(); i++) {
        if (medicines[i].id == id) {
            return i;
        }
    }
    return -1;
}

void MainWindow::on_addMedicineBtn_clicked()
{
    bool ok;
    int id = ui->idInput->text().toInt(&ok);
    if (!ok || id <= 0) {
        QMessageBox::warning(this, "Error", "Enter valid ID!");
        return;
    }

    // Check duplicate
    if (findMedicineById(id) != -1) {
        QMessageBox::warning(this, "Error", "ID already exists!");
        return;
    }

    QString name = ui->nameInput->text().trimmed();
    QString company = ui->companyInput->text().trimmed();

    int price = ui->priceInput->text().toInt(&ok);
    if (!ok || price <= 0) {
        QMessageBox::warning(this, "Error", "Enter valid price!");
        return;
    }

    int qty = ui->qtyInput->text().toInt(&ok);
    if (!ok || qty < 0) {
        QMessageBox::warning(this, "Error", "Enter valid quantity!");
        return;
    }

    if (name.isEmpty() || company.isEmpty()) {
        QMessageBox::warning(this, "Error", "Name and Company required!");
        return;
    }

    // Add medicine
    Medicine med;
    med.id = id;
    med.name = name;
    med.company = company;
    med.price = price;
    med.quantity = qty;

    medicines.append(med);
    saveData();
    refreshAdminTable();
    clearAdminInputs();

    QMessageBox::information(this, "Success", "Medicine added successfully!");
}

void MainWindow::on_refreshAdminBtn_clicked()
{
    refreshAdminTable();
}

void MainWindow::on_updateBtn_clicked()
{
    int row = ui->adminTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Select a medicine first!");
        return;
    }

    bool ok;
    int newPrice = QInputDialog::getInt(this, "Update Price",
                                        "Enter new price:",
                                        medicines[row].price, 1, 999999, 1, &ok);
    if (ok) {
        medicines[row].price = newPrice;
    }

    int newQty = QInputDialog::getInt(this, "Update Quantity",
                                      "Enter new quantity:",
                                      medicines[row].quantity, 0, 999999, 1, &ok);
    if (ok) {
        medicines[row].quantity = newQty;
    }

    saveData();
    refreshAdminTable();
    QMessageBox::information(this, "Success", "Medicine updated!");
}

void MainWindow::on_deleteBtn_clicked()
{
    int row = ui->adminTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Select a medicine first!");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete",
                                  "Are you sure you want to delete this medicine?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        medicines.removeAt(row);
        saveData();
        refreshAdminTable();
        QMessageBox::information(this, "Success", "Medicine deleted!");
    }
}

void MainWindow::on_searchAdminBtn_clicked()
{
    QString searchText = ui->searchAdminInput->text().trimmed().toLower();

    if (searchText.isEmpty()) {
        refreshAdminTable();
        return;
    }

    ui->adminTable->setRowCount(0);
    int rowCount = 0;

    for (const Medicine &med : medicines) {
        if (med.name.toLower().contains(searchText) ||
            med.company.toLower().contains(searchText)) {

            ui->adminTable->insertRow(rowCount);
            ui->adminTable->setItem(rowCount, 0, new QTableWidgetItem(QString::number(med.id)));
            ui->adminTable->setItem(rowCount, 1, new QTableWidgetItem(med.name));
            ui->adminTable->setItem(rowCount, 2, new QTableWidgetItem(med.company));
            ui->adminTable->setItem(rowCount, 3, new QTableWidgetItem(QString::number(med.price)));
            ui->adminTable->setItem(rowCount, 4, new QTableWidgetItem(QString::number(med.quantity)));
            rowCount++;
        }
    }
}

void MainWindow::on_adminLogoutBtn_clicked()
{
    clearAdminInputs();
    ui->stackedWidget->setCurrentIndex(0); // Back to login
}

// ==================== STAFF PAGE ====================

void MainWindow::refreshStaffTable()
{
    ui->staffTable->setRowCount(0);

    for (int i = 0; i < medicines.size(); i++) {
        ui->staffTable->insertRow(i);
        ui->staffTable->setItem(i, 0, new QTableWidgetItem(QString::number(medicines[i].id)));
        ui->staffTable->setItem(i, 1, new QTableWidgetItem(medicines[i].name));
        ui->staffTable->setItem(i, 2, new QTableWidgetItem(medicines[i].company));
        ui->staffTable->setItem(i, 3, new QTableWidgetItem(QString::number(medicines[i].price) + " PKR"));
        ui->staffTable->setItem(i, 4, new QTableWidgetItem(QString::number(medicines[i].quantity)));
    }
}

int MainWindow::findMedicineByName(const QString &name)
{
    QString searchName = name.toLower();
    for (int i = 0; i < medicines.size(); i++) {
        if (medicines[i].name.toLower() == searchName) {
            return i;
        }
    }
    return -1;
}

void MainWindow::on_sellBtn_clicked()
{
    QString name = ui->sellNameInput->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter medicine name!");
        return;
    }

    int index = findMedicineByName(name);
    if (index == -1) {
        QMessageBox::warning(this, "Error", "Medicine not found!");
        ui->billDisplay->setText("Medicine not found in stock!");
        return;
    }

    bool ok;
    int qty = ui->sellQtyInput->text().toInt(&ok);
    if (!ok || qty <= 0) {
        QMessageBox::warning(this, "Error", "Enter valid quantity!");
        return;
    }

    if (qty > medicines[index].quantity) {
        QMessageBox::warning(this, "Error",
                             QString("Not enough stock! Only %1 available").arg(medicines[index].quantity));
        return;
    }

    // Calculate bill
    int total = qty * medicines[index].price;
    medicines[index].quantity -= qty;

    // Display bill
    QString bill = QString(
                       "========== BILL ==========\n"
                       "Item  : %1\n"
                       "Qty   : %2\n"
                       "Price : %3 PKR\n"
                       "Total : %4 PKR\n"
                       "=========================="
                       ).arg(medicines[index].name)
                       .arg(qty)
                       .arg(medicines[index].price)
                       .arg(total);

    ui->billDisplay->setText(bill);

    // Low stock warning
    if (medicines[index].quantity < 10) {
        QString warning = QString("\n\n⚠ WARNING: Low stock!\nOnly %1 units remaining")
                              .arg(medicines[index].quantity);
        ui->billDisplay->setText(bill + warning);
    }

    saveData();
    refreshStaffTable();

    ui->sellNameInput->clear();
    ui->sellQtyInput->clear();

    QMessageBox::information(this, "Success", "Medicine sold successfully!");
}

void MainWindow::on_refreshStaffBtn_clicked()
{
    refreshStaffTable();
}

void MainWindow::on_searchStaffBtn_clicked()
{
    QString searchText = ui->searchStaffInput->text().trimmed().toLower();

    if (searchText.isEmpty()) {
        refreshStaffTable();
        return;
    }

    ui->staffTable->setRowCount(0);
    int rowCount = 0;

    for (const Medicine &med : medicines) {
        if (med.name.toLower().contains(searchText) ||
            med.company.toLower().contains(searchText)) {

            ui->staffTable->insertRow(rowCount);
            ui->staffTable->setItem(rowCount, 0, new QTableWidgetItem(QString::number(med.id)));
            ui->staffTable->setItem(rowCount, 1, new QTableWidgetItem(med.name));
            ui->staffTable->setItem(rowCount, 2, new QTableWidgetItem(med.company));
            ui->staffTable->setItem(rowCount, 3, new QTableWidgetItem(QString::number(med.price) + " PKR"));
            ui->staffTable->setItem(rowCount, 4, new QTableWidgetItem(QString::number(med.quantity)));
            rowCount++;
        }
    }
}

void MainWindow::on_staffLogoutBtn_clicked()
{
    ui->sellNameInput->clear();
    ui->sellQtyInput->clear();
    ui->billDisplay->clear();
    ui->stackedWidget->setCurrentIndex(0); // Back to login
}
