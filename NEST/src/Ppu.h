#pragma once

#include <QList>
#include <QColor>
#include <QImage>
#include <QWidget>


class Cpu;
class Ppu : public QWidget
{
    Q_OBJECT
private:
    Cpu& cpu;

public:
    QImage screen = QImage(256, 240, QImage::Format_RGB32);
    QImage fullScreen = QImage(512, 512, QImage::Format_RGB32);

    unsigned char ppuRam[0x4000];
    unsigned char oamRam[0x100];

    //Current palette
    //private Color[] palette = QColor[0x40] { QColor(0x52, 0x52, 0x52), QColor(0x01, 0x1A, 0x51), QColor(0x0F, 0x0F, 0x65), QColor(0x23, 0x06, 0x63), QColor(0x36, 0x03, 0x4B), QColor(0x40, 0x04, 0x26), QColor(0x3F, 0x09, 0x04), QColor(0x32, 0x13, 0x00), QColor(0x1F, 0x20, 0x00), QColor(0x0B, 0x2A, 0x00), QColor(0x00, 0x2F, 0x00), QColor(0x00, 0x2E, 0x0A), QColor(0x00, 0x26, 0x2D), QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00), QColor(0xA0, 0xA0, 0xA0), QColor(0x1E, 0x4A, 0x9D), QColor(0x38, 0x37, 0xBC), QColor(0x58, 0x28, 0xB8), QColor(0x75, 0x21, 0x94), QColor(0x84, 0x23, 0x5C), QColor(0x82, 0x2E, 0x24), QColor(0x6F, 0x3F, 0x00), QColor(0x51, 0x52, 0x00), QColor(0x31, 0x63, 0x00), QColor(0x1A, 0x6B, 0x05), QColor(0x0E, 0x69, 0x2E), QColor(0x10, 0x5C, 0x68), QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00), QColor(0xFE, 0xFF, 0xFF), QColor(0x69, 0x9E, 0xFC), QColor(0x89, 0x87, 0xFF), QColor(0xAE, 0x76, 0xFF), QColor(0xCE, 0x6D, 0xF1), QColor(0xE0, 0x70, 0xB2), QColor(0xDE, 0x7C, 0x70), QColor(0xC8, 0x91, 0x3E), QColor(0xA6, 0xA7, 0x25), QColor(0x81, 0xBA, 0x28), QColor(0x63, 0xC4, 0x46), QColor(0x54, 0xC1, 0x7D), QColor(0x56, 0xB3, 0xC0), QColor(0x3C, 0x3C, 0x3C), QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00), QColor(0xFE, 0xFF, 0xFF), QColor(0xBE, 0xD6, 0xFD), QColor(0xCC, 0xCC, 0xFF), QColor(0xDD, 0xC4, 0xFF), QColor(0xEA, 0xC0, 0xF9), QColor(0xF2, 0xC1, 0xDF), QColor(0xF1, 0xC7, 0xC2), QColor(0xE8, 0xD0, 0xAA), QColor(0xD9, 0xDA, 0x9D), QColor(0xC9, 0xE2, 0x9E), QColor(0xBC, 0xE6, 0xAE), QColor(0xB4, 0xE5, 0xC7), QColor(0xB5, 0xDF, 0xE4), QColor(0xA9, 0xA9, 0xA9), QColor(0x00, 0x00, 0x00), QColor(0x00, 0x00, 0x00) };

    bool scrollWrittenOnce       = false;
    bool ppuAddressWrittenOnce   = false;
    bool oamAddressWrittenOnce   = false;
    ushort ppuWriteAddress       = 0;
    ushort tempPPUWriteAddress   = 0;
    ushort oamWriteAddress       = 0;
    ushort tempOAMWriteAddress   = 0;
    bool pendingNMI              = false;
    bool spriteZeroHit           = false;
    bool spriteOverflow          = false;
    bool isNametableMirrored     = false;
    bool isHorizNametableMirror  = false;
    bool isVertNametableMirror   = false;

    unsigned char scrollX       = 0;
    unsigned char scrollY       = 0;
    int  ly                     = 0;
    unsigned char ppuState      = 0;
    uint frameCount             = 0;

    const unsigned char PPU_STATE_PRERENDER   = 0b00000000;
    const unsigned char PPU_STATE_DRAWING     = 0b00000001;
    const unsigned char PPU_STATE_POSTRENDER  = 0b00000010;
    const unsigned char PPU_STATE_VBLANK      = 0b00000011;

