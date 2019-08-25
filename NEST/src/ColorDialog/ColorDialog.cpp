#include "ColorDialog.h"
#include "ui_ColorDialog.h"

#include <QFileDialog>

#include "PaletteListView.h"
#include "ColorSelectorWidget.h"
#include "src/Ppu.h"

ColorDialog::ColorDialog(QWidget *parent, Ppu* ppu) :
    QDialog(parent),
    ui(new Ui::ColorDialog)
{
    ui->setupUi(this);
    this->ppu = ppu;

    ui->listView->setContextMenuPolicy(Qt::ActionsContextMenu);
    selectAction = new QAction ("Select Palette", this);
    ui->listView->addAction(selectAction);

    loadPalettes();

    QObject::connect(selectAction, &QAction::triggered,
                        this, &ColorDialog::setPalette);
    QObject::connect(ui->listView, &PaletteListView::selectedIndexChanged,
                        this, &ColorDialog::indexChanged);
    QObject::connect(ui->saveToFileButton, &QPushButton::pressed,
                        this, &ColorDialog::savePaletteToFile);
    QObject::connect(ui->newPaletteButton, &QPushButton::pressed,
                        this, &ColorDialog::addNewPalette);
    QObject::connect(ui->deletePaletteButton, &QPushButton::pressed,
                        this, &ColorDialog::deleteCurrentPalette);
    QObject::connect(ui->loadFromFileButton, &QPushButton::pressed,
                        this, &ColorDialog::loadPaletteFromFile);
    QObject::connect(qobject_cast<QStandardItemModel*>(ui->listView->model()), &QStandardItemModel::itemChanged,
                        this, &ColorDialog::listItemRenamed);
}

void ColorDialog::addNewPalette() {
    // Add new palette item to QListView.
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    model->appendRow(new QStandardItem("New Palette"));

    // Add new palette to PPU palette list to match.
    Palette newPalette = ppu->defaultPalette;
    ppu->palettes.push_back(newPalette);

    // Select/highlight the new item in the QListView and scroll to it.
    QModelIndex newSelectionIndex = model->index(model->rowCount() - 1, 0, QModelIndex());
    ui->listView->setCurrentIndex(newSelectionIndex);
    ui->listView->selectionModel()->select(newSelectionIndex, QItemSelectionModel::Select);
    ui->listView->scrollTo(newSelectionIndex);
}

void ColorDialog::deleteCurrentPalette() {
    int currentIndex = ui->listView->currentIndex().row();
    if (currentIndex >= 0 && currentIndex < ppu->palettes.count()) {
        ui->listView->model()->removeRow(currentIndex, ui->listView->model()->index(currentIndex, 1, QModelIndex()));
        ppu->palettes.removeAt(currentIndex);
    }
}

void ColorDialog::listItemRenamed(QStandardItem* item) {
    int rowIndex = item->row();

    if (rowIndex >= 0 && rowIndex < ppu->palettes.count()) {
        QString newName = item->text();
        if (newName.length() > 0) {
            ppu->palettes[rowIndex].paletteName = newName;
        }
        else {
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
            QString originalName = ppu->palettes[rowIndex].paletteName;
            model->item(rowIndex, 0)->setText(originalName);
        }
    }
}

void ColorDialog::loadPalettes() {

    QStandardItemModel* itemModel = new QStandardItemModel(ppu->palettes.count(), 1, this);
    int paletteCount = ppu->palettes.count();

    if (paletteCount == 0) {
        ppu->palettes.push_front(ppu->defaultPalette);
    }

    for(int i = 0; i < paletteCount; i++)
    {
        QStandardItem* paletteName = new QStandardItem(ppu->palettes[i].paletteName);
        itemModel->setItem(i, paletteName);
    }

    ui->listView->setModel(itemModel);

    //Set first palette to preview
    setPalettePreviews(0);

    // Select and highlight first palette.
    QModelIndex firstItemIndex = itemModel->index(0, 0, QModelIndex());
    ui->listView->setCurrentIndex(firstItemIndex);
    ui->listView->selectionModel()->select(firstItemIndex, QItemSelectionModel::Select);
    ui->listView->scrollTo(firstItemIndex);
}

