#include "Memories.h"
#include "Dibujar.h"

/* ============================================================
   CHECKSUM SIMPLE
   ============================================================ */
uint16_t mem_checksum(const uint8_t *data, int len)
{
    uint16_t s = 0;
    for (int i = 0; i < len; i++)
        s += data[i];
    return s;
}

/* ============================================================
   GUARDAR SECUENCIA EN EEPROM
   ============================================================ */
bool memories_saveSeq(int slot, Dibujar &dibujo)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    memset(&blk, 0, sizeof(blk));

    auto &seq = dibujo.secuencia;

    if (seq.largoSeq == 0)
        return false;

    if (seq.largoSeq > MEM_MAX_STEPS)
        return false;

    // Copiar datos generales
    blk.used = 1;
    blk.largoSeq = (uint8_t)seq.largoSeq;
    blk.seqCerrada = seq.SeqCerrada ? 1 : 0;
    blk.padding1 = 0;
    blk.origenH = seq._posOrigenHombro;
    blk.origenC = seq._posOrigenCodo;

    // Copiar pasos
    for (int i = 0; i < seq.largoSeq; i++)
    {
        blk.pasos[i].M1 = seq.M1[i];
        blk.pasos[i].M2 = seq.M2[i];
        blk.pasos[i].Modo = seq.Modo[i];
        blk.pasos[i].modoMano = seq.modoMano[i];
    }

    // Checksum de todo el bloque, menos el campo checksum
    blk.checksum = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));

    // **DEBUG: Imprimir info antes de guardar**
    Serial.print(F("DEBUG SAVE - Slot: "));
    Serial.print(slot);
    Serial.print(F(" | Size: "));
    Serial.print(sizeof(SeqData));
    Serial.print(F(" | Checksum calculado: "));
    Serial.println(blk.checksum);

    // Guardar en EEPROM
    int addr = slot * sizeof(SeqData);
    EEPROM.put(addr, blk);

    delay(10); // Pequeña espera para asegurar escritura física

    // **Verificación inmediata: releer y comparar**
    SeqData verify;
    EEPROM.get(addr, verify);

    uint16_t verifyChecksum = mem_checksum((uint8_t *)&verify, sizeof(SeqData) - sizeof(verify.checksum));

    Serial.print(F("DEBUG VERIFY - Checksum leído: "));
    Serial.print(verify.checksum);
    Serial.print(F(" | Recalculado: "));
    Serial.println(verifyChecksum);

    if (verify.checksum != verifyChecksum)
    {
        Serial.println(F("⚠️ ERROR: Checksum no coincide inmediatamente después de guardar"));
        return false;
    }

    memories_printInfo(dibujo);
    memories_printSeq(dibujo);

    return true;
}

/* ============================================================
   CARGAR SECUENCIA DESDE EEPROM
   ============================================================ */
bool memories_loadSeq(int slot, Dibujar &dibujo)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    // **DEBUG: Mostrar datos crudos leídos**
    Serial.print(F("DEBUG LOAD - Slot: "));
    Serial.print(slot);
    Serial.print(F(" | Addr: "));
    Serial.print(addr);
    Serial.print(F(" | Used: "));
    Serial.print(blk.used);
    Serial.print(F(" | Checksum almacenado: "));
    Serial.println(blk.checksum);

    if (!blk.used)
    {
        Serial.println(F("ERROR: slot vacío o inválido"));
        return false;
    }

    // Verificar integridad
    uint16_t chk = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));

    Serial.print(F("DEBUG LOAD - Checksum recalculado: "));
    Serial.println(chk);

    if (chk != blk.checksum)
    {
        Serial.println(F("ERROR: checksum inválido, memoria corrupta."));

        // **DEBUG AVANZADO: Mostrar primeros bytes**
        Serial.print(F("Primeros 10 bytes: "));
        uint8_t *raw = (uint8_t *)&blk;
        for (int i = 0; i < 10; i++)
        {
            Serial.print(raw[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        return false;
    }

    auto &seq = dibujo.secuencia;

    seq.largoSeq = blk.largoSeq;
    seq.SeqCerrada = blk.seqCerrada;
    seq._posOrigenHombro = blk.origenH;
    seq._posOrigenCodo = blk.origenC;

    for (int i = 0; i < seq.largoSeq; i++)
    {
        seq.M1[i] = blk.pasos[i].M1;
        seq.M2[i] = blk.pasos[i].M2;
        seq.Modo[i] = blk.pasos[i].Modo;
        seq.modoMano[i] = blk.pasos[i].modoMano;
    }

    Serial.println(F("✅ Secuencia cargada correctamente."));
    memories_printInfo(dibujo);
    memories_printSeq(dibujo);

    return true;
}

/* ============================================================
   LISTAR MEMORIAS
   ============================================================ */
void memories_list()
{
    Serial.println(F("\nMEMORIAS GUARDADAS:"));
    Serial.println(F("---------------------"));

    for (int i = 0; i < MEM_SLOTS; i++)
    {
        SeqData blk;
        int addr = i * sizeof(SeqData);
        EEPROM.get(addr, blk);

        Serial.print("#");
        Serial.print(i);
        Serial.print(" : ");

        if (!blk.used)
        {
            Serial.println(F("[vacía]"));
            continue;
        }

        Serial.print(F("[OK]  Trazos="));
        Serial.print(blk.largoSeq);

        Serial.print(F("  Checksum="));
        Serial.print(blk.checksum);

        // **Verificar checksum**
        uint16_t chk = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));
        if (chk != blk.checksum)
        {
            Serial.print(F("  ⚠️ CORRUPTO (calc="));
            Serial.print(chk);
            Serial.print(")");
        }
        Serial.println();
    }
}

