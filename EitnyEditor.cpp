#include "EitnyEditor.h"

EitnyEditor::EitnyEditor(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("EitnyEditor");
    resize(800, 600);

    model = new QStandardItemModel(this);
    tableView = new QTableView(this);
    tableView->setModel(model);

    searchField = new QLineEdit(this);
    replaceField = new QLineEdit(this);
    searchButton = new QPushButton("Search", this);
    replaceButton = new QPushButton("Replace", this);
    
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(searchField);
    searchLayout->addWidget(replaceField);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(replaceButton);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addLayout(searchLayout);
    layout->addWidget(tableView);
    setCentralWidget(centralWidget);

    connect(searchButton, &QPushButton::clicked, this, &EitnyEditor::searchText);
    connect(replaceButton, &QPushButton::clicked, this, &EitnyEditor::replaceText);
    connect(tableView->verticalScrollBar(), &QScrollBar::valueChanged, this, &EitnyEditor::onScroll);

    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "JSON Files (*.json);;CSV Files (*.csv)");
    if (!filePath.isEmpty()) {
        loadFile(filePath);
    }
}

void EitnyEditor::loadFile(const QString &filePath) {
    file.setFileName(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Cannot open file: " + filePath);
        return;
    }

    model->clear();

    if (filePath.endsWith(".json")) {
        loadJsonBatch();
    } else if (filePath.endsWith(".csv")) {
        loadCsvBatch();
    }
}

void EitnyEditor::loadJsonBatch() {
    if (!file.isOpen()) return;

    QByteArray jsonChunk = file.read(batchSize * 500);
    QJsonDocument doc = QJsonDocument::fromJson(jsonChunk);
    if (!doc.isArray()) {
        QMessageBox::critical(this, "JSON Error", "Invalid JSON format");
        return;
    }

    QJsonArray jsonArray = doc.array();
    if (loadedRows == 0 && !jsonArray.isEmpty() && jsonArray[0].isObject()) {
        QStringList headers = jsonArray[0].toObject().keys();
        model->setHorizontalHeaderLabels(headers);
    }

    for (const QJsonValue &value : jsonArray) {
        if (!value.isObject()) continue;
        QJsonObject obj = value.toObject();
        QList<QStandardItem *> rowItems;

        for (int i = 0; i < model->columnCount(); i++) {
            QString key = model->horizontalHeaderItem(i)->text();
            rowItems.append(new QStandardItem(obj.value(key).toString()));
        }
        model->appendRow(rowItems);
        loadedRows++;
        if (loadedRows >= batchSize) break;
    }
}

void EitnyEditor::loadCsvBatch() {
    QTextStream in(&file);
    int rowCount = 0;
    while (!in.atEnd() && rowCount < batchSize) {
        QString line = in.readLine();
        QList<QStandardItem *> rowItems;
        for (const QString &cell : line.split(",")) {
            rowItems.append(new QStandardItem(cell));
        }
        model->appendRow(rowItems);
        rowCount++;
        loadedRows++;
    }
}

void EitnyEditor::onScroll(int value) {
    if (value == tableView->verticalScrollBar()->maximum()) {
        if (file.fileName().endsWith(".json"))
            loadJsonBatch();
        else if (file.fileName().endsWith(".csv"))
            loadCsvBatch();
    }
}

void EitnyEditor::searchText() {
    QString searchText = searchField->text();
    if (searchText.isEmpty()) return;

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QStandardItem *item = model->item(row, col);
            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                tableView->scrollTo(item->index());
                tableView->setCurrentIndex(item->index());
                return;
            }
        }
    }
}

void EitnyEditor::replaceText() {
    QString searchText = searchField->text();
    QString replaceText = replaceField->text();
    if (searchText.isEmpty() || replaceText.isEmpty()) return;

    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QStandardItem *item = model->item(row, col);
            if (item && item->text().contains(searchText, Qt::CaseInsensitive)) {
                item->setText(item->text().replace(searchText, replaceText, Qt::CaseInsensitive));
            }
        }
    }
}


