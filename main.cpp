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
    Train *trainAtStation;
public:
    RailwayStation() : trainAtStation(nullptr) {}

    void arrived(Train *inTrain) {
        railwayStationAccess.lock();
        std::cout << "Train #" << inTrain->getName() << "  has arrived" << std::endl;
        if (trainAtStation == nullptr) {
            trainAtStation = inTrain;
        } else {
            std::cout << "train #" << inTrain->getName() << "  is waiting";
        }
        railwayStationAccess.unlock();
    }

    void depart() {
        railwayStationAccess.lock();
        std::cout << "Train #" << trainAtStation->getName() << "  at the station now" << std::endl;
        std::string command;
        do {
            std::cout << "Input command (depart): ";
            std::cin >> command;
        } while (command != "depart");

        std::cout << "Train #" << trainAtStation->getName() << "  has depart" << std::endl << std::endl;
        trainAtStation = nullptr;
        railwayStationAccess.unlock();

    }

};

void go(RailwayStation *railwayStation, Train *train) {
    std::this_thread::sleep_for(std::chrono::seconds(train->getDuration()));

    railwayStation->arrived(train);

    railwayStation->depart();

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