void ColorDialog::loadPaletteFromFile() {
    QString appPath = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QString openFilePath = QFileDialog::getOpenFileName(this, "Open Palette file", appPath, tr("Palettes (*.pal)"));
    QFile file(openFilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray paletteData = file.readAll();
    QString paletteName = QInputDialog::getText(this, "Enter Palette Name", "Palette Name:", QLineEdit::Normal, "");

    // Add new palette item to QListView.
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->listView->model());
    model->appendRow(new QStandardItem(paletteName));

    // Add new palette to PPU palette list to match.
    Palette newPalette = { paletteName, QVector<QColor>(64) };

    for (int i = 0; i < 64; i++) {
        unsigned char r = paletteData[(i*3) + 0];
        unsigned char g = paletteData[(i*3) + 1];
        unsigned char b = paletteData[(i*3) + 2];
        QColor nextColor = QColor(r, g, b, 0xFF);
        newPalette.paletteColors[i] = nextColor;
    }

    ppu->palettes.push_back(newPalette);

    // Select/highlight the new item in the QListView and scroll to it.
    QModelIndex newSelectionIndex = model->index(model->rowCount() - 1, 0, QModelIndex());
    ui->listView->setCurrentIndex(newSelectionIndex);
    ui->listView->selectionModel()->select(newSelectionIndex, QItemSelectionModel::Select);
    ui->listView->scrollTo(newSelectionIndex);


    //Set first palette to preview
    setPalettePreviews(newSelectionIndex.row());

    file.close();
}