/* ============================================================
   LIMPIAR LA MEMORIA
   ============================================================ */
void memories_clearAll()
{
    SeqData empty;
    memset(&empty, 0, sizeof(SeqData));
    empty.used = false;

    for (int i = 0; i < MEM_SLOTS; i++)
    {
        int addr = i * sizeof(SeqData);
        EEPROM.put(addr, empty);
    }

    Serial.println(F("Todas las memorias fueron borradas."));
}

void memories_clearSlot(int slot)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return;

    SeqData empty;
    memset(&empty, 0, sizeof(SeqData));
    empty.used = false;

    int addr = slot * sizeof(SeqData);
    EEPROM.put(addr, empty);

    Serial.print(F("Slot "));
    Serial.print(slot);
    Serial.println(F(" borrado."));
}

/* ============================================================
   MOSTRAR TABLA DE MOVIMIENTOS
   ============================================================ */
void memories_printSeq(const Dibujar &dibujo)
{
    const auto &seq = dibujo.secuencia;

    Serial.println(F("\n==================================="));
    Serial.println(F("        TABLA DE MOVIMIENTOS       "));
    Serial.println(F("==================================="));
    Serial.println(F("Idx |   M1   |   M2   | Modo | Mano"));
    Serial.println(F("-----------------------------------"));

    for (int i = 0; i < seq.largoSeq; i++)
    {
        Serial.print(i);
        Serial.print(F("   | "));
        Serial.print(seq.M1[i]);
        Serial.print(F(" | "));
        Serial.print(seq.M2[i]);
        Serial.print(F(" | "));
        Serial.print(seq.Modo[i]);
        Serial.print(F("   | "));
        Serial.println(seq.modoMano[i]);
    }

    Serial.println(F("-----------------------------------"));
}

/* ============================================================
   MOSTRAR INFO PRINCIPAL DE LA SECUENCIA
   ============================================================ */
void memories_printInfo(const Dibujar &dibujo)
{
    const auto &seq = dibujo.secuencia;

    Serial.println(F("\n============ INFO SECUENCIA ============"));

    Serial.print(F("Largo (trazos): "));
    Serial.println(seq.largoSeq);

    Serial.print(F("Cerrada: "));
    Serial.println(seq.SeqCerrada ? "SI" : "NO");

    Serial.print(F("Origen Hombro: "));
    Serial.println(seq._posOrigenHombro);

    Serial.print(F("Origen Codo: "));
    Serial.println(seq._posOrigenCodo);

    Serial.println(F("========================================"));
}

/* ============================================================
   SETEAR VALORACIÓN EN UN SLOT
   ============================================================ */
bool memories_setValoracion(int slot, uint8_t valoracion)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    if (!blk.used)
    {
        Serial.println(F("ERROR: slot vacío"));
        return false;
    }

    // Modificar solo el campo valoración
    blk.valoracion = valoracion;

    // Recalcular checksum
    blk.checksum = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));

    // Guardar de nuevo
    EEPROM.put(addr, blk);
    delay(10);

    Serial.print(F("Valoración actualizada a: "));
    Serial.println(valoracion);

    return true;
}

