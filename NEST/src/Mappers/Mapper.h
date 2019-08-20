#ifndef MAPPER_H
#define MAPPER_H

#include <QObject>

class Rom;
class MMC1;
class Mapper : public QObject
{
    Q_OBJECT
public:
    explicit Mapper(QObject *parent, Rom& rom, MMC1& mmc1);
    bool isMapperWriteAddress(ushort address);
    void writeToCurrentMapper(ushort address, unsigned char value);

private:
    Rom& rom;
    MMC1& mmc1;

signals:

public slots:
};

#endif // MAPPER_H