void ColorDialog::setPalettePreviews(int index) {
    if (index > -1)
    {
        QColor color = ppu->palettes[index].paletteColors[0];
        unsigned char r = color.red();
        unsigned char g = color.green();
        unsigned char b = color.blue();
        setPreviewColor(ui->colorWidget0, ppu->palettes[index].paletteColors[0]);
        setPreviewColor(ui->colorWidget0_2, ppu->palettes[index].paletteColors[(16 * 1) + 0]);
        setPreviewColor(ui->colorWidget0_3, ppu->palettes[index].paletteColors[(16 * 2) + 0]);
        setPreviewColor(ui->colorWidget0_4, ppu->palettes[index].paletteColors[(16 * 3) + 0]);

        setPreviewColor(ui->colorWidget1, ppu->palettes[index].paletteColors[1]);
        setPreviewColor(ui->colorWidget1_2, ppu->palettes[index].paletteColors[(16 * 1) + 1]);
        setPreviewColor(ui->colorWidget1_3, ppu->palettes[index].paletteColors[(16 * 2) + 1]);
        setPreviewColor(ui->colorWidget1_4, ppu->palettes[index].paletteColors[(16 * 3) + 1]);

        setPreviewColor(ui->colorWidget2, ppu->palettes[index].paletteColors[2]);
        setPreviewColor(ui->colorWidget2_2, ppu->palettes[index].paletteColors[(16 * 1) + 2]);
        setPreviewColor(ui->colorWidget2_3, ppu->palettes[index].paletteColors[(16 * 2) + 2]);
        setPreviewColor(ui->colorWidget2_4, ppu->palettes[index].paletteColors[(16 * 3) + 2]);

        setPreviewColor(ui->colorWidget3, ppu->palettes[index].paletteColors[3]);
        setPreviewColor(ui->colorWidget3_2, ppu->palettes[index].paletteColors[(16 * 1) + 3]);
        setPreviewColor(ui->colorWidget3_3, ppu->palettes[index].paletteColors[(16 * 2) + 3]);
        setPreviewColor(ui->colorWidget3_4, ppu->palettes[index].paletteColors[(16 * 3) + 3]);

        setPreviewColor(ui->colorWidget4, ppu->palettes[index].paletteColors[4]);
        setPreviewColor(ui->colorWidget4_2, ppu->palettes[index].paletteColors[(16 * 1) + 4]);
        setPreviewColor(ui->colorWidget4_3, ppu->palettes[index].paletteColors[(16 * 2) + 4]);
        setPreviewColor(ui->colorWidget4_4, ppu->palettes[index].paletteColors[(16 * 3) + 4]);

        setPreviewColor(ui->colorWidget5, ppu->palettes[index].paletteColors[5]);
        setPreviewColor(ui->colorWidget5_2, ppu->palettes[index].paletteColors[(16 * 1) + 5]);
        setPreviewColor(ui->colorWidget5_3, ppu->palettes[index].paletteColors[(16 * 2) + 5]);
        setPreviewColor(ui->colorWidget5_4, ppu->palettes[index].paletteColors[(16 * 3) + 5]);

        setPreviewColor(ui->colorWidget6, ppu->palettes[index].paletteColors[6]);
        setPreviewColor(ui->colorWidget6_2, ppu->palettes[index].paletteColors[(16 * 1) + 6]);
        setPreviewColor(ui->colorWidget6_3, ppu->palettes[index].paletteColors[(16 * 2) + 6]);
        setPreviewColor(ui->colorWidget6_4, ppu->palettes[index].paletteColors[(16 * 3) + 6]);

        setPreviewColor(ui->colorWidget7, ppu->palettes[index].paletteColors[7]);
        setPreviewColor(ui->colorWidget7_2, ppu->palettes[index].paletteColors[(16 * 1) + 7]);
        setPreviewColor(ui->colorWidget7_3, ppu->palettes[index].paletteColors[(16 * 2) + 7]);
        setPreviewColor(ui->colorWidget7_4, ppu->palettes[index].paletteColors[(16 * 3) + 7]);

        setPreviewColor(ui->colorWidget8, ppu->palettes[index].paletteColors[8]);
        setPreviewColor(ui->colorWidget8_2, ppu->palettes[index].paletteColors[(16 * 1) + 8]);
        setPreviewColor(ui->colorWidget8_3, ppu->palettes[index].paletteColors[(16 * 2) + 8]);
        setPreviewColor(ui->colorWidget8_4, ppu->palettes[index].paletteColors[(16 * 3) + 8]);

        setPreviewColor(ui->colorWidget9, ppu->palettes[index].paletteColors[9]);
        setPreviewColor(ui->colorWidget9_2, ppu->palettes[index].paletteColors[(16 * 1) + 9]);
        setPreviewColor(ui->colorWidget9_3, ppu->palettes[index].paletteColors[(16 * 2) + 9]);
        setPreviewColor(ui->colorWidget9_4, ppu->palettes[index].paletteColors[(16 * 3) + 9]);

        setPreviewColor(ui->colorWidget10, ppu->palettes[index].paletteColors[10]);
        setPreviewColor(ui->colorWidget10_2, ppu->palettes[index].paletteColors[(16 * 1) + 10]);
        setPreviewColor(ui->colorWidget10_3, ppu->palettes[index].paletteColors[(16 * 2) + 10]);
        setPreviewColor(ui->colorWidget10_4, ppu->palettes[index].paletteColors[(16 * 3) + 10]);

        setPreviewColor(ui->colorWidget11, ppu->palettes[index].paletteColors[11]);
        setPreviewColor(ui->colorWidget11_2, ppu->palettes[index].paletteColors[(16 * 1) + 11]);
        setPreviewColor(ui->colorWidget11_3, ppu->palettes[index].paletteColors[(16 * 2) + 11]);
        setPreviewColor(ui->colorWidget11_4, ppu->palettes[index].paletteColors[(16 * 3) + 11]);

        setPreviewColor(ui->colorWidget12, ppu->palettes[index].paletteColors[12]);
        setPreviewColor(ui->colorWidget12_2, ppu->palettes[index].paletteColors[(16 * 1) + 12]);
        setPreviewColor(ui->colorWidget12_3, ppu->palettes[index].paletteColors[(16 * 2) + 12]);
        setPreviewColor(ui->colorWidget12_4, ppu->palettes[index].paletteColors[(16 * 3) + 12]);

        setPreviewColor(ui->colorWidget13, ppu->palettes[index].paletteColors[13]);
        setPreviewColor(ui->colorWidget13_2, ppu->palettes[index].paletteColors[(16 * 1) + 13]);
        setPreviewColor(ui->colorWidget13_3, ppu->palettes[index].paletteColors[(16 * 2) + 13]);
        setPreviewColor(ui->colorWidget13_4, ppu->palettes[index].paletteColors[(16 * 3) + 13]);

        setPreviewColor(ui->colorWidget14, ppu->palettes[index].paletteColors[14]);
        setPreviewColor(ui->colorWidget14_2, ppu->palettes[index].paletteColors[(16 * 1) + 14]);
        setPreviewColor(ui->colorWidget14_3, ppu->palettes[index].paletteColors[(16 * 2) + 14]);
        setPreviewColor(ui->colorWidget14_4, ppu->palettes[index].paletteColors[(16 * 3) + 14]);

        setPreviewColor(ui->colorWidget15, ppu->palettes[index].paletteColors[15]);
        setPreviewColor(ui->colorWidget15_2, ppu->palettes[index].paletteColors[(16 * 1) + 15]);
        setPreviewColor(ui->colorWidget15_3, ppu->palettes[index].paletteColors[(16 * 2) + 15]);
        setPreviewColor(ui->colorWidget15_4, ppu->palettes[index].paletteColors[(16 * 3) + 15]);
    }
    else
    {
        QColor white = QColor(255, 255, 255, 255);
        setPreviewColor(ui->colorWidget0, white);
        setPreviewColor(ui->colorWidget0_2, white);
        setPreviewColor(ui->colorWidget0_3, white);
        setPreviewColor(ui->colorWidget0_4, white);

        setPreviewColor(ui->colorWidget1, white);
        setPreviewColor(ui->colorWidget1_2, white);
        setPreviewColor(ui->colorWidget1_3, white);
        setPreviewColor(ui->colorWidget1_4, white);

        setPreviewColor(ui->colorWidget2, white);
        setPreviewColor(ui->colorWidget2_2, white);
        setPreviewColor(ui->colorWidget2_3, white);
        setPreviewColor(ui->colorWidget2_4, white);

        setPreviewColor(ui->colorWidget3, white);
        setPreviewColor(ui->colorWidget3_2, white);
        setPreviewColor(ui->colorWidget3_3, white);
        setPreviewColor(ui->colorWidget3_4, white);

        setPreviewColor(ui->colorWidget4, white);
        setPreviewColor(ui->colorWidget4_2, white);
        setPreviewColor(ui->colorWidget4_3, white);
        setPreviewColor(ui->colorWidget4_4, white);

        setPreviewColor(ui->colorWidget5, white);
        setPreviewColor(ui->colorWidget5_2, white);
        setPreviewColor(ui->colorWidget5_3, white);
        setPreviewColor(ui->colorWidget5_4, white);

        setPreviewColor(ui->colorWidget6, white);
        setPreviewColor(ui->colorWidget6_2, white);
        setPreviewColor(ui->colorWidget6_3, white);
        setPreviewColor(ui->colorWidget6_4, white);

        setPreviewColor(ui->colorWidget7, white);
        setPreviewColor(ui->colorWidget7_2, white);
        setPreviewColor(ui->colorWidget7_3, white);
        setPreviewColor(ui->colorWidget7_4, white);

        setPreviewColor(ui->colorWidget8, white);
        setPreviewColor(ui->colorWidget8_2, white);
        setPreviewColor(ui->colorWidget8_3, white);
        setPreviewColor(ui->colorWidget8_4, white);

        setPreviewColor(ui->colorWidget9, white);
        setPreviewColor(ui->colorWidget9_2, white);
        setPreviewColor(ui->colorWidget9_3, white);
        setPreviewColor(ui->colorWidget9_4, white);

        setPreviewColor(ui->colorWidget10, white);
        setPreviewColor(ui->colorWidget10_2, white);
        setPreviewColor(ui->colorWidget10_3, white);
        setPreviewColor(ui->colorWidget10_4, white);

        setPreviewColor(ui->colorWidget11, white);
        setPreviewColor(ui->colorWidget11_2, white);
        setPreviewColor(ui->colorWidget11_3, white);
        setPreviewColor(ui->colorWidget11_4, white);

        setPreviewColor(ui->colorWidget12, white);
        setPreviewColor(ui->colorWidget12_2, white);
        setPreviewColor(ui->colorWidget12_3, white);
        setPreviewColor(ui->colorWidget12_4, white);

        setPreviewColor(ui->colorWidget13, white);
        setPreviewColor(ui->colorWidget13_2, white);
        setPreviewColor(ui->colorWidget13_3, white);
        setPreviewColor(ui->colorWidget13_4, white);

        setPreviewColor(ui->colorWidget14, white);
        setPreviewColor(ui->colorWidget14_2, white);
        setPreviewColor(ui->colorWidget14_3, white);
        setPreviewColor(ui->colorWidget14_4, white);

        setPreviewColor(ui->colorWidget15, white);
        setPreviewColor(ui->colorWidget15_2, white);
        setPreviewColor(ui->colorWidget15_3, white);
        setPreviewColor(ui->colorWidget15_4, white);
    }
}