    QColor palette[0x40] = {
        QColor(0x7C, 0x7C, 0x7C, 0xFF),
        QColor(0x00, 0x34, 0x9C, 0xFF),
        QColor(0x00, 0x0B, 0xC3, 0xFF),
        QColor(0x34, 0x09, 0xC0, 0xFF),
        QColor(0x81, 0x06, 0x97, 0xFF),
        QColor(0xAE, 0x01, 0x49, 0xFF),
        QColor(0xBA, 0x00, 0x00, 0xFF),
        QColor(0x9D, 0x00, 0x00, 0xFF),
        QColor(0x50, 0x25, 0x00, 0xFF),
        QColor(0x00, 0x44, 0x00, 0xFF),
        QColor(0x00, 0x53, 0x00, 0xFF),
        QColor(0x00, 0x54, 0x00, 0xFF),
        QColor(0x00, 0x4B, 0x52, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0xD1, 0xD1, 0xD1, 0xFF),
        QColor(0x00, 0x87, 0xF7, 0xFF),
        QColor(0x20, 0x5D, 0xFF, 0xFF),
        QColor(0x84, 0x3D, 0xFF, 0xFF),
        QColor(0xE8, 0x22, 0xF0, 0xFF),
        QColor(0xFF, 0x24, 0x98, 0xFF),
        QColor(0xFF, 0x34, 0x2A, 0xFF),
        QColor(0xF5, 0x4D, 0x00, 0xFF),
        QColor(0xA6, 0x71, 0x00, 0xFF),
        QColor(0x40, 0x8F, 0x00, 0xFF),
        QColor(0x00, 0xA1, 0x00, 0xFF),
        QColor(0x00, 0xA5, 0x2E, 0xFF),
        QColor(0x00, 0x9D, 0xA5, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0xFE, 0xFF, 0xFF, 0xFF),
        QColor(0x36, 0xB5, 0xFF, 0xFF),
        QColor(0x68, 0x8F, 0xFF, 0xFF),
        QColor(0xA7, 0x7D, 0xFF, 0xFF),
        QColor(0xF8, 0x7C, 0xFF, 0xFF),
        QColor(0xFF, 0x8A, 0xD2, 0xFF),
        QColor(0xFF, 0x8E, 0x89, 0xFF),
        QColor(0xFF, 0xA6, 0x40, 0xFF),
        QColor(0xFF, 0xC9, 0x05, 0xFF),
        QColor(0xB1, 0xE3, 0x00, 0xFF),
        QColor(0x5B, 0xF6, 0x35, 0xFF),
        QColor(0x1E, 0xFD, 0x9C, 0xFF),
        QColor(0x13, 0xF6, 0xFD, 0xFF),
        QColor(0x5E, 0x5E, 0x5E, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0xFE, 0xFF, 0xFF, 0xFF),
        QColor(0xAE, 0xE0, 0xFF, 0xFF),
        QColor(0xBC, 0xCD, 0xFF, 0xFF),
        QColor(0xD6, 0xC5, 0xFF, 0xFF),
        QColor(0xFB, 0xCA, 0xFF, 0xFF),
        QColor(0xFF, 0xCF, 0xED, 0xFF),
        QColor(0xFF, 0xD1, 0xCF, 0xFF),
        QColor(0xFF, 0xDD, 0xB9, 0xFF),
        QColor(0xFF, 0xF2, 0xB0, 0xFF),
        QColor(0xE7, 0xFA, 0xAE, 0xFF),
        QColor(0xC6, 0xFC, 0xBD, 0xFF),
        QColor(0xB3, 0xFE, 0xDC, 0xFF),
        QColor(0xB2, 0xFE, 0xFF, 0xFF),
        QColor(0xDA, 0xDA, 0xDA, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF),
        QColor(0x00, 0x00, 0x00, 0xFF)
    };


    Ppu(QWidget* parent, Cpu& cpu);
    ~Ppu();
    unsigned char readPPURamByte(ushort address);
    void writePPURamByte(ushort address, unsigned char value);
    unsigned char readOAMRamByte(ushort address);
    void writeOAMRamByte(ushort address, unsigned char value);
    unsigned char getPPUStatus();
    unsigned char getPPURegister();
    unsigned char getPPURegisterNameTableSetting();
    unsigned char getPPURegisterVRAMIncrement();
    bool getPPURegisterSpritePatternTableSetting();
    bool getPPURegisterBackgroundPatternTableSetting();
    bool getPPURegisterSpriteSizeSetting();
    bool getPPURegisterNMISetting();
    unsigned char getPPUMask();
    bool getMaskGreyscaleEnabled();
    bool getMaskLeft8BGPixelsEnabled();
    bool getMaskLeft8SpritesPixelsEnabled();
    bool getMaskBackgroundEnabled();
    bool getMaskSpriteEnabled();
    bool getMaskEmphasizeRedEnabled();
    bool getMaskEmphasizeGreenEnabled();
    bool getMaskEmphasizeBlueEnabled();
    void oamDMATransfer(ushort address);

    void drawLineToFrame(QList<QColor> backGroundLine, QList<QColor>  spriteLine, uint ly);
signals:
    void drawImageToScreen(QImage image);
};

