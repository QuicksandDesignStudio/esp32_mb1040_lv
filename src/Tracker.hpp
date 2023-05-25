// +----------------------------------------------------------------------------------------------------------------------------
//
// SIMPLE TIMER - (c) 2021 Romit Raj. 
//
// File : Tracker.hpp
//
// Description
//
//  A simple timer that returns true when the timer is complete and resets the timer.  
//
// History:     Aug-28-2021     romitraj        Created
//
//----------------------------------------------------------------------------------------------------------------------------

#ifndef Tracker_hpp
#define Tracker_hpp

#include <sys/time.h>

class Tracker
{
private:
    // Black box function from Mr. Dave Plumber himself returns time of day
    static double Time()
    {
        timeval tv = {0};
        gettimeofday(&tv, nullptr);
        return (double)(tv.tv_usec / 1000000.0 + (double) tv.tv_sec);
    }
    
    bool            m_enabled{};                    // Is it the timer enabled
    double          m_startTime{Time()};            // Timekeeper
    double          m_loopTime;                     // Loop time 

public:
    //Constructor that does nothing
    Tracker()
    {
        
    }
    //Constructor that takes in the loop time and enables the timer
    Tracker(double loopTime)
        :m_loopTime{loopTime}
    {
        m_enabled = true;
    }

    // This has to called every frame. It will return true if loop time has elapsed
    bool loop()
    {
        if(m_enabled)
        {   
            double elapsedTime = Time() - m_startTime;
            if(elapsedTime >= m_loopTime)
            {
                m_startTime = Time();
                return true;
            }
        }
        return false;
    }

    // Method to enable the tracker and reset the timekeeper
    // Also Separate Initializer
    void enableTracker()
    {
        m_enabled   = true;
        m_startTime = Time();
    }

    void enableTracker(double loopTime)
    {
        m_loopTime = loopTime;
        enableTracker();
    }
    
    // Method to disable the tracker
    void disableTracker()
    {
        m_enabled = false;
    }
};

#endif