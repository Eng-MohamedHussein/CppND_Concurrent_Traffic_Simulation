#include <iostream>
#include <random>
#include "TrafficLight.h"
//enum  class TrafficlightPhase {red,green};

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    
    std::unique_lock<std::mutex> underTheLock(_mutex);
    _cond.wait(underTheLock,[this]{return !_message.empty();});
    T message = std::move(_message.back());
    _message.pop_back();
    return message;   
}

template <typename T>
void MessageQueue<T>::send(T &&message)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> underTheLock(_mutex);
    _message.push_back(std::move(message));
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficlightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true)
    {
        //TrafficlightPhase check= TrafficLight::phase.receive();
        if(phase.receive()== TrafficlightPhase::green)
        {
            return;
        }

    }
}

TrafficlightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    
    TrafficObject::threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    t1= std::chrono::high_resolution_clock::now();
    double Phaseduration = 4;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        t2= std::chrono::high_resolution_clock::now();
        std::random_device rnd;
        std::uniform_int_distribution<int> duration(4,6);
        double Phaseduration = duration(rnd);


        auto cycleDuration = std::chrono::duration_cast<std::chrono::seconds>(t2-t1).count();
        if (cycleDuration>= Phaseduration)
        {
            if(_currentPhase==TrafficlightPhase::red)
            {
                _currentPhase=TrafficlightPhase::green;
            }
            else
            {
                _currentPhase=TrafficlightPhase::red;
            }
            t1= std::chrono::high_resolution_clock::now();
            //need to update the message, which i still don't have yet
            phase.send(std::move(_currentPhase));
        }
    }
    
}