void ColorDialog::setPreviewColor(ColorSelectorWidget* colorWidget, QColor& color) {
    colorWidget->setColor(color);
}

void ColorDialog::indexChanged(int row, int column) {
    int selectedIndex = row;
    if (selectedIndex >= 0 && selectedIndex < ppu->palettes.count()) {
        setPalettePreviews(selectedIndex);
    }
}

void ColorDialog::setPalette() {
    int paletteRow = ui->listView->currentIndex().row();
    overwritePPUPaletteAtIndexWithCurrentPalette(paletteRow);

    ppu->paletteSetting = paletteRow;
}

void ColorDialog::overwritePPUPaletteAtIndexWithCurrentPalette(int index) {
    if (ppu->palettes.length() > index) {
         ppu->palettes[index].paletteColors[0] = ui->colorWidget0->getColor();
         ppu->palettes[index].paletteColors[16] = ui->colorWidget0_2->getColor();
         ppu->palettes[index].paletteColors[32] = ui->colorWidget0_3->getColor();
         ppu->palettes[index].paletteColors[48] = ui->colorWidget0_4->getColor();

         ppu->palettes[index].paletteColors[1] = ui->colorWidget1->getColor();
         ppu->palettes[index].paletteColors[17] = ui->colorWidget1_2->getColor();
         ppu->palettes[index].paletteColors[33] = ui->colorWidget1_3->getColor();
         ppu->palettes[index].paletteColors[49] = ui->colorWidget1_4->getColor();

         ppu->palettes[index].paletteColors[2] = ui->colorWidget2->getColor();
         ppu->palettes[index].paletteColors[18] = ui->colorWidget2_2->getColor();
         ppu->palettes[index].paletteColors[34] = ui->colorWidget2_3->getColor();
         ppu->palettes[index].paletteColors[50] = ui->colorWidget2_4->getColor();

         ppu->palettes[index].paletteColors[3] = ui->colorWidget3->getColor();
         ppu->palettes[index].paletteColors[19] = ui->colorWidget3_2->getColor();
         ppu->palettes[index].paletteColors[35] = ui->colorWidget3_3->getColor();
         ppu->palettes[index].paletteColors[51] = ui->colorWidget3_4->getColor();

         ppu->palettes[index].paletteColors[4] = ui->colorWidget4->getColor();
         ppu->palettes[index].paletteColors[20] = ui->colorWidget4_2->getColor();
         ppu->palettes[index].paletteColors[36] = ui->colorWidget4_3->getColor();
         ppu->palettes[index].paletteColors[52] = ui->colorWidget4_4->getColor();

         ppu->palettes[index].paletteColors[5] = ui->colorWidget5->getColor();
         ppu->palettes[index].paletteColors[21] = ui->colorWidget5_2->getColor();
         ppu->palettes[index].paletteColors[37] = ui->colorWidget5_3->getColor();
         ppu->palettes[index].paletteColors[53] = ui->colorWidget5_4->getColor();

         ppu->palettes[index].paletteColors[6] = ui->colorWidget6->getColor();
         ppu->palettes[index].paletteColors[22] = ui->colorWidget6_2->getColor();
         ppu->palettes[index].paletteColors[38] = ui->colorWidget6_3->getColor();
         ppu->palettes[index].paletteColors[54] = ui->colorWidget6_4->getColor();

         ppu->palettes[index].paletteColors[7] = ui->colorWidget7->getColor();
         ppu->palettes[index].paletteColors[23] = ui->colorWidget7_2->getColor();
         ppu->palettes[index].paletteColors[39] = ui->colorWidget7_3->getColor();
         ppu->palettes[index].paletteColors[55] = ui->colorWidget7_4->getColor();

         ppu->palettes[index].paletteColors[8] = ui->colorWidget8->getColor();
         ppu->palettes[index].paletteColors[24] = ui->colorWidget8_2->getColor();
         ppu->palettes[index].paletteColors[40] = ui->colorWidget8_3->getColor();
         ppu->palettes[index].paletteColors[56] = ui->colorWidget8_4->getColor();

         ppu->palettes[index].paletteColors[9] = ui->colorWidget9->getColor();
         ppu->palettes[index].paletteColors[25] = ui->colorWidget9_2->getColor();
         ppu->palettes[index].paletteColors[41] = ui->colorWidget9_3->getColor();
         ppu->palettes[index].paletteColors[57] = ui->colorWidget9_4->getColor();

         ppu->palettes[index].paletteColors[10] = ui->colorWidget10->getColor();
         ppu->palettes[index].paletteColors[26] = ui->colorWidget10_2->getColor();
         ppu->palettes[index].paletteColors[42] = ui->colorWidget10_3->getColor();
         ppu->palettes[index].paletteColors[58] = ui->colorWidget10_4->getColor();

         ppu->palettes[index].paletteColors[11] = ui->colorWidget11->getColor();
         ppu->palettes[index].paletteColors[27] = ui->colorWidget11_2->getColor();
         ppu->palettes[index].paletteColors[43] = ui->colorWidget11_3->getColor();
         ppu->palettes[index].paletteColors[59] = ui->colorWidget11_4->getColor();

         ppu->palettes[index].paletteColors[12] = ui->colorWidget12->getColor();
         ppu->palettes[index].paletteColors[28] = ui->colorWidget12_2->getColor();
         ppu->palettes[index].paletteColors[44] = ui->colorWidget12_3->getColor();
         ppu->palettes[index].paletteColors[60] = ui->colorWidget12_4->getColor();

         ppu->palettes[index].paletteColors[13] = ui->colorWidget13->getColor();
         ppu->palettes[index].paletteColors[29] = ui->colorWidget13_2->getColor();
         ppu->palettes[index].paletteColors[45] = ui->colorWidget13_3->getColor();
         ppu->palettes[index].paletteColors[61] = ui->colorWidget13_4->getColor();

         ppu->palettes[index].paletteColors[14] = ui->colorWidget14->getColor();
         ppu->palettes[index].paletteColors[30] = ui->colorWidget14_2->getColor();
         ppu->palettes[index].paletteColors[46] = ui->colorWidget14_3->getColor();
         ppu->palettes[index].paletteColors[62] = ui->colorWidget14_4->getColor();;

         ppu->palettes[index].paletteColors[15] = ui->colorWidget15->getColor();
         ppu->palettes[index].paletteColors[31] = ui->colorWidget15_2->getColor();
         ppu->palettes[index].paletteColors[47] = ui->colorWidget15_3->getColor();
         ppu->palettes[index].paletteColors[63] = ui->colorWidget15_4->getColor();
    }
}

