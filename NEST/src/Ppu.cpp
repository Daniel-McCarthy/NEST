#include "Ppu.h"
#include "Cpu.h"

Ppu::Ppu(QWidget* parent, Cpu& cpu, uint& TOTAL_PPU_CLOCKS)
    : QWidget(parent), cpu(cpu), TOTAL_PPU_CLOCKS(TOTAL_PPU_CLOCKS)
{
}

Ppu::~Ppu()
{
}

unsigned char Ppu::readPPURamByte(ushort address) {
    return ppuRam[address];
}

void Ppu::writePPURamByte(ushort address, unsigned char value) {
    ppuRam[address] = value;
}

unsigned char Ppu::readOAMRamByte(ushort address) {
    return oamRam[address];
}

void Ppu::writeOAMRamByte(ushort address, unsigned char value) {
    oamRam[address] = value;
}

unsigned char Ppu::getPPUStatus() {
    unsigned char ppuStatus = 0;

    unsigned char PPU_STATE_VBLANK = 0b00000011;
    if (ppuState == PPU_STATE_VBLANK) {
        ppuStatus |= 0x80;
    }

    if (spriteZeroHit) {
        ppuStatus |= 0x40;
    }

    if (spriteOverflow) {
        ppuStatus |= 0x20;
    }

    //TODO: Bitwise Or the least significant bits of last byte written into PPU register

    return ppuStatus;
}

unsigned char Ppu::getPPURegister() {
    return cpu.readCPURam(0x2000, true);
}

unsigned char Ppu::getPPURegisterNameTableSetting() {
    //Base Name Table Address Setting
    //0: 0x2000, 1: 0x2400, 2: 0x2800, 3: 0x2C00
    return (unsigned char)(getPPURegister() & 0b00000011);
}

unsigned char Ppu::getPPURegisterVRAMIncrement() {
    //PPU Write Address Increment per read/write:
    //0: Increment of 1 (left to right)
    //1: Increment of 32 (traverses downward)

    return (unsigned char)(((getPPURegister() & 0b00000100) != 0) ? 32 : 1);
}

bool Ppu::getPPURegisterSpritePatternTableSetting() {
    //Pattern Table address for 8x8 sprites
    //0: 0x0000, 1: 0x1000
    return (getPPURegister() & 0b0001000) != 0;
}

bool Ppu::getPPURegisterBackgroundPatternTableSetting() {
    //Pattern Table address for background
    //0: 0x0000, 1: 0x1000
    return (getPPURegister() & 0b00010000) != 0;
}

bool Ppu::getPPURegisterSpriteSizeSetting() {
    //Sprite size Setting
    //0: 8x8 Sprites, 1: 8x16 Sprites
    return (getPPURegister() & 0b00100000) != 0;
}

bool Ppu::getPPURegisterNMISetting() {
    //Generate NMI at V-Blank setting
    //0: False
    //1: True
    return (getPPURegister() & 0b10000000) != 0;
}

unsigned char Ppu::getPPUMask() {
    return cpu.readCPURam(0x2001, true);
}

bool Ppu::getMaskGreyscaleEnabled() {
    return (getPPUMask() & 0b1) != 0;
}

bool Ppu::getMaskLeft8BGPixelsEnabled() {
    return (getPPUMask() & 0b10) != 0;
}

bool Ppu::getMaskLeft8SpritesPixelsEnabled() {
    return (getPPUMask() & 0b100) != 0;
}

bool Ppu::getMaskBackgroundEnabled() {
    return (getPPUMask() & 0b1000) != 0;
}

bool Ppu::getMaskSpriteEnabled() {
    return (getPPUMask() & 0b10000) != 0;
}

bool Ppu::getMaskEmphasizeRedEnabled() {
    return (getPPUMask() & 0b100000) != 0;
}

bool Ppu::getMaskEmphasizeGreenEnabled() {
    return (getPPUMask() & 0b1000000) != 0;
}

bool Ppu::getMaskEmphasizeBlueEnabled() {
    return (getPPUMask() & 0b10000000) != 0;
}

