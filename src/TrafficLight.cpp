#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{ 
    // Use lock to protect vector of messages while reading and modifying
    std::unique_lock<std::mutex> lck(_mutex);
    // Wait until data becomes available
    _cond.wait(lck,[this] {return !_queue.empty(); });
    // Retrieve message and delete it from queue
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // Use lock_guard to protect vector of messages while writing to it
    std::lock_guard<std::mutex> lck(_mutex);
    // Push message into queue and notify client
    _queue.clear();
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // Query the message queue until the traffic light becomes green
    while(true)
    {
        _currentPhase = _messageQueue.receive();
        if(_currentPhase==TrafficLightPhase::green)
        {
            break;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // Start member function cycleThroughPhases in a thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // Set first time-point and toggle time between 4 and 6 seconds
    auto start = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds;
    auto end = std::chrono::steady_clock::now();
    int toggleTime = std::rand() % 2 + 4;
    // Start infinite loop
    while(true)
    {
        // Burn some time
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // Get elapsed time
        end = std::chrono::steady_clock::now();
        elapsed_seconds = end-start;
        // Check if elapsed time is greater than toggle time
        if(elapsed_seconds >= std::chrono::duration<double>(static_cast<double>(toggleTime)))
        {
            // Toggle the traffic light phase
            if(_currentPhase==TrafficLightPhase::green)
            {
                _currentPhase = TrafficLightPhase::red;
            }
            else
            {
                _currentPhase = TrafficLightPhase::green;
            }
            // Send update mathod to message queue
            _messageQueue.send(std::move(_currentPhase));
            // Reset timer
            start = std::chrono::steady_clock::now();
            // Calculate new random toggle time
            toggleTime = std::rand() % 2 + 4;
        }
    } 
}