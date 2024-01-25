#include <QtCore/QSemaphore>
#include <QThread>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>

class Consumer : public QThread {
public:
    explicit Consumer(int id, int dataSize, char* buffer, QSemaphore* consoleAvailable, QSemaphore* allConsumersReady, std::map<char, int>& readCount);
    void run() override;

private:
    int id;
    int dataSize;
    char* buffer;
    QSemaphore* consoleAvailable;
    QSemaphore* allConsumersReady;
    std::map<char, int>& readCount;
};

Consumer::Consumer(int id, int dataSize, char* buffer, QSemaphore* consoleAvailable, QSemaphore* allConsumersReady, std::map<char, int>& readCount)
    : id(id), dataSize(dataSize), buffer(buffer), consoleAvailable(consoleAvailable), allConsumersReady(allConsumersReady), readCount(readCount) {}

void Consumer::run() {
    char tempBuffer[dataSize + 1]; // Copia del buffer

    allConsumersReady->acquire(); // Espera a que todos los consumidores estén listos

    consoleAvailable->acquire();
    std::cout << "Consumer " << id << " ready to start extracting...\n";
    consoleAvailable->release();

    for (int i = 0; i < dataSize; ++i) {
        consoleAvailable->acquire();
        char value = buffer[i];
        std::cout << "Consumer " << id << " extracting... " << value;
        std::strncpy(tempBuffer, buffer, dataSize);
        tempBuffer[dataSize] = '\0';
        std::cout << "\tBuffer: " << tempBuffer << "\n";
        consoleAvailable->release();

        // Marcar el elemento como leído por este consumidor
        --readCount[value];

        // Esperar hasta que todos los consumidores hayan leído este elemento
        while (readCount[value] > 0) {
            QThread::msleep(10); // Espera corta antes de volver a verificar
        }

        // Cambiar el valor del elemento a "-"
        buffer[i] = '-';
    }
}

class Producer : public QThread {
public:
    explicit Producer(int dataSize, char* buffer, QSemaphore* consoleAvailable, QSemaphore* allConsumersReady, std::map<char, int>& readCount);
    void run() override;

private:
    int dataSize;
    char* buffer;
    QSemaphore* consoleAvailable;
    QSemaphore* allConsumersReady;
    std::map<char, int>& readCount;
};

Producer::Producer(int dataSize, char* buffer, QSemaphore* consoleAvailable, QSemaphore* allConsumersReady, std::map<char, int>& readCount)
    : dataSize(dataSize), buffer(buffer), consoleAvailable(consoleAvailable), allConsumersReady(allConsumersReady), readCount(readCount) {}

void Producer::run() {
    char tempBuffer[dataSize + 1]; // Copia del buffer

    consoleAvailable->acquire();
    for (int i = 0; i < dataSize; ++i) {
        buffer[i] = "ACGT"[rand() % 4]; // Genera un carácter aleatorio
        char value = buffer[i];
        std::strncpy(tempBuffer, buffer, dataSize);
        tempBuffer[dataSize] = '\0';
        std::cout << "Producer inserting... " << value;
        std::cout << "\tBuffer: " << tempBuffer << "\n";

        // Inicializar el conteo de lecturas para este elemento
        readCount[value] = 5;
    }
    consoleAvailable->release();

    allConsumersReady->release(dataSize); // Marca todos los consumidores como listos
}

int main() {
    const int dataSize = 100; // Cambio del tamaño del buffer
    char buffer[dataSize];
    QSemaphore consoleAvailable(1); // Semaphore for console access
    QSemaphore allConsumersReady(0); // Semaphore to synchronize consumers

    const int numConsumers = 5;
    std::vector<Consumer*> consumers;
    std::map<char, int> readCount; // Contador de lecturas para cada elemento del buffer

    for (int i = 0; i < numConsumers; ++i) {
        consumers.push_back(new Consumer(i, dataSize, buffer, &consoleAvailable, &allConsumersReady, readCount));
        consumers.back()->start();
    }

    Producer producer(dataSize, buffer, &consoleAvailable, &allConsumersReady, readCount);
    producer.start();

    producer.wait();
    for (auto* consumer : consumers) {
        consumer->wait();
        delete consumer;
    }

    return 0;
}