void Ppu::oamDMATransfer(ushort address) {
    for (int i = 0; i <= 0xFF; i++)
    {
        oamRam[i] = cpu.readCPURam((ushort)(address + i), false);
    }
}

void Ppu::updatePPU(uint& ppuClocks) {
    //Steps:
    //Pre-render scan line 261 && -1
    //Scan lines 0-239
    //Post-render scan line 240
    //V-Blank 241-260

    //Information:
    //262 scanlines
    //240 screen drawn scan lines
    //341 PPU clock cycles per scan line
    //113 CPU clock cycles per scan line

    //int CLOCKS_PER_SCANLINE = 341;

    if (ppuState == PPU_STATE_PRERENDER) {
        spriteZeroHit = false;

        if (ppuClocks >= 341) {
            ppuClocks -= 341;

            if(ly == -1) {
                //If this was the final pre-render scanline, switch to drawing state
                ly++;
                ppuState = PPU_STATE_DRAWING;
            } else {
                //If this was pre-render scanline 261, then switch to final pre-render scanline
                ly = -1;
            }
        }

    } else if (ppuState == PPU_STATE_DRAWING) {
        if (ppuClocks >= 341) {
            ppuClocks -= 341;

            QVector<QColor> bgLine = drawBGFrameLine((uint)ly);
            QVector<QColor> spriteLine = drawSpriteLine((unsigned char)ly);

            drawLineToFrame(bgLine, spriteLine, (uint)ly);

            ly++;

            if(ly == 240) {
                ppuState = PPU_STATE_POSTRENDER;
            }
        }

    } else if (ppuState == PPU_STATE_POSTRENDER) {
        if (ppuClocks >= 341) {
            ppuClocks -= 341;

            //If this was the final pre-render scanline, switch to drawing state
            ly++;
            ppuState = PPU_STATE_VBLANK;

            if (getPPURegisterNMISetting()) {
                pendingNMI = true;
            }

            //Draw frame to screen
            drawImageToScreen(screen);

            int millisecondsElapsed = screenUpdateTimer.elapsed();
            if (millisecondsElapsed == 0) {
                screenUpdateTimer.start();
            } else {
                if (millisecondsElapsed < 16) {
                    QThread::msleep(16 - millisecondsElapsed);
                }
                screenUpdateTimer.restart();
            }
        }

    } else if (ppuState == PPU_STATE_VBLANK) {
        if (ppuClocks >= 341) {
            ppuClocks -= 341;
            ly++;
        }

        if (ly == 261) {
            bool isEvenFrame = (frameCount % 2) == 0;
            if (isEvenFrame) {
                ppuState = PPU_STATE_PRERENDER;
            }  else {
                ppuState = PPU_STATE_DRAWING;
                ly = 0;
            }

            frameCount++;
        }
    }
}

/*
 * This function accepts a 256px wide background and sprite line and writes them to the frame, with the background below and the sprites on top.
 */
void Ppu::drawLineToFrame(QVector<QColor> backGroundLine, QVector<QColor>  spriteLine, uint ly) {
    if (ly < 240) {
        for (uint x = 0; x < 256; x++) {
            screen.setPixelColor(x, ly, backGroundLine[x]);

            if (spriteLine[x].alpha() != 0) {
                screen.setPixelColor(x, ly, spriteLine[x]);
            }
        }
    }
}

/*
 * This function draws a full line to the full 512x512 window. It draws from both the left and right name tables at the line specified.
 * It draws a full line and accounts for name table mirroring. From here the data is drawn to the window to be read in later.
 */
void Ppu::drawFullBGLineToWindow(uint lineNumber) {
    bool isLineUpperTable;

    if (isNametableMirrored && !isHorizNametableMirror)
    {
        //If name table is vertically mirrored, then draw the top. The bottom is just a copy.
        isLineUpperTable = true;
    } else {
        //If line is less in the lower half of 512 lines
        isLineUpperTable = (lineNumber < 256);
    }

    QVector<QColor> leftTable = drawBGLineFromNameTable(lineNumber % 256, true, isLineUpperTable);
    QVector<QColor> rightTable;

    if (isNametableMirrored && isHorizNametableMirror) {
        //If name table is horizontally mirrored, the draw the left table for both tables. The right is just a copy.
        rightTable = leftTable;
    } else {
        rightTable = drawBGLineFromNameTable(lineNumber % 256, false, isLineUpperTable);
    }

    for(int x = 0; x < 256; x++) {
        fullWindow.setPixelColor((uint)x, lineNumber, leftTable[x]);
        fullWindow.setPixelColor((uint)(x + 256), lineNumber, rightTable[x]);
    }
}

