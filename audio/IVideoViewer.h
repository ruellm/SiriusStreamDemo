#ifndef IVIDEOVIEWER_H
#define IVIDEOVIEWER_H
#include "backend/backend.h"

class IVideoVidewer {
public:
    virtual void ResetTime(backend::TimePoint ts) = 0;
};
#endif // IVIDEOVIEWER_H
