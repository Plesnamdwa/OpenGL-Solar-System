#include <GLFW/glfw3.h>
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>
#include <cctype>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

const int WINDOW_WIDTH = 1050;
const int WINDOW_HEIGHT = 800;

// Enum Efek Suara Prosedural
enum SoundEffect {
    SFX_HOVER = 0,
    SFX_CLICK,
    SFX_BACK,
    SFX_SPEED_UP,
    SFX_SLOW_DOWN,
    SFX_PAUSE,
    SFX_RESUME,
    SFX_COUNT
};

std::vector<std::vector<uint8_t>> sfxBuffers;

// Helper untuk membuat file WAV dalam memori
void generateWAV(std::vector<uint8_t>& outWav, const std::vector<int16_t>& samples, int sampleRate = 22050) {
    uint32_t dataSize = (uint32_t)(samples.size() * sizeof(int16_t));
    uint32_t totalSize = 36 + dataSize;

    outWav.resize(44 + dataSize);
    uint8_t* p = outWav.data();

    // RIFF header
    memcpy(p, "RIFF", 4);
    memcpy(p + 4, &totalSize, 4);
    memcpy(p + 8, "WAVE", 4);

    // fmt chunk
    memcpy(p + 12, "fmt ", 4);
    uint32_t fmtSize = 16;
    uint16_t format = 1; // PCM
    uint16_t channels = 1; // Mono
    uint32_t sRate = sampleRate;
    uint32_t byteRate = sRate * 2;
    uint16_t blockAlign = 2;
    uint16_t bitsPerSample = 16;

    memcpy(p + 16, &fmtSize, 4);
    memcpy(p + 20, &format, 2);
    memcpy(p + 22, &channels, 2);
    memcpy(p + 24, &sRate, 4);
    memcpy(p + 28, &byteRate, 4);
    memcpy(p + 32, &blockAlign, 2);
    memcpy(p + 34, &bitsPerSample, 2);

    // data chunk
    memcpy(p + 36, "data", 4);
    memcpy(p + 40, &dataSize, 4);
    memcpy(p + 44, samples.data(), dataSize);
}

// Inisialisasi Synthesizer Audio Prosedural Sci-Fi
void initAudioSynthesizer() {
    int sampleRate = 22050;
    sfxBuffers.resize(SFX_COUNT);

    // 1. SFX_HOVER: Soft high-pitch futuristic blip (45ms)
    {
        int numSamples = (int)(0.045f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = expf(-t * 70.0f);
            float freq = 950.0f + 450.0f * (t / 0.045f);
            float s = sinf(2.0f * M_PI * freq * t) * env;
            samples[i] = (int16_t)(s * 10000.0f);
        }
        generateWAV(sfxBuffers[SFX_HOVER], samples, sampleRate);
    }

    // 2. SFX_CLICK: Sci-fi whoosh + positive chord warp (220ms)
    {
        int numSamples = (int)(0.22f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = (t < 0.03f) ? (t / 0.03f) : expf(-(t - 0.03f) * 15.0f);
            float f1 = 523.25f + 350.0f * (1.0f - expf(-t * 22.0f));
            float f2 = 783.99f + 250.0f * (1.0f - expf(-t * 22.0f));
            float noise = ((float)rand() / RAND_MAX - 0.5f) * expf(-t * 25.0f);
            float s = (0.55f * sinf(2.0f * M_PI * f1 * t) + 0.35f * sinf(2.0f * M_PI * f2 * t) + 0.15f * noise) * env;
            samples[i] = (int16_t)(s * 20000.0f);
        }
        generateWAV(sfxBuffers[SFX_CLICK], samples, sampleRate);
    }

    // 3. SFX_BACK: Descending soft transition chime (200ms)
    {
        int numSamples = (int)(0.20f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = expf(-t * 13.0f);
            float freq = 784.0f - 380.0f * (t / 0.20f);
            float s = (0.65f * sinf(2.0f * M_PI * freq * t) + 0.35f * sinf(2.0f * M_PI * (freq * 0.5f) * t)) * env;
            samples[i] = (int16_t)(s * 18000.0f);
        }
        generateWAV(sfxBuffers[SFX_BACK], samples, sampleRate);
    }

    // 4. SFX_SPEED_UP: Ascending acceleration chirp (120ms)
    {
        int numSamples = (int)(0.12f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = (t < 0.02f) ? (t / 0.02f) : expf(-(t - 0.02f) * 22.0f);
            float freq = 420.0f + 1150.0f * (t / 0.12f);
            float s = sinf(2.0f * M_PI * freq * t) * env;
            samples[i] = (int16_t)(s * 16000.0f);
        }
        generateWAV(sfxBuffers[SFX_SPEED_UP], samples, sampleRate);
    }

    // 5. SFX_SLOW_DOWN: Descending deceleration chirp (120ms)
    {
        int numSamples = (int)(0.12f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = (t < 0.02f) ? (t / 0.02f) : expf(-(t - 0.02f) * 22.0f);
            float freq = 1250.0f - 800.0f * (t / 0.12f);
            float s = sinf(2.0f * M_PI * freq * t) * env;
            samples[i] = (int16_t)(s * 16000.0f);
        }
        generateWAV(sfxBuffers[SFX_SLOW_DOWN], samples, sampleRate);
    }

    // 6. SFX_PAUSE: Sci-Fi Time Stop / Power Down (260ms)
    {
        int numSamples = (int)(0.26f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = expf(-t * 11.0f);
            float freq = 650.0f * expf(-t * 9.0f);
            float ring = sinf(2.0f * M_PI * 45.0f * t);
            float s = sinf(2.0f * M_PI * freq * t) * (0.6f + 0.4f * ring) * env;
            samples[i] = (int16_t)(s * 22000.0f);
        }
        generateWAV(sfxBuffers[SFX_PAUSE], samples, sampleRate);
    }

    // 7. SFX_RESUME: Sci-Fi Time Resume / Power Up (230ms)
    {
        int numSamples = (int)(0.23f * sampleRate);
        std::vector<int16_t> samples(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            float t = (float)i / sampleRate;
            float env = expf(-t * 10.0f);
            float freq = 220.0f + 550.0f * (1.0f - expf(-t * 16.0f));
            float s = (0.7f * sinf(2.0f * M_PI * freq * t) + 0.3f * sinf(2.0f * M_PI * (freq * 1.5f) * t)) * env;
            samples[i] = (int16_t)(s * 20000.0f);
        }
        generateWAV(sfxBuffers[SFX_RESUME], samples, sampleRate);
    }
}

