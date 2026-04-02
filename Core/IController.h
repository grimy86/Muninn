#pragma once

class IController
{
protected:
    // Dispose fallback
    virtual bool Dispose() noexcept
    {
        return true; 
    }

public:
    IController() noexcept = default;
    virtual ~IController() noexcept
    {
        Dispose();
    }

    // Deleted copy/assignment
    IController(const IController&) = delete;
    IController& operator=(const IController&) = delete;
};