/*
 * Draws a full 256 wide line from a name table of choice at the line specified.
 * This allows us to read an entire name table line by line.
 */
QVector<QColor> Ppu::drawBGLineFromNameTable(uint lineNumber, bool isLeftTable, bool isUpperTable) {
    int nameTableSelection = (isUpperTable ? 0 : 2) + (isLeftTable ? 0 : 1); //Top Left: 0, Top Right: 1, Bottom Left: 2, Bottom Right: 3
    QVector<QColor> line = QVector<QColor>(256);

    int tileCount = 256 / 8;

    for (int tileIndex = 0; tileIndex < tileCount; tileIndex++)
    {
        //Determine which tile we are drawing
        int tileYPos = (int)lineNumber / 8;
        int tileXPos = tileIndex;
        int tileNumber = tileXPos + (tileYPos * 32);

        //Determine which name table cell the current tile is in
        int cellYPos = (tileYPos / 2);
        int cellXPos = (tileXPos / 2);
        int cellNumber = (cellYPos * 16) + cellXPos;
        bool leftTile = (tileXPos % 2) == 0;
        bool upperTile = (tileYPos % 2) == 0;
        bool leftCell = (cellXPos % 2) == 0;
        bool upperCell = (cellYPos % 2) == 0;


        //Determine the attribute byte for this cell

        //A Cell is made up of 2 rows of 2 tiles in a square.
        //One attribute byte defines the palettes for a 2x2 square of cells.
        //The bottom right cell is cell 1, bottom left is cell 2, upper right is cell 3, and upper left is cell 4.
        //The attribute data is 8 bits. 2 bits for each cell. These bits are the palette number.
        //Then we retrieve the 2 relevant bits that define the palette number for this tile.
        //The attribute value contains the palettes in the order of 0bAABBCCDD. AA is cell 4. BB is cell 3. CC is cell 2. DD is cell 1.

        int attributeRegion = ((cellYPos / 2) * 8) + (cellXPos / 2);
        unsigned char attributeValue = ppuRam[0x23C0 + (nameTableSelection * 0x400) + attributeRegion];
        unsigned char tilePalette = attributeValue;

        tilePalette = (upperCell) ? (unsigned char)(tilePalette & 0x0F) : (unsigned char)((tilePalette & 0xF0) >> 4);
        tilePalette = (!leftCell) ? (unsigned char)((tilePalette & 0b1100) >> 2) : (unsigned char)(tilePalette & 0b0011);

        //Determine the background pattern address
        ushort backgroundPatternTableAddress = getPPURegisterBackgroundPatternTableSetting() ? (ushort)(0x0000) : (ushort)(0x1000);

        //Read tileID from name table
        ushort nameTableAddress = (ushort)(0x2000 + (nameTableSelection * 0x400));
        unsigned char tileID = ppuRam[nameTableAddress + tileNumber];

        //Read current line of tile
        int yLineOffset = (int)lineNumber % 8; //This lets us know which line of the tile we are drawing, so that we can read the correct line data.
        ushort patternAddressTemp = (ushort)(backgroundPatternTableAddress + (tileID * 16) + (yLineOffset * 2));
        unsigned char tileDataRow1 = ppuRam[backgroundPatternTableAddress + (tileID * 16) + (yLineOffset)];
        unsigned char tileDataRow2 = ppuRam[backgroundPatternTableAddress + (tileID * 16) + (yLineOffset) + 8];

        QVector<unsigned char> tileColorIndices = QVector<unsigned char>(8);

        for (int i = 0; i < 8; i++) {
            //Patterns are defined by 16 bytes that detail an 8x8 pixel pattern. 2 bytes per line.
            //Each pixel can be one of four colors. 2 bytes are read in. The first bit
            //in the first byte and the first bit in the second byte define the color of the first pixel.
            tileColorIndices[i] = (unsigned char)(((tileDataRow1 & 0x01)) | (tileDataRow2 & 0x01) << 1);
            tileDataRow1 >>= 1;
            tileDataRow2 >>= 1;
        }

        // Reverse contents of tileColorIndices
        int tileColorCount = tileColorIndices.count();
        for (int i = 0; i < tileColorCount/2; i++) {
            unsigned char c = tileColorIndices[i];
            tileColorIndices[i] = tileColorIndices[(tileColorCount - 1) - i];
            tileColorIndices[(tileColorCount - 1) - i] = c;
        }

        //Draw current tile data to line
        for (int i = 0; i < 8; i++) {
            QColor pixelColor;

            if (tileColorIndices[i] != 0) {
                //Use the palette index retrieved from the attribute table to select the proper palette from 0x3F00-0x3F20
                //Select pixel color index from selected palette using tile color index retrieved from the tile data rows.
                //Use this final color index to index a color from the NES color palette.
                //unsigned char colorAddress = (unsigned char)((tilePalette * 4) + tileColorIndices[i]);
                unsigned char colorAddress = (unsigned char)((tilePalette << 2) | tileColorIndices[i]);
                unsigned char colorIndex = ppuRam[0x3F00 + colorAddress];
                pixelColor = palette[colorIndex];
            } else {
                //Read Default BG Color.
                pixelColor = palette[ppuRam[0x3F00]];
            }

            line[(tileIndex * 8) + i] = pixelColor;
        }
    }

    return line;
}


