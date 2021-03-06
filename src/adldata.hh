/*
 * libADLMIDI is a free Software MIDI synthesizer library with OPL3 emulation
 *
 * Original ADLMIDI code: Copyright (c) 2010-2014 Joel Yliluoma <bisqwit@iki.fi>
 * ADLMIDI Library API:   Copyright (c) 2015-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Library is based on the ADLMIDI, a MIDI player for Linux and Windows with OPL3 emulation:
 * http://iki.fi/bisqwit/source/adlmidi.html
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADLDATA_H
#define ADLDATA_H

#include <string.h>
#include <stdint.h>
#include <cstring>

#pragma pack(push, 1)
#define ADLDATA_BYTE_COMPARABLE(T)                      \
    inline bool operator==(const T &a, const T &b)      \
    { return !memcmp(&a, &b, sizeof(T)); }              \
    inline bool operator!=(const T &a, const T &b)      \
    { return !operator==(a, b); }

struct adldata
{
    uint32_t    modulator_E862, carrier_E862;  // See below
    uint8_t     modulator_40, carrier_40; // KSL/attenuation settings
    uint8_t     feedconn; // Feedback/connection bits for the channel

    int8_t      finetune;
};
ADLDATA_BYTE_COMPARABLE(struct adldata)

struct adlinsdata
{
    enum { Flag_Pseudo4op = 0x01, Flag_NoSound = 0x02, Flag_Real4op = 0x04 };

    enum { Flag_RM_BassDrum  = 0x08, Flag_RM_Snare = 0x10, Flag_RM_TomTom = 0x18,
           Flag_RM_Cymbal = 0x20, Flag_RM_HiHat = 0x28, Mask_RhythmMode = 0x38 };

    uint16_t    adlno1, adlno2;
    uint8_t     tone;
    uint8_t     flags;
    uint16_t    ms_sound_kon;  // Number of milliseconds it produces sound;
    uint16_t    ms_sound_koff;
    int8_t      midi_velocity_offset;
    double      voice2_fine_tune;
};
ADLDATA_BYTE_COMPARABLE(struct adlinsdata)

enum { adlNoteOnMaxTime = 40000 };

/**
 * @brief Instrument data with operators included
 */
struct adlinsdata2
{
    adldata     adl[2];
    uint8_t     tone;
    uint8_t     flags;
    uint16_t    ms_sound_kon;  // Number of milliseconds it produces sound;
    uint16_t    ms_sound_koff;
    int8_t      midi_velocity_offset;
    double      voice2_fine_tune;
    static adlinsdata2 from_adldata(const adlinsdata &d);
};
ADLDATA_BYTE_COMPARABLE(struct adlinsdata2)

#undef ADLDATA_BYTE_COMPARABLE
#pragma pack(pop)

/**
 * @brief Bank global setup
 */
struct AdlBankSetup
{
    int     volumeModel;
    bool    deepTremolo;
    bool    deepVibrato;
    bool    scaleModulators;
};

#ifndef DISABLE_EMBEDDED_BANKS
int maxAdlBanks();
extern const adldata adl[];
extern const adlinsdata adlins[];
extern const unsigned short banks[][256];
extern const char* const banknames[];
extern const AdlBankSetup adlbanksetup[];
#endif

/**
 * @brief Conversion of storage formats
 */
inline adlinsdata2 adlinsdata2::from_adldata(const adlinsdata &d)
{
    adlinsdata2 ins;
    ins.tone = d.tone;
    ins.flags = d.flags;
    ins.ms_sound_kon = d.ms_sound_kon;
    ins.ms_sound_koff = d.ms_sound_koff;
    ins.midi_velocity_offset = d.midi_velocity_offset;
    ins.voice2_fine_tune = d.voice2_fine_tune;
#ifdef DISABLE_EMBEDDED_BANKS
    std::memset(ins.adl, 0, sizeof(adldata) * 2);
#else
    ins.adl[0] = ::adl[d.adlno1];
    ins.adl[1] = ::adl[d.adlno2];
#endif
    return ins;
}

/**
 * @brief Convert external instrument to internal instrument
 */
void cvt_ADLI_to_FMIns(adlinsdata2 &dst, const struct ADL_Instrument &src);

/**
 * @brief Convert internal instrument to external instrument
 */
void cvt_FMIns_to_ADLI(struct ADL_Instrument &dst, const adlinsdata2 &src);

#endif //ADLDATA_H