void ColorDialog::savePaletteToFile() {
    // Update PPU palettes with current selected palette.
    int paletteRow = ui->listView->currentIndex().row();

    if (paletteRow < 0 && paletteRow < ppu->palettes.count()) {
        return;
    }

    overwritePPUPaletteAtIndexWithCurrentPalette(paletteRow);

    // Write current PPU palette to .pal file at user selected destination.
    QString savePath = QFileDialog::getSaveFileName(this, "Save Palette as", NULL, "Palettes (*.pal)");
    if (!savePath.endsWith(".pal", Qt::CaseInsensitive)) {
        savePath += ".pal";
    }

    if (savePath.length() <= 0) {
        return;
    }

    QByteArray paletteData = QByteArray(64 * 3, 0);

    for (int i = 0; i < 64; i++) {
        QColor nextColor = ppu->palettes[paletteRow].paletteColors[i];
        unsigned char r = nextColor.red();
        unsigned char g = nextColor.green();
        unsigned char b = nextColor.blue();

       paletteData[(i * 3) + 0] = r;
       paletteData[(i * 3) + 1] = g;
       paletteData[(i * 3) + 2] = b;
    }

    QFile file(savePath);

    // Delete original palettes.xml file to write from scratch.
    file.remove();

    if (file.open(QIODevice::WriteOnly)) {
        file.write(paletteData);
    }
}

ColorDialog::~ColorDialog()
{
    QObject::disconnect(selectAction, &QAction::triggered,
                        this, &ColorDialog::setPalette);

    QObject::disconnect(ui->listView, &PaletteListView::selectedIndexChanged,
                        this, &ColorDialog::indexChanged);

    QObject::disconnect(ui->saveToFileButton, &QPushButton::pressed,
                        this, &ColorDialog::savePaletteToFile);

    QObject::disconnect(ui->newPaletteButton, &QPushButton::pressed,
                        this, &ColorDialog::addNewPalette);

    QObject::disconnect(ui->deletePaletteButton, &QPushButton::pressed,
                        this, &ColorDialog::deleteCurrentPalette);

    QObject::disconnect(ui->loadFromFileButton, &QPushButton::pressed,
                        this, &ColorDialog::loadPaletteFromFile);

    QObject::connect(qobject_cast<QStandardItemModel*>(ui->listView->model()), &QStandardItemModel::itemChanged,
                        this, &ColorDialog::listItemRenamed);

    ui->listView->removeAction(selectAction);
    delete selectAction;
    delete ui;
}
