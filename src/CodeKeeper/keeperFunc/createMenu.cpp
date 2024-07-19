#include "mainwindow.h"

void MainWindow::createNotesMenu(QMenu* menu, QString font_size)
{
    newNote =
            menu->addAction(QPixmap(":/new.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
                            "New Note", this, SLOT(createNote()), Qt::CTRL + Qt::Key_N);
    rmNote = menu->addAction(
            QPixmap(":/delete.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Remove",
            this, SLOT(removeNote()), Qt::CTRL + Qt::Key_Delete);
    newFolder = menu->addAction(
            QPixmap(":/new_folder.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
            "New folder", this, SLOT(createFolder()), Qt::CTRL + Qt::SHIFT + Qt::Key_N);
    renameItemA = menu->addAction(
            QPixmap(":/rename.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Rename",
            this, SLOT(renameItem()), Qt::Key_F2);

    menu->addSeparator();

    expandAllA = menu->addAction(
            QPixmap(":/expand.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
            "Expand on one stage", [this]() { notesList->expandAll(); }, Qt::CTRL + Qt::Key_E);

    menu->addSeparator();

    QMenu* viewMenu = new QMenu("View", menu);

    viewMenu->setIcon(QPixmap(":/view.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    showList = viewMenu->addAction("Show notes list", this, SLOT(hideNotesList()), Qt::CTRL + Qt::SHIFT + Qt::Key_L);
    showList->setCheckable(true);
    showList->setChecked(isVisibleNotesList);
    showRender = viewMenu->addAction("Show md preview", this, SLOT(showPreview()), Qt::CTRL + Qt::SHIFT + Qt::Key_P);
    showRender->setCheckable(true);
    showRender->setChecked(isVisiblePreview);
    viewMenu->addSeparator();
    viewMode = viewMenu->addAction(
            QPixmap(":/view.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Reading mode",
            this, SLOT(toViewMode()), Qt::CTRL + Qt::SHIFT + Qt::Key_V);
    viewMode->setCheckable(true);
    viewMode->setChecked(isViewMode);

    QMenu* editMenu = new QMenu("Edit", menu);
    editMenu->setIcon(QPixmap(":/edit.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setH1A = editMenu->addAction(
            QPixmap(":/h1.png").scaled(font_size.toInt() + 3, font_size.toInt() + 3, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Set H1", this,
            SLOT(setH1()));
    setH2A = editMenu->addAction(
            QPixmap(":/h2.png").scaled(font_size.toInt() + 3, font_size.toInt() + 3, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Set H2", this,
            SLOT(setH2()));
    setH3A = editMenu->addAction(
            QPixmap(":/h3.png").scaled(font_size.toInt() + 3, font_size.toInt() + 3, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Set H3", this,
            SLOT(setH3()));
    editMenu->addSeparator();
    setQuoteA = editMenu->addAction(
            QPixmap(":/quote.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Add quote",
            this, SLOT(setQuote()));
    setListA = editMenu->addAction(
            QPixmap(":/list.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
            "Add list item", this, SLOT(setList()));
    setNumListA = editMenu->addAction(
            QPixmap(":/numList.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
            "Add numbered list", this, SLOT(setNumList()));
    setLinkA = editMenu->addAction(
            QPixmap(":/link.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Add link",
            this, SLOT(setLink()));
    setTaskA = editMenu->addAction(
            QPixmap(":/checkbox.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Add task",
            this, SLOT(setTask()));
    editMenu->addSeparator();
    setBoldA = editMenu->addAction(
            QPixmap(":/bold.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Set bold",
            this, SLOT(setBold()));
    setItalicA = editMenu->addAction(
            QPixmap(":/italic.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Set italic",
            this, SLOT(setItalic()));
    setStrikeA = editMenu->addAction(
            QPixmap(":/strikethrough.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
            "Set strikethrough", this, SLOT(setStrike()));
    setTableA = editMenu->addAction(
            QPixmap(":/table.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Add table",
            this, SLOT(setTable()));

    QMenu* sortMenu = new QMenu("Sort by", menu);
    sortMenu->setIcon(QPixmap(":/sorting.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    nameAction = sortMenu->addAction("Name", this, SLOT(setSortByName()));
    dateAction = sortMenu->addAction("Date", this, SLOT(setSortByTime()));

    QMenu* exportMenu = new QMenu("Export as", menu);
    exportMenu->setIcon(QPixmap(":/export.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    exportToHtml = exportMenu->addAction("HTML", this, SLOT(exportNoteToHtml()));
    exportToPdf = exportMenu->addAction("Pdf", this, SLOT(exportNoteToPdf()));

    menu->addMenu(editMenu);
    menu->addMenu(viewMenu);
    menu->addMenu(sortMenu);
    menu->addMenu(exportMenu);
}

void MainWindow::createProjectMenu(QMenu* menu, QString font_size)
{
    newProject = projectsMenu->addAction(
            QPixmap(":/new.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "New", this,
            SLOT(createProject()), Qt::CTRL + Qt::Key_N);
    rmProject = projectsMenu->addAction(
            QPixmap(":/delete.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Remove",
            this, SLOT(removeProject()), Qt::Key_Delete);
}

void MainWindow::createTaskMenu(QMenu* menu, QString font_size)
{
    addTask = tasksMenu->addAction(
            QPixmap(":/new.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation), "Add task",
            this, SLOT(addNewTask()), Qt::Key_Return);
    rmTask = tasksMenu->addAction(
            QPixmap(":/delete.png").scaled(font_size.toInt() + 1, font_size.toInt() + 1, Qt::KeepAspectRatio, Qt::SmoothTransformation),
            "Delete task", this, SLOT(removeTask()), Qt::Key_Delete);
}