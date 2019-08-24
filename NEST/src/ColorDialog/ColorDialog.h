#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QFile>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QPushButton>

namespace Ui {
class ColorDialog;
}

class PaletteListView;
class ColorSelectorWidget;
class Ppu;
class ColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent, Ppu* ppu);
    void loadPaletteFromFile();
    void loadPalettes();
    void setPalettePreviews(int index);
    void setPreviewColor(ColorSelectorWidget* colorWidget, QColor& color);
    void setPalette();
    void addNewPalette();
    void deleteCurrentPalette();
    void overwritePPUPaletteAtIndexWithCurrentPalette(int index);
    void indexChanged(int row, int column);
    void savePaletteToFile();
    ~ColorDialog();

public slots:
    void listItemRenamed(QStandardItem* item);

private:
    Ppu* ppu;
    Ui::ColorDialog *ui;
    QAction* selectAction;
};

#endif // COLORDIALOG_H
