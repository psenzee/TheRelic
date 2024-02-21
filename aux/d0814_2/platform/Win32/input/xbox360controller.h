#ifndef _XBOX360CONTROLLER_H
#define _XBOX360CONTROLLER_H

class Xbox360Controller
{
public:
    Xbox360Controller();

    void poll();
    bool active() const;
    void get(float &x, float &y, float &rx, float &ry, float &ltrigger, float &rtrigger);
    bool isButtonDown(int button) const;
};

#endif  // _XBOX360CONTROLLER_H