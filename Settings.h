#ifndef SETTINGS_H
#define SETTINGS_H

struct Settings
{
    float ripple_amplitude;
    float ripple_speed;
    float ripple_energy;
    bool show_boat;
};

extern Settings settings;

#endif // SETTINGS_H
