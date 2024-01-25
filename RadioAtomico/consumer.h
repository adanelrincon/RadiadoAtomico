#ifndef CONSUMER_H
#define CONSUMER_H

#include "QtCore/qsemaphore.h"
#include <QThread>
#include <QSemaphore>

class Consumer : public QThread
{
public:
    explicit Consumer(QObject *parent = nullptr);
    explicit Consumer(const int DataSize,
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

#endif // CONSUMER_H
