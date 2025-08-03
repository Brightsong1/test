#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <fstream>
#include <cstdint>

struct WavHeader 
{
    int8_t chunkId[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize;
    int8_t format[4] = {'W', 'A', 'V', 'E'};
    int8_t fmtChunkId[4] = {'f', 'm', 't', ' '};
    uint32_t fmtChunkSize = 16;
    uint16_t audioFormat = 1;
    uint16_t numChannels = 1;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign = 2;
    uint16_t bitsPerSample = 16;
    int8_t dataChunkId[4] = {'d', 'a', 't', 'a'};
    uint32_t dataChunkSize;
};

std::vector<int16_t> karplusStrong(double frequency, double sampleRate, double duration, double decay, double noiseLevel) 
{
    int32_t sampleCount = static_cast<int32_t>(sampleRate * duration);
    std::vector<int16_t> signal(sampleCount);
    int32_t bufferSize = static_cast<int32_t>(sampleRate / frequency);
    std::vector<double> buffer(bufferSize);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-noiseLevel, noiseLevel);

    for (int32_t i = 0; i < bufferSize; ++i) 
    {
        buffer[i] = dis(gen);
    }

    int32_t bufferIndex = 0;
    double maxAmplitude = 0.0;
    for (int32_t i = 0; i < sampleCount; ++i) 
    {
        double output = decay * (buffer[bufferIndex] + buffer[(bufferIndex + 1) % bufferSize]) / 2.0;
        buffer[bufferIndex] = output;
        bufferIndex = (bufferIndex + 1) % bufferSize;
        signal[i] = static_cast<int16_t>(output * 32767.0);
        maxAmplitude = std::max(maxAmplitude, std::abs(output));
    }

    if (maxAmplitude > 0.0) 
    {
        for (int32_t i = 0; i < sampleCount; ++i) 
        {
            signal[i] = static_cast<int16_t>(static_cast<double>(signal[i]) * 0.9 / maxAmplitude);
        }
    }
    return signal;
}

std::vector<int16_t> synthesizeMultipleFrequencies(const std::vector<double>& frequencies, double sampleRate, double duration, double decay, double noiseLevel) 
{
    int32_t sampleCount = static_cast<int32_t>(sampleRate * duration);
    std::vector<int16_t> signal(sampleCount, 0);
    for (double freq : frequencies) 
    {
        auto partialSignal = karplusStrong(freq, sampleRate, duration, decay, noiseLevel);
        for (int32_t i = 0; i < sampleCount; ++i) 
        {
            int32_t sum = static_cast<int32_t>(signal[i]) + static_cast<int32_t>(partialSignal[i]);
            signal[i] = static_cast<int16_t>(std::min(std::max(sum, -32768), 32767));
        }
    }

    return signal;
}

void writeWavFile(const std::vector<int16_t>& signal, double sampleRate, const std::string& filename) 
{
    WavHeader header;
    header.sampleRate = static_cast<uint32_t>(sampleRate);
    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.dataChunkSize = signal.size() * header.numChannels * header.bitsPerSample / 8;
    header.chunkSize = 36 + header.dataChunkSize;

    std::ofstream outFile(filename, std::ios::binary);
    outFile.write(reinterpret_cast<char*>(&header), sizeof(WavHeader));
    outFile.write(reinterpret_cast<const char*>(signal.data()), signal.size() * sizeof(int16_t));
    outFile.close();
}

int main() 
{
    std::vector<double> frequencies = {110.0, 220.0, 440.0};
    double sampleRate = 44100.0;
    double duration = 2.0;
    double decay = 0.996;
    double noiseLevel = 0.5;
    
    auto signal = synthesizeMultipleFrequencies(frequencies, sampleRate, duration, decay, noiseLevel);
    writeWavFile(signal, sampleRate, "output.wav");

    for (int32_t i = 0; i < 10; ++i) 
    {
        std::cout << signal[i] << std::endl;
    }
    return 0;
}