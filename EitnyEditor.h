#ifndef EITNYEDITOR_H
#define EITNYEDITOR_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScrollBar>

class EitnyEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit EitnyEditor(QWidget *parent = nullptr);

private:
    QTableView *tableView;
    QStandardItemModel *model;
    QFile file;
    int loadedRows = 0;
    const int batchSize = 20000;
    QLineEdit *searchField;
    QLineEdit *replaceField;
    QPushButton *searchButton;
    QPushButton *replaceButton;

    void loadFile(const QString &filePath);
    void loadJsonBatch();
    void loadCsvBatch();
    void onScroll(int value);
    void searchText();
    void replaceText();
};

#endif // EITNYEDITOR_H
