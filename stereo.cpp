#include <iostream>
#include <vector>
#include <cmath>

double calculateDistanceLeft(double radius, double earDistance, double theta) 
{
    return std::sqrt(radius * radius + earDistance * earDistance / 4.0 - radius * earDistance * std::cos(theta));
}

double calculateDistanceRight(double radius, double earDistance, double theta) 
{
    return std::sqrt(radius * radius + earDistance * earDistance / 4.0 + radius * earDistance * std::cos(theta));
}

double calculateItd(double distanceLeft, double distanceRight, double soundSpeed) 
{
    return (distanceRight - distanceLeft) / soundSpeed;
}

std::vector<std::vector<double>> generateStereoSignal(double radius, double angularVelocity, double earDistance,
                                                     double soundSpeed, double duration, double sampleRate,
                                                     double frequency) 
{
    std::vector<std::vector<double>> signal(2);
    int sampleCount = static_cast<int>(duration * sampleRate);
    signal[0].resize(sampleCount);
    signal[1].resize(sampleCount);
    for (int i = 0; i < sampleCount; ++i) 
    {
        double time = i / sampleRate;
        double theta = angularVelocity * time;
        double distanceLeft = calculateDistanceLeft(radius, earDistance, theta);
        double distanceRight = calculateDistanceRight(radius, earDistance, theta);
        double itd = calculateItd(distanceLeft, distanceRight, soundSpeed);
        signal[0][i] = std::sin(2.0 * M_PI * frequency * time);
        double rightTime = time + itd;
        signal[1][i] = std::sin(2.0 * M_PI * frequency * rightTime);
    }
    return signal;
}

int main() 
{
    double radius = 1.0;
    double angularVelocity = 2.0 * M_PI;
    double earDistance = 0.204;
    double soundSpeed = 340.29;
    double duration = 1.0;
    double sampleRate = 44100.0;
    double frequency = 440.0;
    auto signal = generateStereoSignal(radius, angularVelocity, earDistance, soundSpeed, duration, sampleRate, frequency);
    for (int i = 0; i < 10; ++i) 
    {
        std::cout << signal[0][i] << " " << signal[1][i] << std::endl;
    }
    return 0;
}