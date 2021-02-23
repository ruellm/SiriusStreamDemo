#ifndef IAUDIOPROCESSOR_H
#define IAUDIOPROCESSOR_H

class IAudioProcessor {
public:
    virtual bool ProcessAudio(const void* buffer, int len) = 0;
};

#endif // IAUDIOPROCESSOR_H
