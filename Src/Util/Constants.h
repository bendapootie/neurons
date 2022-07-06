#pragma  once

constexpr float k_pi = 3.14159265358979323846f;
constexpr float k_2pi = k_pi * 2.0f;
constexpr float k_piOver180 = k_pi / 180.0f;
constexpr float k_180OverPi = 180.0f / k_pi;

constexpr float RadToDeg(float rad) { return rad * k_180OverPi; }
constexpr float DegToRad(float deg) { return deg * k_piOver180; }