// Memutar Efek Suara Secara Asinkron (Non-Blocking)
void playSFX(SoundEffect sfx) {
    if (sfx >= 0 && sfx < (int)sfxBuffers.size() && !sfxBuffers[sfx].empty()) {
        PlaySoundA((LPCSTR)sfxBuffers[sfx].data(), NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    }
}

// Struktur data informasi planet
struct PlanetInfo {
    std::string type;
    std::string diameter;
    std::string distance;
    std::string orbitPeriod;
    std::string dayLength;
    std::string moons;
    std::string avgTemp;
    std::string factLine1;
    std::string factLine2;
};

// Struktur data untuk Planet
struct Planet {
    std::string nameEng; // Nama dalam Bahasa Inggris
    float distance;      // Jarak orbit dari matahari
    float radius;        // Ukuran jari-jari bola
    float orbitSpeed;    // Kecepatan revolusi
    float rotSpeed;      // Kecepatan rotasi
    float axialTilt;     // Kemiringan sumbu rotasi (derajat)
    bool hasRing;        // Apakah memiliki cincin
    int id;              // ID untuk procedural texture (1..8)
    PlanetInfo info;     // Informasi detail planet
};

// Informasi Matahari (Sun)
PlanetInfo sunInfo = {
    "YELLOW DWARF STAR",
    "1,392,700 KM",
    "0 KM (CENTER)",
    "230M YRS (GALAXY)",
    "27 DAYS",
    "8 PLANETS",
    "5,500 C (SURFACE)",
    "CONTAINS 99.8 PERCENT OF TOTAL",
    "MASS IN OUR SOLAR SYSTEM."
};

// Daftar 8 Planet di Tata Surya (Bahasa Inggris)
std::vector<Planet> planets = {
    {"MERCURY",  3.8f, 0.24f, 4.15f,  0.12f,   0.03f, false, 1,
        {"TERRESTRIAL PLANET", "4,879 KM", "57.9M KM", "88 DAYS", "58.6 DAYS", "0", "-180 TO 430 C", "SMALLEST PLANET AND CLOSEST", "TO THE SUN WITH NO ATMOSPHERE."}},
    {"VENUS",    5.6f, 0.40f, 1.62f, -0.06f, 177.3f, false, 2,
        {"TERRESTRIAL PLANET", "12,104 KM", "108.2M KM", "225 DAYS", "243 DAYS (RETRO)", "0", "465 C", "HOTTEST PLANET WITH TOXIC", "SULFURIC ACID CLOUD LAYERS."}},
    {"EARTH",    7.8f, 0.45f, 1.00f,  1.00f,  23.4f, false, 3,
        {"TERRESTRIAL PLANET", "12,742 KM", "149.6M KM (1 AU)", "365.25 DAYS", "23.9 HOURS", "1 (THE MOON)", "15 C (AVG)", "OUR HOME PLANET AND THE ONLY", "KNOWN WORLD WITH ACTIVE LIFE."}},
    {"MARS",    10.0f, 0.32f, 0.53f,  0.98f,  25.2f, false, 4,
        {"TERRESTRIAL PLANET", "6,779 KM", "227.9M KM", "687 DAYS", "24.6 HOURS", "2 (PHOBOS, DEIMOS)", "-60 C (AVG)", "THE RED PLANET WITH GIANT", "VOLCANOES AND POLAR ICE CAPS."}},
    {"JUPITER", 13.8f, 1.10f, 0.24f,  2.42f,   3.1f, false, 5,
        {"GAS GIANT", "139,820 KM", "778.5M KM", "11.86 YEARS", "9.9 HOURS", "95 CONFIRMED", "-110 C", "LARGEST PLANET WITH POWERFUL", "STORMS AND GREAT RED SPOT."}},
    {"SATURN",  18.0f, 0.90f, 0.15f,  2.24f,  26.7f, true,  6,
        {"GAS GIANT", "116,460 KM", "1.43B KM", "29.45 YEARS", "10.7 HOURS", "146 CONFIRMED", "-140 C", "FAMOUS FOR ITS SPECTACULAR", "COMPLEX ICY RING SYSTEM."}},
    {"URANUS",  22.2f, 0.62f, 0.09f,  1.40f,  97.8f, true,  7,
        {"ICE GIANT", "50,724 KM", "2.87B KM", "84 YEARS", "17.2 HOURS", "28 CONFIRMED", "-195 C", "UNIQUE TILTED PLANET THAT", "ROTATES ENTIRELY ON ITS SIDE."}},
    {"NEPTUNE", 26.0f, 0.58f, 0.06f,  1.50f,  28.3f, false, 8,
        {"ICE GIANT", "49,244 KM", "4.50B KM", "164.8 YEARS", "16.1 HOURS", "16 CONFIRMED", "-200 C", "COLD AND SUPERSONIC WINDY", "WORLD WITH METHANE CLOUDS."}}
};

// Variabel Kontrol Kamera 3D & Target Tracking
float camDistance = 34.0f;
float targetCamDist = 34.0f;
float camPitch = 28.0f;
float camYaw = -45.0f;

float currentLookAtX = 0.0f;
float currentLookAtY = 0.0f;
float currentLookAtZ = 0.0f;

// Status Fokus Objek (-1: Overview, 0: Sun, 1..8: Planet)
int focusedObject = -1;
bool isTransitioning = false;

// Status Hover Mouse (-1: None, 0: Sun, 1..8: Planet)
int hoveredObject = -1;
int prevHoveredObject = -1;
bool isBottomButtonHovered = false;
bool prevButtonHovered = false;

// Variabel Mouse
bool isDragging = false;
double currentMouseX = 0, currentMouseY = 0;
double pressMouseX = 0, pressMouseY = 0;
double lastMouseX = 0, lastMouseY = 0;

// Simulasi Waktu
float simSpeed = 1.0f;
bool isPaused = false;
double lastTime = 0.0;
float globalTime = 0.0f;

// Matriks untuk Proyeksi Layar & Picking
float savedModelView[16];
float savedProjection[16];
int savedViewport[4];

// Bintang 3D
struct Star {
    float x, y, z;
    float brightness;
};
std::vector<Star> stars;

void initStars(int count = 800) {
    stars.clear();
    for (int i = 0; i < count; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        float dist = 70.0f + ((float)rand() / RAND_MAX) * 40.0f;
        Star s;
        s.x = dist * sinf(phi) * cosf(theta);
        s.y = dist * sinf(phi) * sinf(theta);
        s.z = dist * cosf(phi);
        s.brightness = 0.4f + ((float)rand() / RAND_MAX) * 0.6f;
        stars.push_back(s);
    }
}

// Proyeksi Perspektif 3D
void setPerspective(float fovY, float aspect, float zNear, float zFar) {
    float fH = tanf(fovY / 360.0f * M_PI) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

// Fungsi Mengatur Fokus Objek
void setFocus(int newFocusId) {
    if (newFocusId != focusedObject) {
        if (newFocusId == -1) {
            playSFX(SFX_BACK);
        } else {
            playSFX(SFX_CLICK);
        }
    }

    focusedObject = newFocusId;
    isTransitioning = true;

    if (focusedObject == 0) {
        targetCamDist = 7.5f;
    } else if (focusedObject >= 1 && focusedObject <= 8) {
        const auto& p = planets[focusedObject - 1];
        targetCamDist = p.radius * 3.8f + 1.0f;
    } else {
        targetCamDist = 34.0f;
    }
}

// Proyeksi Koordinat 3D ke Koordinat Piksel Layar 2D
bool projectToScreen(float objX, float objY, float objZ, float &screenX, float &screenY, float &depth) {
    float eyeX = savedModelView[0] * objX + savedModelView[4] * objY + savedModelView[8]  * objZ + savedModelView[12];
    float eyeY = savedModelView[1] * objX + savedModelView[5] * objY + savedModelView[9]  * objZ + savedModelView[13];
    float eyeZ = savedModelView[2] * objX + savedModelView[6] * objY + savedModelView[10] * objZ + savedModelView[14];
    float eyeW = savedModelView[3] * objX + savedModelView[7] * objY + savedModelView[11] * objZ + savedModelView[15];

    float clipX = savedProjection[0] * eyeX + savedProjection[4] * eyeY + savedProjection[8]  * eyeZ + savedProjection[12] * eyeW;
    float clipY = savedProjection[1] * eyeX + savedProjection[5] * eyeY + savedProjection[9]  * eyeZ + savedProjection[13] * eyeW;
    float clipZ = savedProjection[2] * eyeX + savedProjection[6] * eyeY + savedProjection[10] * eyeZ + savedProjection[14] * eyeW;
    float clipW = savedProjection[3] * eyeX + savedProjection[7] * eyeY + savedProjection[11] * eyeZ + savedProjection[15] * eyeW;

    if (clipW <= 0.0001f) return false;

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;
    float ndcZ = clipZ / clipW;

    screenX = (ndcX + 1.0f) * 0.5f * (float)savedViewport[2] + (float)savedViewport[0];
    screenY = (1.0f - ndcY) * 0.5f * (float)savedViewport[3];
    depth = ndcZ;
    return true;
}

// Menentukan Objek Tata Surya di Bawah Posisi Kursor Mouse
int pickObject(double mouseX, double mouseY) {
    if (focusedObject != -1 && mouseX < 310.0 && mouseY < 520.0) {
        return -1;
    }

    float bestDist = 1e9f;
    float bestDepth = 1e9f;
    int selectedId = -1;

    // 1. Cek Sun (ID: 0)
    float sunScreenX, sunScreenY, sunDepth;
    if (projectToScreen(0.0f, 0.0f, 0.0f, sunScreenX, sunScreenY, sunDepth)) {
        float dist = (float)hypot(mouseX - sunScreenX, mouseY - sunScreenY);
        float clickRadius = 40.0f;
        if (dist <= clickRadius) {
            bestDist = dist;
            bestDepth = sunDepth;
            selectedId = 0;
        }
    }

    // 2. Cek 8 Planet (ID: 1..8)
    for (size_t i = 0; i < planets.size(); ++i) {
        const auto& p = planets[i];
        float orbitAngle = globalTime * p.orbitSpeed * 25.0f;
        float radA = orbitAngle * M_PI / 180.0f;
        float px = p.distance * cosf(radA);
        float py = 0.0f;
        float pz = -p.distance * sinf(radA);

        float pScreenX, pScreenY, pDepth;
        if (projectToScreen(px, py, pz, pScreenX, pScreenY, pDepth)) {
            float dist = (float)hypot(mouseX - pScreenX, mouseY - pScreenY);
            
            float clickRadius = (p.radius / camDistance) * 700.0f;
            if (clickRadius < 24.0f) clickRadius = 24.0f;

            if (dist <= clickRadius) {
                if (pDepth < bestDepth || dist < bestDist) {
                    bestDist = dist;
                    bestDepth = pDepth;
                    selectedId = (int)(i + 1);
                }
            }
        }
    }

    return selectedId;
}

// Fungsi Menggambar Karakter Huruf & Angka Vektor
void drawChar(char c, float x, float y, float w, float h) {
    c = (char)toupper((unsigned char)c);
    glBegin(GL_LINES);
    switch (c) {
        case 'A':
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            break;
        case 'B':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w * 0.8f, y);
            glVertex2f(x + w * 0.8f, y); glVertex2f(x + w * 0.8f, y + h * 0.5f);
            glVertex2f(x + w * 0.8f, y + h * 0.5f); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w * 0.8f, y + h * 0.5f);
            glVertex2f(x + w * 0.8f, y + h * 0.5f); glVertex2f(x + w * 0.8f, y + h);
            glVertex2f(x + w * 0.8f, y + h); glVertex2f(x, y + h);
            break;
        case 'C':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case 'D':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w * 0.7f, y);
            glVertex2f(x + w * 0.7f, y); glVertex2f(x + w, y + h * 0.3f);
            glVertex2f(x + w, y + h * 0.3f); glVertex2f(x + w, y + h * 0.7f);
            glVertex2f(x + w, y + h * 0.7f); glVertex2f(x + w * 0.7f, y + h);
            glVertex2f(x + w * 0.7f, y + h); glVertex2f(x, y + h);
            break;
        case 'E':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w * 0.7f, y + h * 0.5f);
            break;
        case 'F':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w * 0.7f, y + h * 0.5f);
            break;
        case 'G':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x + w * 0.5f, y + h * 0.5f);
            break;
        case 'H':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            break;
        case 'I':
            glVertex2f(x + w * 0.5f, y); glVertex2f(x + w * 0.5f, y + h);
            glVertex2f(x + w * 0.2f, y); glVertex2f(x + w * 0.8f, y);
            glVertex2f(x + w * 0.2f, y + h); glVertex2f(x + w * 0.8f, y + h);
            break;
        case 'J':
            glVertex2f(x + w * 0.8f, y); glVertex2f(x + w * 0.8f, y + h * 0.8f);
            glVertex2f(x + w * 0.8f, y + h * 0.8f); glVertex2f(x + w * 0.5f, y + h);
            glVertex2f(x + w * 0.5f, y + h); glVertex2f(x + w * 0.2f, y + h * 0.8f);
            glVertex2f(x + w * 0.2f, y + h * 0.8f); glVertex2f(x + w * 0.2f, y + h * 0.6f);
            break;
        case 'K':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x + w, y); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h);
            break;
        case 'L':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case 'M':
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + w * 0.5f, y + h * 0.6f);
            glVertex2f(x + w * 0.5f, y + h * 0.6f); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            break;
        case 'N':
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x + w, y);
            break;
        case 'O':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x, y);
            break;
        case 'P':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x, y + h * 0.5f);
            break;
        case 'R':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h);
            break;
        case 'S':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            break;
        case 'T':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w * 0.5f, y); glVertex2f(x + w * 0.5f, y + h);
            break;
        case 'U':
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x + w, y);
            break;
        case 'V':
            glVertex2f(x, y); glVertex2f(x + w * 0.5f, y + h);
            glVertex2f(x + w * 0.5f, y + h); glVertex2f(x + w, y);
            break;
        case 'W':
            glVertex2f(x, y); glVertex2f(x + w * 0.25f, y + h);
            glVertex2f(x + w * 0.25f, y + h); glVertex2f(x + w * 0.5f, y + h * 0.4f);
            glVertex2f(x + w * 0.5f, y + h * 0.4f); glVertex2f(x + w * 0.75f, y + h);
            glVertex2f(x + w * 0.75f, y + h); glVertex2f(x + w, y);
            break;
        case 'X':
            glVertex2f(x, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y); glVertex2f(x, y + h);
            break;
        case 'Y':
            glVertex2f(x, y); glVertex2f(x + w * 0.5f, y + h * 0.5f);
            glVertex2f(x + w, y); glVertex2f(x + w * 0.5f, y + h * 0.5f);
            glVertex2f(x + w * 0.5f, y + h * 0.5f); glVertex2f(x + w * 0.5f, y + h);
            break;
        case 'Z':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case '0':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x + w, y); glVertex2f(x, y + h);
            break;
        case '1':
            glVertex2f(x + w * 0.5f, y); glVertex2f(x + w * 0.5f, y + h);
            glVertex2f(x + w * 0.2f, y + h * 0.3f); glVertex2f(x + w * 0.5f, y);
            glVertex2f(x + w * 0.2f, y + h); glVertex2f(x + w * 0.8f, y + h);
            break;
        case '2':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            break;
        case '3':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            break;
        case '4':
            glVertex2f(x, y); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w * 0.8f, y); glVertex2f(x + w * 0.8f, y + h);
            break;
        case '5':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            break;
        case '6':
            glVertex2f(x + w, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x + w, y + h * 0.5f); glVertex2f(x, y + h * 0.5f);
            break;
        case '7':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w * 0.3f, y + h);
            break;
        case '8':
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            glVertex2f(x, y + h); glVertex2f(x, y);
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            break;
        case '9':
            glVertex2f(x, y + h * 0.5f); glVertex2f(x + w, y + h * 0.5f);
            glVertex2f(x, y); glVertex2f(x, y + h * 0.5f);
            glVertex2f(x, y); glVertex2f(x + w, y);
            glVertex2f(x + w, y); glVertex2f(x + w, y + h);
            glVertex2f(x + w, y + h); glVertex2f(x, y + h);
            break;
        case ':':
            glVertex2f(x + w * 0.5f, y + h * 0.3f); glVertex2f(x + w * 0.5f, y + h * 0.35f);
            glVertex2f(x + w * 0.5f, y + h * 0.7f); glVertex2f(x + w * 0.5f, y + h * 0.75f);
            break;
        case '.':
            glVertex2f(x + w * 0.4f, y + h * 0.9f); glVertex2f(x + w * 0.6f, y + h * 0.9f);
            break;
        case ',':
            glVertex2f(x + w * 0.5f, y + h * 0.85f); glVertex2f(x + w * 0.3f, y + h);
            break;
        case '(':
            glVertex2f(x + w * 0.7f, y); glVertex2f(x + w * 0.3f, y + h * 0.5f);
            glVertex2f(x + w * 0.3f, y + h * 0.5f); glVertex2f(x + w * 0.7f, y + h);
            break;
        case ')':
            glVertex2f(x + w * 0.3f, y); glVertex2f(x + w * 0.7f, y + h * 0.5f);
            glVertex2f(x + w * 0.7f, y + h * 0.5f); glVertex2f(x + w * 0.3f, y + h);
            break;
        case '-':
            glVertex2f(x + w * 0.1f, y + h * 0.5f); glVertex2f(x + w * 0.9f, y + h * 0.5f);
            break;
        default:
            break;
    }
    glEnd();
}