/*
 *  New line drawing function. It can cross name tables and account for scroll. It only draws the tiles needed for this particular line.
 *  Instead of drawing a full 512x1 line then selecting the pixels we need, we can reduce the work heavily by only drawing the tiles that will actually be used.
 */
QVector<QColor> Ppu::drawBGFrameLine(uint lineNumber) {
    QVector<QColor> bgLine = QVector<QColor>(256);

    bool isRightNametable = (getPPURegisterNameTableSetting() % 2) != 0; //Name table setting can be 0 to 3. 1 and 3 are both odd and both right side tables.
    bool isLowerNametable = (getPPURegisterNameTableSetting() > 1); //Name table setting can be 0 to 3. 2 and 3 are both greater than 1 and bottom tables.
    bool overrideHorizTable = false;
    bool overrideVertTable = false;
    uint xPosOffset = (uint)(isRightNametable ? 0xFF : 0x00); //Add 256
    uint yPosOffset = (uint)(isLowerNametable ? 0xF0 : 0x00); //Add 240
    uint actualXScroll = (scrollX + xPosOffset) % 512;
    uint actualYScroll = (scrollY + yPosOffset + lineNumber) % 512;

    uint actualReadLine = (scrollY + yPosOffset + lineNumber) % 512;

    int tileXPos = (int)(actualXScroll / 8);
    int tileYPos = (int)(actualReadLine / 8);

    uint totalPixelsDrawn = 0;

    //Draw pixels from first tile (could be partial tile)
    uint actualX = (uint)((actualXScroll + totalPixelsDrawn) % 512);
    tileXPos = (int)(actualX / 8);


    //Account for Nametable Mirroring
    if (isNametableMirrored) {
        if (isHorizNametableMirror) {
            isRightNametable = false;
            overrideHorizTable = true;
        }

        if (isVertNametableMirror) {
            isLowerNametable = false;
            overrideVertTable = true;
        }
    }

    bool horizontalNameTableSelection = (overrideHorizTable) ? (!isRightNametable) : (actualX < 256);
    bool verticalNameTableSelection = (overrideVertTable) ? (!isLowerNametable) : (actualYScroll < 256);
    QVector<QColor> firstTile = drawBGTileLineFromNameTable(actualReadLine % 8, horizontalNameTableSelection, verticalNameTableSelection, tileXPos % 32, tileYPos % 30);

    for (int x = (int)(actualXScroll % 8); x < 8; x++)
    {
        bgLine[totalPixelsDrawn++] = firstTile[x];
    }

    //Draw middle tiles (guaranteed full 8 pixel tiles)
    for (int x = (int)totalPixelsDrawn; x < 256; x += 8)
    {
        if (totalPixelsDrawn <= (256 - 8)) {
            actualX = (uint)((actualXScroll + x) % 512);
            tileXPos = (int)(actualX / 8);

            horizontalNameTableSelection = (overrideHorizTable) ? (!isRightNametable) : (actualX < 256);

            QVector<QColor> nextTile = drawBGTileLineFromNameTable(actualReadLine % 8, horizontalNameTableSelection, verticalNameTableSelection, tileXPos % 32, tileYPos % 30);

            for (int i = 0; i < 8; i++) {
                bgLine[totalPixelsDrawn++] = nextTile[i];
            }
        }  else {
            //End looping
            x = 256;
        }
    }

    if (totalPixelsDrawn <= 256) {
        //Draw pixels from last tile  (could be partial tile)
        actualX = (uint)((actualXScroll + totalPixelsDrawn) % 512);
        tileXPos = (int)(actualX / 8);

        horizontalNameTableSelection = (overrideHorizTable) ? (!isRightNametable) : (actualX < 256);
        QVector<QColor> lastTile = drawBGTileLineFromNameTable(actualReadLine % 8, horizontalNameTableSelection, verticalNameTableSelection, tileXPos % 32, tileYPos % 30);
        //QVector<QColor> lastTile = drawBGTileLineFromNameTable(actualReadLine % 8, actualX < 256, !isLowerNametable, tileXPos % 32, tileYPos);
        uint lastTilePixelsToDraw = 256 - totalPixelsDrawn;

        for (int x = 0; x < lastTilePixelsToDraw; x++) {
            bgLine[totalPixelsDrawn++] = lastTile[x];
        }
    }

    return bgLine;
}

