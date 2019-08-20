#ifndef MAPPER_H
#define MAPPER_H

#include <QObject>

class Rom;
class MMC1;
class UNROM;
class CNROM;
class Mapper : public QObject
{
    Q_OBJECT
public:
    explicit Mapper(QObject *parent, Rom& rom, MMC1& mmc1, UNROM& unrom, CNROM& cnrom);
    bool isMapperWriteAddress(ushort address);
    void writeToCurrentMapper(ushort address, unsigned char value);

private:
    Rom& rom;
    MMC1& mmc1;
    UNROM& unrom;
    CNROM& cnrom;

signals:

public slots:
};

#endif // MAPPER_H
