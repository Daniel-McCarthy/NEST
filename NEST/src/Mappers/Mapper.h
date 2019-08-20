#ifndef MAPPER_H
#define MAPPER_H

#include <QObject>

class Rom;
class Mapper : public QObject
{
    Q_OBJECT
public:
    explicit Mapper(QObject *parent, Rom& rom);
    bool isMapperWriteAddress(ushort address);
    void writeToCurrentMapper(ushort address, unsigned char value);

private:
    Rom& rom;

signals:

public slots:
};

#endif // MAPPER_H
