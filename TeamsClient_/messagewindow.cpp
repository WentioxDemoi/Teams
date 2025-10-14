#include "messagewindow.h"
#include "mainwindow.h"
#include <iostream>


MessageWindow::MessageWindow(QStackedWidget *list, QWidget *parent, MainWindow *main)
    : QWidget(parent)
{
    List_ = list;
    main_ = main;
    previous = parent;
    Database_ = new DatabaseManager();
    Database_->open(":memory:");
    persistent_ = new DatabaseManager();
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(dataPath))
        dir.mkpath(dataPath);

    QString dbFile = dataPath + "/data.db";
    persistent_->open(dbFile.toStdString());
    this->Init();
}

void MessageWindow::Init()
{
    next = new Message2Window(List_, this);
    List_->addWidget(next);

    // Layout principal vertical
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ------------------ Layout horizontal pour les 2 colonnes ------------------
    QHBoxLayout *columnsLayout = new QHBoxLayout();

    // ------------------ Colonne 1 : Cache ------------------
    QVBoxLayout *cacheLayout = new QVBoxLayout();

    // Input field + bouton pour cache
    QHBoxLayout *cacheInputLayout = new QHBoxLayout();
    QLineEdit *cacheInput = new QLineEdit();
    QPushButton *cacheSendButton = new QPushButton("Send (Cache)");
    cacheInputLayout->addWidget(cacheInput);
    cacheInputLayout->addWidget(cacheSendButton);

    // Scroll view pour messages cache
    QTextEdit *cacheMessages = new QTextEdit();
    cacheMessages->setReadOnly(true);

    cacheLayout->addLayout(cacheInputLayout);
    cacheLayout->addWidget(cacheMessages);

    // ------------------ Colonne 2 : Persistante ------------------
    QVBoxLayout *persistentLayout = new QVBoxLayout();

    // Input field + bouton pour DB persistante
    QHBoxLayout *persistentInputLayout = new QHBoxLayout();
    QLineEdit *persistentInput = new QLineEdit();
    QPushButton *persistentSendButton = new QPushButton("Send (Persistent)");
    persistentInputLayout->addWidget(persistentInput);
    persistentInputLayout->addWidget(persistentSendButton);

    // Scroll view pour messages persistants
    QTextEdit *persistentMessages = new QTextEdit();
    persistentMessages->setReadOnly(true);

    persistentLayout->addLayout(persistentInputLayout);
    persistentLayout->addWidget(persistentMessages);

    // Ajouter les 2 colonnes dans le layout horizontal
    columnsLayout->addLayout(cacheLayout);
    columnsLayout->addLayout(persistentLayout);

    // ------------------ Boutons Next / Previous en bas ------------------
    QHBoxLayout *bottomButtonsLayout = new QHBoxLayout();
    previousButton_ = new QPushButton("Previous");
    nextButton_ = new QPushButton("Next");
    bottomButtonsLayout->addWidget(previousButton_);
    bottomButtonsLayout->addWidget(nextButton_);

    // Ajouter les colonnes et les boutons au layout principal
    mainLayout->addLayout(columnsLayout);
    mainLayout->addLayout(bottomButtonsLayout);

    setLayout(mainLayout);

    connect(nextButton_, &QPushButton::clicked, this, &MessageWindow::nextWindow);
    connect(previousButton_, &QPushButton::clicked, this, &MessageWindow::previousWindow);

    std::vector<std::string> messages = Database_->getAllMessages();
    for (int i = 0; i < messages.size(); i++) {
        cacheMessages->append(QString::fromStdString(messages[i]));
    }
    messages.clear();
    messages = persistent_->getAllMessages();
    for (int i = 0; i < messages.size(); i++) {
        persistentMessages->append(QString::fromStdString(messages[i]));
    }

    connect(cacheSendButton, &QPushButton::clicked, [=]() {
        std::string text = cacheInput->text().toStdString();
        if (text.empty()) return;
        Database_->execute("CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY, text TEXT);");
        Database_->execute("INSERT INTO messages (text) VALUES ('" + text + "');");

        cacheMessages->append(QString::fromStdString(text));
    });

    connect(persistentSendButton, &QPushButton::clicked, [=]() {
        persistent_->execute("CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY, text TEXT);");

        std::string text = persistentInput->text().toStdString();
        if (text.empty()) return;

        persistent_->execute("INSERT INTO messages (text) VALUES ('" + text + "');");
        persistentMessages->append(QString::fromStdString(text));

    });
}

void MessageWindow::nextWindow()
{
    List_->setCurrentWidget(next);
    persistent_->close(); // <-- important
    Database_->close();
    delete Database_;
    delete persistent_;

    std::cout << "Databases ready.\n";
}
void MessageWindow::previousWindow()
{
    List_->setCurrentWidget(previous);
    main_->PlayVideo();
}

MessageWindow::~MessageWindow()
{
}