#include <iostream>
#include <mutex>
#include <thread>

std::mutex railwayStationAccess;

class Train {
    std::string name;
    int duration;
public:
    Train(std::string inName) : name(inName), duration(0) {}

    std::string getName() {
        return name;
    }

    void setDuration(int inDuration) {
        if (inDuration > 0) {
            duration = inDuration;
        } else {
            std::cout << "Wrong duration!" << std::endl;
        }
    }

    int getDuration() {
        return duration;
    }

};

class RailwayStation {
    Train *train;
public:
    RailwayStation() : train(nullptr) {}

    void arrived(Train *inTrain) {
        std::cout << "Train #" << inTrain->getName() << "  has arrived" << std::endl;

        if (train == nullptr) {
            railwayStationAccess.lock();
            train = inTrain;
            railwayStationAccess.unlock();
        } else {
            std::cout << "train #" << inTrain->getName() << "  is waiting";
        }

    }

    void depart(Train *inTrain) {
        railwayStationAccess.lock();
        if (train == nullptr) {
            train = inTrain;
        }
        std::cout << "Train #" << train->getName() << "  at the station now" << std::endl;
        std::string command;
        do {
            std::cout << "Input command (depart): ";
            std::cin >> command;
        } while (command != "depart");

        std::cout << "Train #" << train->getName() << "  has depart" << std::endl << std::endl;
        train = nullptr;
        railwayStationAccess.unlock();

    }

};

void go(RailwayStation *railwayStation, Train *train) {
    std::this_thread::sleep_for(std::chrono::seconds(train->getDuration()));

    railwayStation->arrived(train);

    railwayStation->depart(train);

}

int main() {
    Train *trains[3] = {new Train("A"),
                        new Train("B"),
                        new Train("C")};
    RailwayStation *railwayStation = new RailwayStation;
    std::thread threads[3];

    for (int i = 0; i < 3; ++i) {
        std::cout << "Train #" << trains[i]->getName() << ". Input duration to the station (sec):";
        int timeSec;
        std::cin >> timeSec;
        trains[i]->setDuration(timeSec);
    }

    for (int i = 0; i < 3; ++i) {
        threads[i] = std::thread(go, railwayStation, trains[i]);
    }

    for (int i = 0; i < 3; ++i) {
        threads[i].join();
    }
    delete railwayStation;
    railwayStation = nullptr;

    for (int i = 0; i < 3; ++i) {
        delete trains[i];
        trains[i] = nullptr;
    }

    return 0;
}