/*
 * Draws an 8px wide line from a tile in the specified name table. A tile is 8x8, this draws  an 8x1 line at the line specified.
 * This function is so that one can get pixel data from the line necessary on a tile per tile basis.
 */
QVector<QColor> Ppu::drawBGTileLineFromNameTable(uint lineNumber, bool isLeftTable, bool isUpperTable, int tileXPos, int tileYPos) {
    int nameTableSelection = (isUpperTable ? 0 : 2) + (isLeftTable ? 0 : 1); //Top Left: 0, Top Right: 1, Bottom Left: 2, Bottom Right: 3
    QVector<QColor> line = QVector<QColor>(8);

    int tileCount = 256 / 8;


    //Determine which tile we are drawing
    int tileNumber = tileXPos + (tileYPos * 32);

    //Determine which name table cell the current tile is in
    int cellYPos = (tileYPos / 2);
    int cellXPos = (tileXPos / 2);
    int cellNumber = (cellYPos * 16) + cellXPos;
    bool leftTile = (tileXPos % 2) == 0;
    bool upperTile = (tileYPos % 2) == 0;
    bool leftCell = (cellXPos % 2) == 0;
    bool upperCell = (cellYPos % 2) == 0;


    //Determine the attribute byte for this cell

    //A Cell is made up of 2 rows of 2 tiles in a square.
    //One attribute byte defines the palettes for a 2x2 square of cells.
    //The bottom right cell is cell 1, bottom left is cell 2, upper right is cell 3, and upper left is cell 4.
    //The attribute data is 8 bits. 2 bits for each cell. These bits are the palette number.
    //Then we retrieve the 2 relevant bits that define the palette number for this tile.
    //The attribute value contains the palettes in the order of 0bAABBCCDD. AA is cell 4. BB is cell 3. CC is cell 2. DD is cell 1.

    int attributeRegion = ((cellYPos / 2) * 8) + (cellXPos / 2);
    unsigned char attributeValue = ppuRam[0x23C0 + (nameTableSelection * 0x400) + attributeRegion];
    unsigned char tilePalette = attributeValue;

    tilePalette = (upperCell) ? (unsigned char)(tilePalette & 0x0F) : (unsigned char)((tilePalette & 0xF0) >> 4);
    tilePalette = (!leftCell) ? (unsigned char)((tilePalette & 0b1100) >> 2) : (unsigned char)(tilePalette & 0b0011);

    //Determine the background pattern address
    ushort backgroundPatternTableAddress = getPPURegisterBackgroundPatternTableSetting() ? (ushort)(0x1000) : (ushort)(0x0000);

    //Read tileID from name table
    ushort nameTableAddress = (ushort)(0x2000 + (nameTableSelection * 0x400));
    unsigned char tileID = ppuRam[nameTableAddress + tileNumber];

    //Read current line of tile
    int yLineOffset = (int)lineNumber % 8; //This lets us know which line of the tile we are drawing, so that we can read the correct line data.
    ushort patternAddressTemp = (ushort)(backgroundPatternTableAddress + (tileID * 16) + (yLineOffset * 2));
    unsigned char tileDataRow1 = ppuRam[backgroundPatternTableAddress + (tileID * 16) + (yLineOffset)];
    unsigned char tileDataRow2 = ppuRam[backgroundPatternTableAddress + (tileID * 16) + (yLineOffset) + 8];

    QVector<unsigned char> tileColorIndices = QVector<unsigned char>(8);

    for (int i = 0; i < 8; i++) {
        //Patterns are defined by 16 bytes that detail an 8x8 pixel pattern. 2 bytes per line.
        //Each pixel can be one of four colors. 2 bytes are read in. The first bit
        //in the first byte and the first bit in the second byte define the color of the first pixel.
        tileColorIndices[i] = (unsigned char)(((tileDataRow1 & 0x01)) | (tileDataRow2 & 0x01) << 1);
        tileDataRow1 >>= 1;
        tileDataRow2 >>= 1;
    }

    // Reverse contents of tileColorIndices
    int tileColorCount = tileColorIndices.count();
    for (int i = 0; i < tileColorCount/2; i++) {
        unsigned char c = tileColorIndices[i];
        tileColorIndices[i] = tileColorIndices[(tileColorCount - 1) - i];
        tileColorIndices[(tileColorCount - 1) - i] = c;
    }

    //Draw current tile data to line
    for (int i = 0; i < 8; i++) {
        QColor pixelColor;

        if (tileColorIndices[i] != 0) {
            //Use the palette index retrieved from the attribute table to select the proper palette from 0x3F00-0x3F20
            //Select pixel color index from selected palette using tile color index retrieved from the tile data rows.
            //Use this final color index to index a color from the NES color palette.
            //unsigned char colorAddress = (unsigned char)((tilePalette * 4) + tileColorIndices[i]);
            unsigned char colorAddress = (unsigned char)((tilePalette << 2) | tileColorIndices[i]);
            unsigned char colorIndex = ppuRam[0x3F00 + colorAddress];
            pixelColor = palette[colorIndex];
        } else {
            //Read Default BG Color.
            pixelColor = palette[ppuRam[0x3F00]];
        }

        line[i] = pixelColor;
    }

    return line;
}

