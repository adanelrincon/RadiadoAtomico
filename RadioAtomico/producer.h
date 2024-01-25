#ifndef PRODUCER_H
#define PRODUCER_H

#include "QtCore/qsemaphore.h"
#include <QThread>
#include <QSemaphore>

class Producer : public QThread
{
public:
    explicit Producer(QObject *parent = nullptr);
    explicit Producer(const int DataSize,
                      const int BufferSize,
                      char *buffer,
                      QSemaphore *freeBytes,
                      QSemaphore *usedBytes,
                      QSemaphore *consoleAvailable,
                      QSemaphore *consumersReadCount
                      );
    int threadId;  // Identificador de hilo

private:
    int DataSize;
    int BufferSize;
    char *buffer;
    QSemaphore *freeBytes;
    QSemaphore *usedBytes;
    QSemaphore *consoleAvailable;
    QSemaphore *consumersReadCount;

protected:
    void run();
};

#endif // PRODUCER_H