// Fungsi Menulis Rentetan Teks String
void drawStrokeText(const std::string& text, float x, float y, float charW, float charH, float r, float g, float b, float a = 1.0f) {
    glColor4f(r, g, b, a);
    glLineWidth(1.6f);
    float curX = x;
    for (char c : text) {
        if (c == ' ') {
            curX += charW * 0.75f;
            continue;
        }
        drawChar(c, curX, y, charW, charH);
        curX += charW * 1.30f;
    }
}

// Menggambar Panel Informasi Planet Bergaya Glassmorphism Elegan di Kiri Layar
void drawGlassInfoPanel(int width, int height) {
    if (focusedObject == -1) return;

    std::string name = "SUN";
    PlanetInfo info = sunInfo;

    if (focusedObject >= 1 && focusedObject <= 8) {
        const auto& p = planets[focusedObject - 1];
        name = p.nameEng;
        info = p.info;
    }

    float panelX = 22.0f;
    float panelY = 25.0f;
    float panelW = 275.0f;
    float panelH = 460.0f;

    float rX = panelX + panelW;
    float bY = panelY + panelH;

    // 1. Layer Blur / Dark Tint Glass
    glBegin(GL_QUADS);
        glColor4f(0.02f, 0.05f, 0.12f, 0.88f);
        glVertex2f(panelX, panelY);
        glColor4f(0.03f, 0.08f, 0.18f, 0.85f);
        glVertex2f(rX, panelY);
        glColor4f(0.01f, 0.03f, 0.08f, 0.92f);
        glVertex2f(rX, bY);
        glColor4f(0.01f, 0.03f, 0.08f, 0.92f);
        glVertex2f(panelX, bY);
    glEnd();

    // 2. Gloss Highlight Header (Efek Pantulan Kaca)
    glBegin(GL_QUADS);
        glColor4f(0.20f, 0.50f, 0.80f, 0.30f);
        glVertex2f(panelX, panelY);
        glVertex2f(rX, panelY);
        glColor4f(0.05f, 0.15f, 0.30f, 0.0f);
        glVertex2f(rX, panelY + 65.0f);
        glVertex2f(panelX, panelY + 65.0f);
    glEnd();

    // 3. Border Kaca Neon Halus
    glLineWidth(1.6f);
    glBegin(GL_LINE_LOOP);
        glColor4f(0.35f, 0.75f, 1.0f, 0.70f);
        glVertex2f(panelX, panelY);
        glVertex2f(rX, panelY);
        glColor4f(0.15f, 0.40f, 0.70f, 0.40f);
        glVertex2f(rX, bY);
        glVertex2f(panelX, bY);
    glEnd();

    // 4. Aksen Garis Neon Vertikal di Kiri Header
    glBegin(GL_QUADS);
        glColor4f(0.0f, 0.85f, 1.0f, 0.95f);
        glVertex2f(panelX + 12.0f, panelY + 16.0f);
        glVertex2f(panelX + 16.0f, panelY + 16.0f);
        glVertex2f(panelX + 16.0f, panelY + 45.0f);
        glVertex2f(panelX + 12.0f, panelY + 45.0f);
    glEnd();

    // 5. Header: Judul Nama Planet & Tipe
    drawStrokeText(name, panelX + 24.0f, panelY + 16.0f, 10.5f, 15.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    drawStrokeText(info.type, panelX + 24.0f, panelY + 36.0f, 5.5f, 7.5f, 0.35f, 0.85f, 1.0f, 0.90f);

    // Garis Pemisah Header
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glColor4f(0.30f, 0.60f, 0.90f, 0.45f);
        glVertex2f(panelX + 14.0f, panelY + 54.0f);
        glVertex2f(rX - 14.0f, panelY + 54.0f);
    glEnd();

    // 6. Baris Informasi Data Spesifikasi
    float rowY = panelY + 68.0f;
    float labelW = 5.2f, labelH = 7.5f;
    float valW = 5.6f, valH = 8.0f;

    struct DataRow { std::string label; std::string val; };
    std::vector<DataRow> rows = {
        {"DIAMETER", info.diameter},
        {"DISTANCE", info.distance},
        {"ORBIT PERIOD", info.orbitPeriod},
        {"ROTATION", info.dayLength},
        {"MOONS", info.moons},
        {"AVG TEMP", info.avgTemp}
    };

    for (const auto& r : rows) {
        drawStrokeText(r.label, panelX + 16.0f, rowY, labelW, labelH, 0.60f, 0.75f, 0.90f, 0.85f);
        drawStrokeText(r.val, panelX + 16.0f, rowY + 11.0f, valW, valH, 1.0f, 0.98f, 0.92f, 1.0f);
        
        glLineWidth(0.8f);
        glBegin(GL_LINES);
            glColor4f(0.20f, 0.40f, 0.65f, 0.25f);
            glVertex2f(panelX + 16.0f, rowY + 24.0f);
            glVertex2f(rX - 16.0f, rowY + 24.0f);
        glEnd();

        rowY += 31.0f;
    }

    // 7. Kotak Ringkasan Fakta / Deskripsi di Bawah
    float factBoxY = rowY + 10.0f;
    glBegin(GL_QUADS);
        glColor4f(0.05f, 0.15f, 0.30f, 0.45f);
        glVertex2f(panelX + 12.0f, factBoxY);
        glVertex2f(rX - 12.0f, factBoxY);
        glVertex2f(rX - 12.0f, factBoxY + 55.0f);
        glVertex2f(panelX + 12.0f, factBoxY + 55.0f);
    glEnd();

    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glColor4f(0.25f, 0.65f, 0.90f, 0.35f);
        glVertex2f(panelX + 12.0f, factBoxY);
        glVertex2f(rX - 12.0f, factBoxY);
        glVertex2f(rX - 12.0f, factBoxY + 55.0f);
        glVertex2f(panelX + 12.0f, factBoxY + 55.0f);
    glEnd();

    drawStrokeText("KEY FACTS:", panelX + 18.0f, factBoxY + 8.0f, 5.0f, 6.8f, 0.35f, 0.90f, 1.0f, 0.95f);
    drawStrokeText(info.factLine1, panelX + 18.0f, factBoxY + 22.0f, 4.8f, 6.5f, 0.85f, 0.90f, 0.95f, 0.90f);
    drawStrokeText(info.factLine2, panelX + 18.0f, factBoxY + 36.0f, 4.8f, 6.5f, 0.85f, 0.90f, 0.95f, 0.90f);
}

// Menggambar Badge Bersih di Atas Planet (Hanya Nama Planet)
void drawPlanetBadge(float screenX, float screenY, const std::string& title, bool isFocused) {
    float charW = 9.0f;
    float charH = 13.0f;
    float textW = title.length() * (charW * 1.30f);
    float padX = 12.0f;
    float padY = 7.0f;
    float boxW = textW + padX * 2.0f;
    float boxH = charH + padY * 2.0f;

    float boxLeft = screenX - boxW * 0.5f;
    float boxTop  = screenY - boxH - 12.0f;
    float boxRight = boxLeft + boxW;
    float boxBottom = boxTop + boxH;

    // 1. Background Semi-transparan (Card Panel)
    glBegin(GL_QUADS);
    if (isFocused) {
        glColor4f(0.03f, 0.10f, 0.20f, 0.88f);
    } else {
        glColor4f(0.02f, 0.04f, 0.08f, 0.82f);
    }
    glVertex2f(boxLeft, boxTop);
    glVertex2f(boxRight, boxTop);
    glVertex2f(boxRight, boxBottom);
    glVertex2f(boxLeft, boxBottom);
    glEnd();

    // 2. Garis Border Glowing
    glLineWidth(1.6f);
    glBegin(GL_LINE_LOOP);
    if (isFocused) {
        glColor4f(0.2f, 0.85f, 1.0f, 0.95f);
    } else {
        glColor4f(1.0f, 0.80f, 0.30f, 0.85f);
    }
    glVertex2f(boxLeft, boxTop);
    glVertex2f(boxRight, boxTop);
    glVertex2f(boxRight, boxBottom);
    glVertex2f(boxLeft, boxBottom);
    glEnd();

    // 3. Panah Kecil Menunjuk ke Planet
    glBegin(GL_TRIANGLES);
    if (isFocused) {
        glColor4f(0.2f, 0.85f, 1.0f, 0.95f);
    } else {
        glColor4f(1.0f, 0.80f, 0.30f, 0.85f);
    }
    glVertex2f(screenX - 4.5f, boxBottom);
    glVertex2f(screenX + 4.5f, boxBottom);
    glVertex2f(screenX, boxBottom + 5.5f);
    glEnd();

    // 4. Gambar Teks Judul Bahasa Inggris
    float textX = boxLeft + padX;
    float textY = boxTop + padY;
    if (isFocused) {
        drawStrokeText(title, textX, textY, charW, charH, 1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        drawStrokeText(title, textX, textY, charW, charH, 1.0f, 0.95f, 0.82f, 1.0f);
    }
}

// Koordinat Tombol Subtitle Bawah (Untuk Interaksi Klik Mouse)
float btnLeft = 0, btnTop = 0, btnRight = 0, btnBottom = 0;

// Menggambar Tombol Subtitle Bawah: "PRESS ESC TO STOP FOLLOWING"
void drawBottomSubtitle(int width, int height) {
    if (focusedObject == -1) return;

    std::string subText = "PRESS ESC TO STOP FOLLOWING";
    float charW = 7.0f;
    float charH = 10.0f;
    float textW = subText.length() * (charW * 1.30f);
    float padX = 18.0f;
    float padY = 8.0f;
    float boxW = textW + padX * 2.0f;
    float boxH = charH + padY * 2.0f;

    btnLeft = (width - boxW) * 0.5f;
    btnBottom = height - 22.0f;
    btnTop = btnBottom - boxH;
    btnRight = btnLeft + boxW;

    // Background Tombol Pill Subtitle
    glBegin(GL_QUADS);
    if (isBottomButtonHovered) {
        glColor4f(0.08f, 0.22f, 0.42f, 0.92f);
    } else {
        glColor4f(0.02f, 0.05f, 0.10f, 0.80f);
    }
    glVertex2f(btnLeft, btnTop);
    glVertex2f(btnRight, btnTop);
    glVertex2f(btnRight, btnBottom);
    glVertex2f(btnLeft, btnBottom);
    glEnd();

    // Border Tombol
    glLineWidth(isBottomButtonHovered ? 1.8f : 1.2f);
    glBegin(GL_LINE_LOOP);
    if (isBottomButtonHovered) {
        glColor4f(0.4f, 0.85f, 1.0f, 0.95f);
    } else {
        glColor4f(0.35f, 0.65f, 0.95f, 0.60f);
    }
    glVertex2f(btnLeft, btnTop);
    glVertex2f(btnRight, btnTop);
    glVertex2f(btnRight, btnBottom);
    glVertex2f(btnLeft, btnBottom);
    glEnd();

    // Teks Subtitle
    float textX = btnLeft + padX;
    float textY = btnTop + padY;
    if (isBottomButtonHovered) {
        drawStrokeText(subText, textX, textY, charW, charH, 1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        drawStrokeText(subText, textX, textY, charW, charH, 0.88f, 0.94f, 1.0f, 0.92f);
    }
}

// Menggambar Semua Komponen Overlay 2D (Panel Kiri, Badge Planet, Subtitle Bawah)
void drawAllLabels(int width, int height) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 1. Gambar Panel Kiri Info Glassmorphic jika sedang Fokus
    drawGlassInfoPanel(width, height);

    // 2. Gambar Badge Nama di Atas Planet
    std::vector<int> objectsToDraw;
    if (focusedObject != -1) {
        objectsToDraw.push_back(focusedObject);
    }
    if (hoveredObject != -1 && hoveredObject != focusedObject) {
        objectsToDraw.push_back(hoveredObject);
    }

    for (int objId : objectsToDraw) {
        float px = 0.0f, py = 0.0f, pz = 0.0f, pRadius = 1.8f;
        std::string name = "SUN";

        if (objId == 0) {
            px = 0.0f; py = 0.0f; pz = 0.0f;
            pRadius = 1.8f;
            name = "SUN";
        } else if (objId >= 1 && objId <= 8) {
            const auto& p = planets[objId - 1];
            float orbitAngle = globalTime * p.orbitSpeed * 25.0f;
            float radA = orbitAngle * M_PI / 180.0f;
            px = p.distance * cosf(radA);
            py = 0.0f;
            pz = -p.distance * sinf(radA);
            pRadius = p.radius;
            name = p.nameEng;
        }

        float topX = px;
        float topY = py + pRadius * 1.15f;
        float topZ = pz;

        float screenX, screenY, depth;
        if (projectToScreen(topX, topY, topZ, screenX, screenY, depth)) {
            bool isFoc = (objId == focusedObject);
            drawPlanetBadge(screenX, screenY, name, isFoc);
        }
    }

    // 3. Gambar Tombol Subtitle di Bawah Layar Saat Sedang Tracking Objek
    drawBottomSubtitle(width, height);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// Fungsi Prosedural Detail Permukaan Setiap Planet
void getSurfaceColor(int planetId, float lat, float lng, float &r, float &g, float &b, float timeVal) {
    float absLat = fabsf(lat);

    switch (planetId) {
        // 0: SUN
        case 0: {
            float flare = sinf(lat * 8.0f + timeVal * 2.0f) * cosf(lng * 6.0f + timeVal);
            flare += 0.5f * sinf(lat * 16.0f - lng * 12.0f + timeVal * 3.0f);
            if (flare > 0.4f) {
                r = 1.0f; g = 0.98f; b = 0.60f;
            } else if (flare > -0.2f) {
                r = 1.0f; g = 0.82f; b = 0.12f;
            } else {
                r = 0.98f; g = 0.45f; b = 0.05f;
            }
            break;
        }

        // 1: MERCURY
        case 1: {
            float crater = sinf(lat * 12.0f) * sinf(lng * 10.0f) + cosf(lat * 6.0f + lng * 8.0f);
            float maria = sinf(lat * 3.0f) * cosf(lng * 4.0f);
            if (maria > 0.3f) {
                r = 0.45f; g = 0.42f; b = 0.39f;
            } else if (crater > 0.6f) {
                r = 0.82f; g = 0.80f; b = 0.77f;
            } else {
                r = 0.65f; g = 0.61f; b = 0.56f;
            }
            break;
        }

        // 2: VENUS
        case 2: {
            float wave = sinf(lat * 8.0f + 0.5f * sinf(lng * 4.0f + lat * 2.0f));
            wave += 0.25f * cosf(lat * 16.0f + lng * 3.0f);
            if (wave > 0.4f) {
                r = 0.96f; g = 0.92f; b = 0.76f;
            } else if (wave > -0.3f) {
                r = 0.90f; g = 0.76f; b = 0.48f;
            } else {
                r = 0.80f; g = 0.62f; b = 0.35f;
            }
            break;
        }

        // 3: EARTH
        case 3: {
            if (absLat > 1.05f) {
                r = 0.95f; g = 0.98f; b = 1.0f;
                return;
            }

            float land = sinf(lng * 2.0f) * cosf(lat) 
                       + 0.55f * sinf(lng * 3.0f + 1.2f) * cosf(lat * 2.0f) 
                       + 0.45f * cosf(lng * 4.0f - 0.5f) 
                       - 0.25f * sinf(lat * 3.0f);

            float clouds = sinf(lat * 8.0f + lng * 4.0f + timeVal * 0.2f) * cosf(lat * 4.0f - lng * 3.0f);
            if (clouds > 0.75f && absLat < 1.0f) {
                r = 0.92f; g = 0.95f; b = 0.98f;
                return;
            }

            if (land > 0.08f) {
                if (absLat > 0.75f) {
                    r = 0.55f; g = 0.60f; b = 0.45f;
                } else if (absLat > 0.25f && absLat < 0.55f && sinf(lng * 3.0f) > 0.1f) {
                    r = 0.78f; g = 0.70f; b = 0.42f;
                } else {
                    r = 0.16f; g = 0.58f; b = 0.22f;
                }
            } else {
                if (land > -0.05f) {
                    r = 0.12f; g = 0.50f; b = 0.78f;
                } else {
                    r = 0.06f; g = 0.24f; b = 0.65f;
                }
            }
            break;
        }

        // 4: MARS
        case 4: {
            if (absLat > 1.15f) {
                r = 0.96f; g = 0.96f; b = 0.98f;
                return;
            }

            float canyon = sinf(lat * 6.0f) * cosf(lng * 4.0f) + 0.35f * sinf(lng * 7.0f);
            if (canyon > 0.45f) {
                r = 0.48f; g = 0.22f; b = 0.14f;
            } else if (canyon < -0.3f) {
                r = 0.92f; g = 0.48f; b = 0.22f;
            } else {
                r = 0.84f; g = 0.34f; b = 0.16f;
            }
            break;
        }

        // 5: JUPITER
        case 5: {
            float dLat = (lat - (-0.38f)) / 0.14f;
            float dLng = (lng - 1.20f) / 0.32f;
            if (dLat * dLat + dLng * dLng < 1.0f) {
                r = 0.86f; g = 0.22f; b = 0.12f;
                return;
            }

            float band = sinf(lat * 18.0f) + 0.20f * sinf(lat * 36.0f);
            if (band > 0.45f) {
                r = 0.95f; g = 0.88f; b = 0.78f;
            } else if (band > 0.0f) {
                r = 0.82f; g = 0.60f; b = 0.42f;
            } else if (band > -0.5f) {
                r = 0.70f; g = 0.40f; b = 0.25f;
            } else {
                r = 0.58f; g = 0.32f; b = 0.18f;
            }
            break;
        }

        // 6: SATURN
        case 6: {
            if (lat > 1.25f) {
                r = 0.60f; g = 0.62f; b = 0.45f;
                return;
            }

            float sBand = sinf(lat * 16.0f) + 0.15f * sinf(lat * 32.0f);
            if (sBand > 0.4f) {
                r = 0.94f; g = 0.88f; b = 0.68f;
            } else if (sBand > -0.2f) {
                r = 0.86f; g = 0.78f; b = 0.55f;
            } else {
                r = 0.75f; g = 0.65f; b = 0.44f;
            }
            break;
        }

        // 7: URANUS
        case 7: {
            if (absLat > 1.15f) {
                r = 0.72f; g = 0.94f; b = 0.96f;
            } else {
                float uBand = sinf(lat * 10.0f);
                r = 0.46f + 0.08f * uBand;
                g = 0.82f + 0.06f * uBand;
                b = 0.86f + 0.05f * uBand;
            }
            break;
        }

        // 8: NEPTUNE
        case 8: {
            float dLat = (lat - (-0.42f)) / 0.16f;
            float dLng = (lng - 2.1f) / 0.35f;
            if (dLat * dLat + dLng * dLng < 1.0f) {
                r = 0.06f; g = 0.15f; b = 0.48f;
                return;
            }

            float cirrus = sinf(lat * 14.0f + lng * 6.0f) * cosf(lat * 6.0f);
            if (cirrus > 0.65f && absLat < 1.0f) {
                r = 0.75f; g = 0.90f; b = 1.0f;
            } else {
                float nBand = sinf(lat * 12.0f);
                r = 0.10f + 0.04f * nBand;
                g = 0.32f + 0.06f * nBand;
                b = 0.90f + 0.05f * nBand;
            }
            break;
        }

        default:
            r = 0.8f; g = 0.8f; b = 0.8f;
            break;
    }
}

// Menggambar Bola 3D
void drawDetailedSphere(float radius, int slices, int stacks, int planetId, float timeVal, bool isSun = false) {
    if (isSun) {
        GLfloat emit[] = {1.0f, 0.75f, 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, emit);
    } else {
        GLfloat emit[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, emit);
    }

    for (int i = 0; i < stacks; ++i) {
        float lat0 = M_PI * (-0.5f + (float)(i) / stacks);
        float y0  = radius * sinf(lat0);
        float r0  = radius * cosf(lat0);

        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
        float y1  = radius * sinf(lat1);
        float r1  = radius * cosf(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2.0f * M_PI * (float)(j) / slices;
            float cosL = cosf(lng);
            float sinL = sinf(lng);

            float x0 = r0 * cosL;
            float z0 = r0 * sinL;
            float x1 = r1 * cosL;
            float z1 = r1 * sinL;

            float red0, green0, blue0;
            getSurfaceColor(planetId, lat0, lng, red0, green0, blue0, timeVal);
            glColor3f(red0, green0, blue0);
            glNormal3f(x0 / radius, y0 / radius, z0 / radius);
            glVertex3f(x0, y0, z0);

            float red1, green1, blue1;
            getSurfaceColor(planetId, lat1, lng, red1, green1, blue1, timeVal);
            glColor3f(red1, green1, blue1);
            glNormal3f(x1 / radius, y1 / radius, z1 / radius);
            glVertex3f(x1, y1, z1);
        }
        glEnd();
    }
}

// Cincin Saturnus
void drawDetailedSaturnRing(float radius, int segments = 80) {
    GLfloat emit[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, emit);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        float c = cosf(angle), s = sinf(angle);
        glColor3f(0.55f, 0.48f, 0.35f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(radius * 1.25f * c, 0.0f, radius * 1.25f * s);
        glVertex3f(radius * 1.50f * c, 0.0f, radius * 1.50f * s);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        float c = cosf(angle), s = sinf(angle);
        glColor3f(0.94f, 0.88f, 0.70f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(radius * 1.52f * c, 0.0f, radius * 1.52f * s);
        glVertex3f(radius * 1.95f * c, 0.0f, radius * 1.95f * s);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        float c = cosf(angle), s = sinf(angle);
        glColor3f(0.80f, 0.74f, 0.58f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(radius * 2.05f * c, 0.0f, radius * 2.05f * s);
        glVertex3f(radius * 2.35f * c, 0.0f, radius * 2.35f * s);
    }
    glEnd();
}

// Cincin Uranus
void drawUranusRing(float radius, int segments = 60) {
    glDisable(GL_LIGHTING);
    glColor3f(0.60f, 0.85f, 0.90f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        glVertex3f(radius * 1.7f * cosf(angle), 0.0f, radius * 1.7f * sinf(angle));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// Garis Orbit
void drawOrbitLine(float radius, int segments = 120) {
    glDisable(GL_LIGHTING);
    glColor3f(0.20f, 0.25f, 0.35f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        glVertex3f(radius * cosf(theta), 0.0f, radius * sinf(theta));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// Bintang Latar Belakang
void drawStarfield() {
    glDisable(GL_LIGHTING);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (const auto& s : stars) {
        glColor3f(s.brightness, s.brightness, s.brightness);
        glVertex3f(s.x, s.y, s.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void display(int width, int height) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup Proyeksi 3D Perspektif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)(height > 0 ? height : 1);
    setPerspective(45.0f, aspect, 0.1f, 250.0f);

    // Simpan matriks Projection & Viewport untuk proyeksi 2D
    glGetFloatv(GL_PROJECTION_MATRIX, savedProjection);
    glGetIntegerv(GL_VIEWPORT, savedViewport);

    // Setup Kamera 3D
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -camDistance);
    glRotatef(camPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(camYaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-currentLookAtX, -currentLookAtY, -currentLookAtZ);

    // Simpan matriks ModelView untuk proyeksi 2D
    glGetFloatv(GL_MODELVIEW_MATRIX, savedModelView);

    // 1. Gambar Bintang di Latar Belakang
    drawStarfield();

    // 2. Setup Lampu dari Sun di (0,0,0)
    GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 3. SUN di Pusat
    glPushMatrix();
        glRotatef(globalTime * 10.0f, 0.0f, 1.0f, 0.0f);
        drawDetailedSphere(1.8f, 48, 32, 0, globalTime, true);
    glPopMatrix();

    // 4. 8 PLANET (Mercury s/d Neptune)
    for (size_t i = 0; i < planets.size(); ++i) {
        const auto& p = planets[i];

        // Gambar Garis Lintasan Orbit
        drawOrbitLine(p.distance);

        glPushMatrix();
            // REVOLUSI: Mengitari Sun
            float orbitAngle = globalTime * p.orbitSpeed * 25.0f;
            glRotatef(orbitAngle, 0.0f, 1.0f, 0.0f);
            
            // Translasi ke jarak orbit
            glTranslatef(p.distance, 0.0f, 0.0f);

            // ROTASI: Berputar pada poros sendiri
            glPushMatrix();
                glRotatef(p.axialTilt, 0.0f, 0.0f, 1.0f);

                float rotAngle = globalTime * p.rotSpeed * 60.0f;
                glRotatef(rotAngle, 0.0f, 1.0f, 0.0f);

                // Gambar Bola Planet
                drawDetailedSphere(p.radius, 48, 32, p.id, globalTime, false);

                // Cincin Saturn
                if (p.id == 6) {
                    drawDetailedSaturnRing(p.radius);
                }

                // Cincin Uranus
                if (p.id == 7) {
                    drawUranusRing(p.radius);
                }

            glPopMatrix();

            // Khusus EARTH: Moon
            if (p.id == 3) {
                glPushMatrix();
                    float moonOrbit = globalTime * 12.0f * 25.0f;
                    glRotatef(moonOrbit, 0.0f, 1.0f, 0.2f);
                    glTranslatef(0.85f, 0.0f, 0.0f);
                    drawDetailedSphere(0.10f, 16, 12, 1, 0.0f, false);
                glPopMatrix();
            }

        glPopMatrix();
    }

    // 5. Gambar Semua Komponen Overlay 2D (Panel Kiri, Badge Nama, Subtitle Bawah)
    drawAllLabels(width, height);
}

// Callback Posisi Kursor Mouse (Hover Detection)
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    currentMouseX = xpos;
    currentMouseY = ypos;

    // Cek apakah mouse sedang meng-hover tombol subtitle di bawah
    if (focusedObject != -1 &&
        xpos >= btnLeft && xpos <= btnRight &&
        ypos >= btnTop && ypos <= btnBottom) {
        isBottomButtonHovered = true;
    } else {
        isBottomButtonHovered = false;
    }

    if (isBottomButtonHovered != prevButtonHovered) {
        if (isBottomButtonHovered) playSFX(SFX_HOVER);
        prevButtonHovered = isBottomButtonHovered;
    }

    if (isDragging) {
        float dx = (float)(xpos - lastMouseX);
        float dy = (float)(ypos - lastMouseY);
        camYaw += dx * 0.35f;
        camPitch += dy * 0.35f;

        if (camPitch > 89.0f) camPitch = 89.0f;
        if (camPitch < -89.0f) camPitch = -89.0f;

        lastMouseX = xpos;
        lastMouseY = ypos;
    } else {
        hoveredObject = pickObject(xpos, ypos);
        if (hoveredObject != prevHoveredObject) {
            if (hoveredObject != -1) {
                playSFX(SFX_HOVER);
            }
            prevHoveredObject = hoveredObject;
        }
    }
}

// Callback Mouse Click & Drag
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            glfwGetCursorPos(window, &pressMouseX, &pressMouseY);
            lastMouseX = pressMouseX;
            lastMouseY = pressMouseY;
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
            double releaseX, releaseY;
            glfwGetCursorPos(window, &releaseX, &releaseY);

            float moveDist = (float)hypot(releaseX - pressMouseX, releaseY - pressMouseY);
            if (moveDist < 7.0f) {
                // 1. Cek apakah pengguna mengklik tombol "PRESS ESC TO STOP FOLLOWING" di bawah
                if (focusedObject != -1 &&
                    releaseX >= btnLeft && releaseX <= btnRight &&
                    releaseY >= btnTop && releaseY <= btnBottom) {
                    setFocus(-1);
                    std::cout << "-> [BUTTON CLICK] Stop following. Returning to Solar System Overview." << std::endl;
                    return;
                }

                // 2. Cek apakah pengguna mengklik planet atau Matahari
                int clickedId = pickObject(releaseX, releaseY);
                if (clickedId != -1) {
                    setFocus(clickedId);
                    if (clickedId == 0) {
                        std::cout << "-> [CLICK] Zoom In & Tracking: SUN" << std::endl;
                    } else {
                        std::cout << "-> [CLICK] Zoom In & Tracking: " << planets[clickedId - 1].nameEng << std::endl;
                    }
                }
            }
        }
    }
}

// Callback Scroll Mouse untuk Zoom In / Out
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    targetCamDist -= (float)yoffset * 1.5f;
    if (targetCamDist < 1.0f) targetCamDist = 1.0f;
    if (targetCamDist > 120.0f) targetCamDist = 120.0f;
}

// Callback Keyboard
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_ESCAPE) {
            if (focusedObject != -1) {
                setFocus(-1);
                std::cout << "-> [ESC] Exit focus. Returning to Solar System Overview." << std::endl;
            }
        }

        if (key == GLFW_KEY_SPACE) {
            isPaused = !isPaused;
            if (isPaused) {
                playSFX(SFX_PAUSE);
                std::cout << "-> [PAUSE] Time Frozen." << std::endl;
            } else {
                playSFX(SFX_RESUME);
                std::cout << "-> [RESUME] Time Resumed." << std::endl;
            }
        }
        if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_RIGHT) {
            simSpeed *= 1.25f;
            playSFX(SFX_SPEED_UP);
            std::cout << "-> [SPEED UP] Warp: " << simSpeed << "x" << std::endl;
        }
        if (key == GLFW_KEY_MINUS || key == GLFW_KEY_LEFT) {
            simSpeed /= 1.25f;
            if (simSpeed < 0.05f) simSpeed = 0.05f;
            playSFX(SFX_SLOW_DOWN);
            std::cout << "-> [SLOW DOWN] Warp: " << simSpeed << "x" << std::endl;
        }

        if (key >= GLFW_KEY_1 && key <= GLFW_KEY_8) {
            setFocus(key - GLFW_KEY_0);
            std::cout << "-> Focus: " << planets[focusedObject - 1].nameEng << std::endl;
        }
        if (key == GLFW_KEY_0) {
            setFocus(0);
            std::cout << "-> Focus: SUN" << std::endl;
        }
        if (key == GLFW_KEY_R) {
            setFocus(-1);
            camPitch = 28.0f;
            camYaw = -45.0f;
            simSpeed = 1.0f;
            std::cout << "-> Reset to Solar System Overview" << std::endl;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Gagal menginisialisasi GLFW!" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "3D Solar System - Procedural Sci-Fi SFX & Glassmorphism UI", NULL, NULL);
    if (!window) {
        std::cerr << "Gagal membuat window GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inisialisasi Audio Synthesizer Prosedural
    initAudioSynthesizer();

    // Registrasi Callback Input
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // Inisialisasi Fitur 3D OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Setup Pencahayaan
    GLfloat ambientLight[]  = {0.18f, 0.18f, 0.18f, 1.0f};
    GLfloat diffuseLight[]  = {1.0f, 0.98f, 0.92f, 1.0f};
    GLfloat specularLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Background Luar Angkasa
    glClearColor(0.01f, 0.01f, 0.02f, 1.0f);

    // Bintang Latar Belakang
    initStars(800);

    lastTime = glfwGetTime();

    std::cout << "==================================================================" << std::endl;
    std::cout << "   3D SOLAR SYSTEM - PROCEDURAL SFX & GLASSMORPHISM INFO PANEL    " << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << "  MOUSE HOVER (Planet / Button) : [SFX: Soft Blip]                " << std::endl;
    std::cout << "  MOUSE CLICK (Planet / Sun)    : [SFX: Warp Zoom Whoosh]         " << std::endl;
    std::cout << "  TOMBOL [ESC] / KLIK BAWAH     : [SFX: Exit Chime]               " << std::endl;
    std::cout << "  SPASI (Spacebar)              : [SFX: Time Freeze / Power Down] " << std::endl;
    std::cout << "  PANAH KANAN (+) / KIRI (-)    : [SFX: Warp Accelerate / Decel]  " << std::endl;
    std::cout << "  Scroll Mouse (Roda Mouse)     : Zoom In / Zoom Out Bebas        " << std::endl;
    std::cout << "  Mouse Drag (Klik & Geser)     : Putar Sudut Pandang Kamera      " << std::endl;
    std::cout << "==================================================================" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        lastTime = currentTime;

        if (!isPaused) {
            globalTime += deltaTime * simSpeed;
        }

        // Hitung Posisi Sasaran Kamera
        float destX = 0.0f, destY = 0.0f, destZ = 0.0f;

        if (focusedObject == 0) {
            destX = 0.0f; destY = 0.0f; destZ = 0.0f;
        } else if (focusedObject >= 1 && focusedObject <= 8) {
            const auto& p = planets[focusedObject - 1];
            float orbitAngle = globalTime * p.orbitSpeed * 25.0f;
            float radA = orbitAngle * M_PI / 180.0f;
            destX = p.distance * cosf(radA);
            destY = 0.0f;
            destZ = -p.distance * sinf(radA);
        } else {
            destX = 0.0f; destY = 0.0f; destZ = 0.0f;
        }

        // Perbarui Target Tracking Kamera
        if (isTransitioning) {
            float lerpSpeed = 8.0f;
            float lerpFactor = 1.0f - expf(-lerpSpeed * deltaTime);
            currentLookAtX += (destX - currentLookAtX) * lerpFactor;
            currentLookAtY += (destY - currentLookAtY) * lerpFactor;
            currentLookAtZ += (destZ - currentLookAtZ) * lerpFactor;
            camDistance    += (targetCamDist - camDistance) * lerpFactor;

            float distToTarget = (float)hypot(hypot(destX - currentLookAtX, destY - currentLookAtY), destZ - currentLookAtZ);
            if (distToTarget < 0.08f) {
                isTransitioning = false;
            }
        } else {
            currentLookAtX = destX;
            currentLookAtY = destY;
            currentLookAtZ = destZ;
            
            float zoomLerp = 1.0f - expf(-12.0f * deltaTime);
            camDistance += (targetCamDist - camDistance) * zoomLerp;
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        display(width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