QVector<QColor> Ppu::drawSpriteLine(unsigned char lineNumber) {
    QVector<QColor> line = QVector<QColor>(256, QColor(0,0,0,0));

    int spriteCount = oamRam.length() / 4;

    int spritesDrawn = 0;

    for (int spriteIndex = 0; (spriteIndex < spriteCount) && (spritesDrawn < 8); spriteIndex++) {
        unsigned char spriteYPos = oamRam[spriteIndex * 4];
        unsigned char tileID     = oamRam[(spriteIndex * 4) + 1];
        unsigned char spriteXPos = oamRam[(spriteIndex * 4) + 3];
        unsigned char attributes = oamRam[(spriteIndex * 4) + 2];

        unsigned char spritePalette      = (unsigned char)(4 + (attributes & 0b11));
        unsigned char spriteHeight       = getPPURegisterSpriteSizeSetting() ? (unsigned char)16 : (unsigned char)8;
        bool isBelowBackground  = (attributes & 0b00100000) != 0;
        bool isXFlipped         = (attributes & 0b01000000) != 0;
        bool isYFlipped         = (attributes & 0b10000000) != 0;

        bool isSpriteOnLine = (spriteYPos <= lineNumber) && ((spriteYPos + (spriteHeight)) > lineNumber);

        if (isSpriteOnLine && (!isBelowBackground || spriteIndex == 0)) {
            //Read current line of sprite tile
            //int yLineOffset = lineNumber % spriteHeight; //This lets us know which line of the tile we are drawing, so that we can read the correct line data.
            int yLineOffset = lineNumber - spriteYPos;
            yLineOffset = (isYFlipped) ? ((spriteHeight - 1) - yLineOffset) : yLineOffset;

            ushort spritePatternTableAddress = 0;

            //Determine pattern table address for this sprite
            if (spriteHeight == 8) {
                //If the sprite is 8x8, then the sprite pattern table address is decided by bit 3 in the PPUCTRL register.
                spritePatternTableAddress = (ushort)(getPPURegisterSpritePatternTableSetting() ? 0x1000 : 0x0000);
            } else {
                //If the sprite is 8x16, then the sprite pattern table address is decided by bit 0 in the tileID.
                //Additionally, we mask off bit 0 so it is not factored into the sprite tile id.
                spritePatternTableAddress = (ushort)(((tileID & 0x1) != 0) ? 0x1000 : 0x0000);
                tileID &= 0b11111110;
            }

            int spriteHalfOffset = (yLineOffset > 7) ? 8 : 0; //If the sprite is 8x16 and is drawing the lower half, offset 8 bytes over
            unsigned char tileDataRow1 = ppuRam[spritePatternTableAddress + ((tileID * 16) + spriteHalfOffset + (yLineOffset))];
            unsigned char tileDataRow2 = ppuRam[spritePatternTableAddress + ((tileID * 16) + spriteHalfOffset + (yLineOffset)) + 8];

            QVector<unsigned char> tileColorIndices = QVector<unsigned char>(8);

            for (int i = 0; i < 8; i++) {
                //Patterns are defined by 16 bytes that detail an 8x8 pixel pattern. 2 bytes per line.
                //Each pixel can be one of four colors. 2 bytes are read in. The first bit
                //in the first byte and the first bit in the second byte define the color of the first pixel.
                tileColorIndices[i] = (unsigned char)(((tileDataRow1 & 0x01)) | (tileDataRow2 & 0x01) << 1);
                tileDataRow1 >>= 1;
                tileDataRow2 >>= 1;
            }

            if (!isXFlipped) {
                // Reverse contents of tileColorIndices
                int tileColorCount = tileColorIndices.count();
                for (int i = 0; i < tileColorCount/2; i++) {
                    unsigned char c = tileColorIndices[i];
                    tileColorIndices[i] = tileColorIndices[(tileColorCount - 1) - i];
                    tileColorIndices[(tileColorCount - 1) - i] = c;
                }
            }

            bool spriteHit = false;

            //Draw current tile data to line
            for (int i = 0; i < 8; i++) {
                QColor pixelColor;

                if (tileColorIndices[i] != 0) {
                    //Use the palette index retrieved from the attribute table to select the proper palette from 0x3F00-0x3F20
                    //Select pixel color index from selected palette using tile color index retrieved from the tile data rows.
                    //Use this final color index to index a color from the NES color palette.
                    //unsigned char colorAddress = (unsigned char)((tilePalette * 4) + tileColorIndices[i]);
                    unsigned char colorAddress = (unsigned char)((spritePalette << 2) | tileColorIndices[i]);
                    unsigned char colorIndex = ppuRam[0x3F00 + colorAddress];
                    pixelColor = palette[colorIndex];

                    if ((spriteXPos + i) < line.length() && !isBelowBackground)
                        line[spriteXPos + i] = pixelColor;

                    spriteHit = true;
                }
            }

            if(spriteHit) {
                ++spritesDrawn;

                if (spriteIndex == 0) {
                    bool bgEnabled = getMaskBackgroundEnabled();
                    bool cycleIs256 = TOTAL_PPU_CLOCKS == 256;

                    if (bgEnabled && !cycleIs256) {
                        spriteZeroHit = true;
                    }
                }
            }
        }
    }

    return line;
}