/* ============================================================
   SETEAR EMOCIÓN PADRE EN UN SLOT
   ============================================================ */
bool memories_setEmocionPadre(int slot, uint8_t emocion)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    if (!blk.used)
    {
        Serial.println(F("ERROR: slot vacío"));
        return false;
    }

    blk.emocionPadre = emocion;
    blk.checksum = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));
    EEPROM.put(addr, blk);
    delay(10);

    Serial.print(F("Emoción Padre actualizada a: "));
    Serial.println(emocion);

    return true;
}

/* ============================================================
   SETEAR EMOCIÓN ANA EN UN SLOT
   ============================================================ */
bool memories_setEmocionANA(int slot, uint8_t emocion)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    if (!blk.used)
    {
        Serial.println(F("ERROR: slot vacío"));
        return false;
    }

    blk.emocionANA = emocion;
    blk.checksum = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));
    EEPROM.put(addr, blk);
    delay(10);

    Serial.print(F("Emoción ANA actualizada a: "));
    Serial.println(emocion);

    return true;
}

/* ============================================================
   SETEAR TODOS LOS CAMPOS EMOCIONALES A LA VEZ
   ============================================================ */
bool memories_setEmociones(int slot, uint8_t valoracion, uint8_t emocionPadre, uint8_t emocionANA)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    if (!blk.used)
    {
        Serial.println(F("ERROR: slot vacío"));
        return false;
    }

    // Actualizar los 3 campos
    blk.valoracion = valoracion;
    blk.emocionPadre = emocionPadre;
    blk.emocionANA = emocionANA;

    // Recalcular checksum
    blk.checksum = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));

    // Guardar
    EEPROM.put(addr, blk);
    delay(10);

    Serial.print(F("Emociones actualizadas - Val:"));
    Serial.print(valoracion);
    Serial.print(F(" Padre:"));
    Serial.print(emocionPadre);
    Serial.print(F(" ANA:"));
    Serial.println(emocionANA);

    return true;
}

/* ============================================================
   OBTENER TODOS LOS CAMPOS EMOCIONALES
   ============================================================ */
bool memories_getEmociones(int slot, uint8_t &valoracion, uint8_t &emocionPadre, uint8_t &emocionANA)
{
    if (slot < 0 || slot >= MEM_SLOTS)
        return false;

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    if (!blk.used)
        return false;

    // Verificar integridad
    uint16_t chk = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));
    if (chk != blk.checksum)
        return false;

    // Devolver valores por referencia
    valoracion = blk.valoracion;
    emocionPadre = blk.emocionPadre;
    emocionANA = blk.emocionANA;

    return true;
}

/* ============================================================
   VERSIÓN MEJORADA DE memories_printInfo
   Reemplaza la anterior para mostrar los nuevos campos
   ============================================================ */
void memories_printInfoExtended(int slot)
{
    if (slot < 0 || slot >= MEM_SLOTS)
    {
        Serial.println(F("ERROR: slot inválido"));
        return;
    }

    SeqData blk;
    int addr = slot * sizeof(SeqData);
    EEPROM.get(addr, blk);

    if (!blk.used)
    {
        Serial.println(F("ERROR: slot vacío"));
        return;
    }

    // Verificar integridad
    uint16_t chk = mem_checksum((uint8_t *)&blk, sizeof(SeqData) - sizeof(blk.checksum));
    if (chk != blk.checksum)
    {
        Serial.println(F("ERROR: checksum inválido"));
        return;
    }

    Serial.println(F("\n============ INFO SECUENCIA ============"));
    Serial.print(F("Slot: "));
    Serial.println(slot);

    Serial.print(F("Largo (trazos): "));
    Serial.println(blk.largoSeq);

    Serial.print(F("Cerrada: "));
    Serial.println(blk.seqCerrada ? "SI" : "NO");

    Serial.print(F("Origen Hombro: "));
    Serial.println(blk.origenH);

    Serial.print(F("Origen Codo: "));
    Serial.println(blk.origenC);

    Serial.println(F("\n--- Datos Emocionales ---"));
    Serial.print(F("Valoración: "));
    Serial.println(blk.valoracion);

    Serial.print(F("Emoción Padre: "));
    Serial.println(blk.emocionPadre);

    Serial.print(F("Emoción ANA: "));
    Serial.println(blk.emocionANA);

    Serial.println(F("========================================"));
}
