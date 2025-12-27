#pragma once

#include <cmath>
#include <cstdint>
#include <array>
#include <algorithm>
#include <random>

namespace jerv::core {
    class PerlinNoise {
    public:
        PerlinNoise(const uint64_t seed = 0) {
            for (int i = 0; i < 256; i++) {
                perm_[i] = i;
            }

            std::mt19937_64 rng(seed);
            std::shuffle(perm_.begin(), perm_.begin() + 256, rng);

            for (int i = 0; i < 256; i++) {
                perm_[256 + i] = perm_[i];
            }
        }

        double noise2D(double x, double y) const {
            int xi = static_cast<int>(std::floor(x)) & 255;
            int yi = static_cast<int>(std::floor(y)) & 255;

            double xf = x - std::floor(x);
            double yf = y - std::floor(y);

            double u = fade(xf);
            double v = fade(yf);

            int aa = perm_[perm_[xi] + yi];
            int ab = perm_[perm_[xi] + yi + 1];
            int ba = perm_[perm_[xi + 1] + yi];
            int bb = perm_[perm_[xi + 1] + yi + 1];

            double x1 = lerp(grad2D(aa, xf, yf), grad2D(ba, xf - 1, yf), u);
            double x2 = lerp(grad2D(ab, xf, yf - 1), grad2D(bb, xf - 1, yf - 1), u);

            return lerp(x1, x2, v);
        }

        double noise3D(double x, double y, double z) const {
            int xi = static_cast<int>(std::floor(x)) & 255;
            int yi = static_cast<int>(std::floor(y)) & 255;
            int zi = static_cast<int>(std::floor(z)) & 255;

            double xf = x - std::floor(x);
            double yf = y - std::floor(y);
            double zf = z - std::floor(z);

            double u = fade(xf);
            double v = fade(yf);
            double w = fade(zf);

            int aaa = perm_[perm_[perm_[xi] + yi] + zi];
            int aba = perm_[perm_[perm_[xi] + yi + 1] + zi];
            int aab = perm_[perm_[perm_[xi] + yi] + zi + 1];
            int abb = perm_[perm_[perm_[xi] + yi + 1] + zi + 1];
            int baa = perm_[perm_[perm_[xi + 1] + yi] + zi];
            int bba = perm_[perm_[perm_[xi + 1] + yi + 1] + zi];
            int bab = perm_[perm_[perm_[xi + 1] + yi] + zi + 1];
            int bbb = perm_[perm_[perm_[xi + 1] + yi + 1] + zi + 1];

            double x1 = lerp(grad3D(aaa, xf, yf, zf), grad3D(baa, xf - 1, yf, zf), u);
            double x2 = lerp(grad3D(aba, xf, yf - 1, zf), grad3D(bba, xf - 1, yf - 1, zf), u);
            double y1 = lerp(x1, x2, v);

            x1 = lerp(grad3D(aab, xf, yf, zf - 1), grad3D(bab, xf - 1, yf, zf - 1), u);
            x2 = lerp(grad3D(abb, xf, yf - 1, zf - 1), grad3D(bbb, xf - 1, yf - 1, zf - 1), u);
            double y2 = lerp(x1, x2, v);

            return lerp(y1, y2, w);
        }

        double fbm2D(double x, double y, int octaves = 6, double persistence = 0.5, double lacunarity = 2.0) const {
            double total = 0.0;
            double amplitude = 1.0;
            double frequency = 1.0;
            double maxValue = 0.0;

            for (int i = 0; i < octaves; i++) {
                total += amplitude * noise2D(x * frequency, y * frequency);
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= lacunarity;
            }

            return total / maxValue;
        }

        double ridgeNoise2D(double x, double y, int octaves = 6, double persistence = 0.5,
                            double lacunarity = 2.0) const {
            double total = 0.0;
            double amplitude = 1.0;
            double frequency = 1.0;
            double maxValue = 0.0;

            for (int i = 0; i < octaves; i++) {
                double n = noise2D(x * frequency, y * frequency);
                n = 1.0 - std::abs(n);
                n = n * n;
                total += amplitude * n;
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= lacunarity;
            }

            return total / maxValue;
        }
    private:
        std::array<int, 512> perm_;

        static double fade(double t) {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        static double lerp(double a, double b, double t) {
            return a + t * (b - a);
        }

        static double grad2D(int hash, double x, double y) {
            int h = hash & 3;
            double u = h < 2 ? x : y;
            double v = h < 2 ? y : x;
            return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
        }

        static double grad3D(int hash, double x, double y, double z) {
            int h = hash & 15;
            double u = h < 8 ? x : y;
            double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
            return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
        }
    };
}