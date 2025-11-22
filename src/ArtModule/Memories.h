#ifndef MEMORIES_H
#define MEMORIES_H

#include <Arduino.h>
#include <EEPROM.h>

// Forward declaration
class Dibujar;

// Configuración
#define MEM_SLOTS 20
#define MEM_MAX_STEPS 10 // Ajusta según tu MAX_SEQ

// ⚠️ CRÍTICO: Estructura empaquetada para evitar padding
struct __attribute__((packed)) StepData
{
    long M1;      // 4 bytes
    long M2;      // 4 bytes
    int Modo;     // 2 bytes
    int modoMano; // 2 bytes
    // Total: 12 bytes por paso
};

struct __attribute__((packed)) SeqData
{
    uint8_t used;                  // 1 byte (cambio de bool a uint8_t)
    uint8_t largoSeq;              // 1 byte
    uint8_t seqCerrada;            // 1 byte (cambio de bool a uint8_t)
    uint8_t valoracion;            // 1 byte ⭐ NUEVO campo
    uint8_t emocionPadre;          // 1 byte ⭐ NUEVO campo
    uint8_t emocionANA;            // 1 byte ⭐ NUEVO campo
    uint8_t padding1;              // 1 byte de padding manual
    uint8_t padding2;              // 1 byte de padding adicional (total 8 bytes antes de long)
    long origenH;                  // 4 bytes
    long origenC;                  // 4 bytes
    StepData pasos[MEM_MAX_STEPS]; // 12 * 10 = 120 bytes
    uint16_t checksum;             // 2 bytes
    // Total aproximado: 142 bytes por slot
    // 142 bytes × 50 slots = 7100 bytes ⚠️ EXCEDE 4096 de Mega2560
};

// Funciones
uint16_t mem_checksum(const uint8_t *data, int len);
bool memories_saveSeq(int slot, Dibujar &dibujo);
bool memories_loadSeq(int slot, Dibujar &dibujo);
void memories_list();
void memories_clearAll();
void memories_clearSlot(int slot);
void memories_printSeq(const Dibujar &dibujo);
void memories_printInfo(const Dibujar &dibujo);

bool memories_setValoracion(int slot, uint8_t valoracion);
bool memories_setEmocionPadre(int slot, uint8_t emocion);
bool memories_setEmocionANA(int slot, uint8_t emocion);
bool memories_setEmociones(int slot, uint8_t valoracion, uint8_t emocionPadre, uint8_t emocionANA);
bool memories_getEmociones(int slot, uint8_t &valoracion, uint8_t &emocionPadre, uint8_t &emocionANA);
void memories_printInfoExtended(int slot);

// 🔍 FUNCIÓN DE DIAGNÓSTICO
inline void memories_diagnostics()
{
    Serial.println(F("\n========== DIAGNÓSTICO EEPROM =========="));
    Serial.print(F("Tamaño SeqData: "));
    Serial.print(sizeof(SeqData));
    Serial.println(F(" bytes"));

    Serial.print(F("Tamaño StepData: "));
    Serial.print(sizeof(StepData));
    Serial.println(F(" bytes"));

    Serial.print(F("Slots totales: "));
    Serial.println(MEM_SLOTS);

    Serial.print(F("Memoria total requerida: "));
    Serial.print(sizeof(SeqData) * MEM_SLOTS);
    Serial.println(F(" bytes"));

    Serial.print(F("EEPROM disponible en Mega2560: 4096 bytes"));

    if (sizeof(SeqData) * MEM_SLOTS > 4096)
    {
        Serial.println(F("\n⚠️ ERROR: Excede capacidad de EEPROM"));
        Serial.print(F("Slots máximos posibles: "));
        Serial.println(4096 / sizeof(SeqData));
        Serial.print(F("O reduce MEM_MAX_STEPS a: "));
        Serial.println((4096 / MEM_SLOTS - 16) / sizeof(StepData));
    }
    else
    {
        Serial.println(F("\n✅ Tamaño OK"));
    }
    Serial.println(F("========================================\n"));
}

#endif