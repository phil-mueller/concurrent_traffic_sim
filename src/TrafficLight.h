#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

// Generic message queue class
template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
};

enum TrafficLightPhase
{
    red,
    green,
};

class TrafficLight : public TrafficObject, public std::enable_shared_from_this<TrafficLight>
{
public:
    // constructor / destructor
    TrafficLight();
    // No destructor, because the default destructor of the parent class contains the thread barrier

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _messageQueue;
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
};

#endif