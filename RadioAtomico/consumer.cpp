#include "consumer.h"
#include <iostream>

Consumer::Consumer(QObject *parent)
    : QThread{parent}
{}

Consumer::Consumer(const int DataSize,
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

void Consumer::run()
{
    for (int i = 0; i < DataSize; ++i) {
        usedBytes->acquire();
        const char auxCharacter = buffer[i % BufferSize];

        consoleAvailable->acquire();
        std::cout << "Consumer" << threadId << " extracting..." << auxCharacter << '\t' << i % BufferSize << '\t' << buffer << "\n";
        consoleAvailable->release();

        // Incrementa el contador de consumidores que han leído el elemento
        consumersReadCount->release();
    }
}
