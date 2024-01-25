#include "producer.h"
#include <QRandomGenerator>
#include <iostream>

Producer::Producer(QObject *parent)
    : QThread{parent}
{}

Producer::Producer(const int DataSize,
                   const int BufferSize,
                   char *buffer,
                   QSemaphore *freeBytes,
                   QSemaphore *usedBytes,
                   QSemaphore *consoleAvailable,
                   QSemaphore *consumersReadCount
                   )
{
    this->DataSize = DataSize;
    this->BufferSize = BufferSize;
    this->buffer = buffer;
    this->freeBytes = freeBytes;
    this->usedBytes = usedBytes;
    this->consoleAvailable = consoleAvailable;
    this->consumersReadCount = consumersReadCount;
}

void Producer::run()
{
    for (int i = 0; i < DataSize; ++i) {
        // Espera hasta que todos los consumidores hayan leído el elemento
        consumersReadCount->acquire();

        buffer[i % BufferSize] = "ACGT"[QRandomGenerator::global()->bounded(4)];

        consoleAvailable->acquire();
        std::cout << "Producer" << threadId << " inserting..." << buffer[i % BufferSize] << '\t' << i % BufferSize << '\t' << buffer << "\n";
        consoleAvailable->release();

        usedBytes->release();
    }
}
