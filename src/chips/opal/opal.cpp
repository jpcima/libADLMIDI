/*

    The Opal OPL3 emulator.

*/



#include "opal.hpp"



//--------------------------------------------------------------------------------------------------
static const uint8_t RateTables[4] = {
    0x55, // {   1, 0, 1, 0, 1, 0, 1, 0  }
    0x45, // {   1, 0, 1, 0, 0, 0, 1, 0  }
    0x11, // {   1, 0, 0, 0, 1, 0, 0, 0  }
    0x01, // {   1, 0, 0, 0, 0, 0, 0, 0  }
 };
//--------------------------------------------------------------------------------------------------
static const uint16_t ExpTable[0x100] = {
    2042, 2037, 2031, 2026, 2020, 2015, 2010, 2004, 1999, 1993, 1988, 1983, 1977, 1972, 1966, 1961,
    1956, 1951, 1945, 1940, 1935, 1930, 1924, 1919, 1914, 1909, 1904, 1898, 1893, 1888, 1883, 1878,
    1873, 1868, 1863, 1858, 1853, 1848, 1843, 1838, 1833, 1828, 1823, 1818, 1813, 1808, 1803, 1798,
    1794, 1789, 1784, 1779, 1774, 1769, 1765, 1760, 1755, 1750, 1746, 1741, 1736, 1732, 1727, 1722,
    1717, 1713, 1708, 1704, 1699, 1694, 1690, 1685, 1681, 1676, 1672, 1667, 1663, 1658, 1654, 1649,
    1645, 1640, 1636, 1631, 1627, 1623, 1618, 1614, 1609, 1605, 1601, 1596, 1592, 1588, 1584, 1579,
    1575, 1571, 1566, 1562, 1558, 1554, 1550, 1545, 1541, 1537, 1533, 1529, 1525, 1520, 1516, 1512,
    1508, 1504, 1500, 1496, 1492, 1488, 1484, 1480, 1476, 1472, 1468, 1464, 1460, 1456, 1452, 1448,
    1444, 1440, 1436, 1433, 1429, 1425, 1421, 1417, 1413, 1409, 1406, 1402, 1398, 1394, 1391, 1387,
    1383, 1379, 1376, 1372, 1368, 1364, 1361, 1357, 1353, 1350, 1346, 1342, 1339, 1335, 1332, 1328,
    1324, 1321, 1317, 1314, 1310, 1307, 1303, 1300, 1296, 1292, 1289, 1286, 1282, 1279, 1275, 1272,
    1268, 1265, 1261, 1258, 1255, 1251, 1248, 1244, 1241, 1238, 1234, 1231, 1228, 1224, 1221, 1218,
    1214, 1211, 1208, 1205, 1201, 1198, 1195, 1192, 1188, 1185, 1182, 1179, 1176, 1172, 1169, 1166,
    1163, 1160, 1157, 1154, 1150, 1147, 1144, 1141, 1138, 1135, 1132, 1129, 1126, 1123, 1120, 1117,
    1114, 1111, 1108, 1105, 1102, 1099, 1096, 1093, 1090, 1087, 1084, 1081, 1078, 1075, 1072, 1069,
    1066, 1064, 1061, 1058, 1055, 1052, 1049, 1046, 1044, 1041, 1038, 1035, 1032, 1030, 1027, 1024,
};
//--------------------------------------------------------------------------------------------------
static const uint16_t LogSinTable[0x100] = {
    2137, 1731, 1543, 1419, 1326, 1252, 1190, 1137, 1091, 1050, 1013,  979,  949,  920,  894,  869,
     846,  825,  804,  785,  767,  749,  732,  717,  701,  687,  672,  659,  646,  633,  621,  609,
     598,  587,  576,  566,  556,  546,  536,  527,  518,  509,  501,  492,  484,  476,  468,  461,
     453,  446,  439,  432,  425,  418,  411,  405,  399,  392,  386,  380,  375,  369,  363,  358,
     352,  347,  341,  336,  331,  326,  321,  316,  311,  307,  302,  297,  293,  289,  284,  280,
     276,  271,  267,  263,  259,  255,  251,  248,  244,  240,  236,  233,  229,  226,  222,  219,
     215,  212,  209,  205,  202,  199,  196,  193,  190,  187,  184,  181,  178,  175,  172,  169,
     167,  164,  161,  159,  156,  153,  151,  148,  146,  143,  141,  138,  136,  134,  131,  129,
     127,  125,  122,  120,  118,  116,  114,  112,  110,  108,  106,  104,  102,  100,   98,   96,
      94,   92,   91,   89,   87,   85,   83,   82,   80,   78,   77,   75,   74,   72,   70,   69,
      67,   66,   64,   63,   62,   60,   59,   57,   56,   55,   53,   52,   51,   49,   48,   47,
      46,   45,   43,   42,   41,   40,   39,   38,   37,   36,   35,   34,   33,   32,   31,   30,
      29,   28,   27,   26,   25,   24,   23,   23,   22,   21,   20,   20,   19,   18,   17,   17,
      16,   15,   15,   14,   13,   13,   12,   12,   11,   10,   10,    9,    9,    8,    8,    7,
       7,    7,    6,    6,    5,    5,    5,    4,    4,    4,    3,    3,    3,    2,    2,    2,
       2,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,    0,    0,    0,    0,    0,
};
//--------------------------------------------------------------------------------------------------
// Needs to be multiplied by two (and divided by two later when we use it) because the first
// entry is actually .5
static const uint8_t FrequencyMultiplierTableMul2[] = {
    1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30,
};
//--------------------------------------------------------------------------------------------------
static const uint8_t KeyScaleLevelTable[] = {
    0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,      0,      8,      12,     16,     20,     24,     28,     32,
    0,      0,      0,      0,      0,      12,     20,     28,     32,     40,     44,     48,     52,     56,     60,     64,
    0,      0,      0,      20,     32,     44,     52,     60,     64,     72,     76,     80,     84,     88,     92,     96,
    0,      0,      32,     52,     64,     76,     84,     92,     96,     104,    108,    112,    116,    120,    124,    128,
    0,      32,     64,     84,     96,     108,    116,    124,    128,    136,    140,    144,    148,    152,    156,    160,
    0,      64,     96,     116,    128,    140,    148,    156,    160,    168,    172,    176,    180,    184,    188,    192,
    0,      96,     128,    148,    160,    172,    180,    188,    192,    200,    204,    208,    212,    216,    220,    224,
};
//--------------------------------------------------------------------------------------------------
static const int8_t OperatorLookupTable[] = {
//  00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F
    0,  1,  2,  3,  4,  5,  -1, -1, 6,  7,  8,  9,  10, 11, -1, -1,
//  10  11  12  13  14  15  16  17  18  19  1A  1B  1C  1D  1E  1F
    12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};
//--------------------------------------------------------------------------------------------------
static const uint16_t PanLawTable[128] =
{
    65535, 65529, 65514, 65489, 65454, 65409, 65354, 65289,
    65214, 65129, 65034, 64929, 64814, 64689, 64554, 64410,
    64255, 64091, 63917, 63733, 63540, 63336, 63123, 62901,
    62668, 62426, 62175, 61914, 61644, 61364, 61075, 60776,
    60468, 60151, 59825, 59489, 59145, 58791, 58428, 58057,
    57676, 57287, 56889, 56482, 56067, 55643, 55211, 54770,
    54320, 53863, 53397, 52923, 52441, 51951, 51453, 50947,
    50433, 49912, 49383, 48846, 48302, 47750, 47191,
    46340, // Center left
    46340, // Center right
    45472, 44885, 44291, 43690, 43083, 42469, 41848, 41221,
    40588, 39948, 39303, 38651, 37994, 37330, 36661, 35986,
    35306, 34621, 33930, 33234, 32533, 31827, 31116, 30400,
    29680, 28955, 28225, 27492, 26754, 26012, 25266, 24516,
    23762, 23005, 22244, 21480, 20713, 19942, 19169, 18392,
    17613, 16831, 16046, 15259, 14469, 13678, 12884, 12088,
    11291, 10492, 9691, 8888, 8085, 7280, 6473, 5666,
    4858, 4050, 3240, 2431, 1620, 810, 0
};



//==================================================================================================
// This is the temporary code for generating the above tables.  Maths and data from this nice
// reverse-engineering effort:
//
// https://docs.google.com/document/d/18IGx18NQY_Q1PJVZ-bHywao9bhsDoAqoIn1rIm42nwo/edit
//==================================================================================================
#if 0
#include <math.h>

void GenerateTables() {

    // Build the exponentiation table (reversed from the official OPL3 ROM)
    FILE *fd = fopen("exptab.txt", "wb");
    if (fd) {
        for (int i = 0; i < 0x100; i++) {
            int v = (pow(2, (0xFF - i) / 256.0) - 1) * 1024 + 0.5;
            if (i & 15)
                fprintf(fd, " %4d,", v);
            else
                fprintf(fd, "\n\t%4d,", v);
        }
        fclose(fd);
    }

    // Build the log-sin table
    fd = fopen("sintab.txt", "wb");
    if (fd) {
        for (int i = 0; i < 0x100; i++) {
            int v = -log(sin((i + 0.5) * 3.1415926535897933 / 256 / 2)) / log(2) * 256 + 0.5;
            if (i & 15)
                fprintf(fd, " %4d,", v);
            else
                fprintf(fd, "\n\t%4d,", v);
        }
        fclose(fd);
    }
}
#endif



//==================================================================================================
// Constructor/destructor.
//==================================================================================================
Opal::Opal(int sample_rate) {

    Init(sample_rate);
}
//--------------------------------------------------------------------------------------------------
Opal::~Opal() {
}



//==================================================================================================
// Initialise the emulation.
//==================================================================================================
void Opal::Init(int sample_rate) {

    Clock = 0;
    TremoloClock = 0;
    TremoloLevel = 0;
    VibratoTick = 0;
    VibratoClock = 0;
    NoteSel = false;
    TremoloDepth = false;
    VibratoDepth = false;

//  // Build the exponentiation table (reversed from the official OPL3 ROM)
//  for (int i = 0; i < 0x100; i++)
//      ExpTable[i] = (pow(2, (0xFF - i) / 256.0) - 1) * 1024 + 0.5;
//
//  // Build the log-sin table
//  for (int i = 0; i < 0x100; i++)
//      LogSinTable[i] = -log(sin((i + 0.5) * 3.1415926535897933 / 256 / 2)) / log(2) * 256 + 0.5;

    // Let sub-objects know where to find us
    for (int i = 0; i < NumOperators; i++)
        Op[i].SetMaster(this);

    for (int i = 0; i < NumChannels; i++)
        Chan[i].SetMaster(this);

    // Add the operators to the channels.  Note, some channels can't use all the operators
    // FIXME: put this into a separate routine
    const int8_t chan_ops[] = {
        0, 1, 2, 6, 7, 8, 12, 13, 14, 18, 19, 20, 24, 25, 26, 30, 31, 32,
    };

    for (int i = 0; i < NumChannels; i++) {
        Channel *chan = &Chan[i];
        int8_t op = chan_ops[i];
        if (i < 3 || (i >= 9 && i < 12))
            chan->SetOperators(op, op + 6);
        else
            chan->SetOperators(op, -1);
    }

    // Initialise the operator rate data.  We can't do this in the Operator constructor as it
    // relies on referencing the master and channel objects
    for (int i = 0; i < NumOperators; i++)
        Op[i].ComputeRates();

    // Initialise channel panning at center.
    for (int i = 0; i < NumChannels; i++)
        Chan[i].SetPan(64);

    SetSampleRate(sample_rate);
}



//==================================================================================================
// Change the sample rate.
//==================================================================================================
void Opal::SetSampleRate(int sample_rate) {

    // Sanity
    if (sample_rate == 0)
        sample_rate = OPL3SampleRate;

    SampleRate = sample_rate;
    SampleAccum = 0;
    LastOutput[0] = LastOutput[1] = 0;
    CurrOutput[0] = CurrOutput[1] = 0;
}



//==================================================================================================
// Write a value to an OPL3 register.
//==================================================================================================
void Opal::Port(uint16_t reg_num, uint8_t val) {

    uint16_t type = reg_num & 0xE0;

    // Is it BD, the one-off register stuck in the middle of the register array?
    if (reg_num == 0xBD) {
        TremoloDepth = (val & 0x80) != 0;
        VibratoDepth = (val & 0x40) != 0;
        return;
    }

    // Global registers
    if (type == 0x00) {

        // 4-OP enables
        if (reg_num == 0x104) {

            // Enable/disable channels based on which 4-op enables
            uint8_t mask = 1;
            for (int i = 0; i < 6; i++, mask <<= 1) {

                // The 4-op channels are 0, 1, 2, 9, 10, 11
                uint16_t chan = i < 3 ? i : i + 6;
                Channel *primary = &Chan[chan];
                Channel *secondary = &Chan[chan + 3];

                if (val & mask) {

                    // Let primary channel know it's controlling the secondary channel
                    primary->SetFourOp(true);

                    // Turn off the second channel in the pair
                    secondary->SetEnable(false);

                } else {

                    // Let primary channel know it's no longer controlling the secondary channel
                    primary->SetFourOp(false);

                    // Turn on the second channel in the pair
                    secondary->SetEnable(true);
                }
            }

        // CSW / Note-sel
        } else if (reg_num == 0x08) {

            NoteSel = (val & 0x40) != 0;

            // Get the channels to recompute the Key Scale No. as this varies based on NoteSel
            for (int i = 0; i < NumChannels; i++)
                Chan[i].ComputeKeyScaleNumber();
        }

    // Channel registers
    } else if (type >= 0xA0 && type <= 0xC0) {

        // Convert to channel number
        int chan_num = reg_num & 15;

        // Valid channel?
        if (chan_num >= 9)
            return;

        // Is it the other bank of channels?
        if (reg_num & 0x100)
            chan_num += 9;

        Channel &chan = Chan[chan_num];

        /* libADLMIDI: registers Ax and Cx affect both channels */
        Channel *chans[2] = {&chan, &chan + 3};
        unsigned numchans = chan.IsFourOp() ? 2 : 1;

        // Do specific registers
        switch (reg_num & 0xF0) {

            // Frequency low
            case 0xA0: {
                for (unsigned i = 0; i < numchans; ++i) { /* libADLMIDI */
                    chans[i]->SetFrequencyLow(val);
                }
                break;
            }

            // Key-on / Octave / Frequency High
            case 0xB0: {
                for (unsigned i = 0; i < numchans; ++i) { /* libADLMIDI */
                    chans[i]->SetKeyOn((val & 0x20) != 0);
                    chans[i]->SetOctave(val >> 2 & 7);
                    chans[i]->SetFrequencyHigh(val & 3);
                }
                break;
            }

            // Right Stereo Channel Enable / Left Stereo Channel Enable / Feedback Factor / Modulation Type
            case 0xC0: {
                chan.SetRightEnable((val & 0x20) != 0);
                chan.SetLeftEnable((val & 0x10) != 0);
                chan.SetFeedback(val >> 1 & 7);
                chan.SetModulationType(val & 1);
                break;
            }
        }

    // Operator registers
    } else if ((type >= 0x20 && type <= 0x80) || type == 0xE0) {

        // Convert to operator number
        int op_num = OperatorLookupTable[reg_num & 0x1F];

        // Valid register?
        if (op_num < 0)
            return;

        // Is it the other bank of operators?
        if (reg_num & 0x100)
            op_num += 18;

        Operator &op = Op[op_num];

        // Do specific registers
        switch (type) {

            // Tremolo Enable / Vibrato Enable / Sustain Mode / Envelope Scaling / Frequency Multiplier
            case 0x20: {
                op.SetTremoloEnable((val & 0x80) != 0);
                op.SetVibratoEnable((val & 0x40) != 0);
                op.SetSustainMode((val & 0x20) != 0);
                op.SetEnvelopeScaling((val & 0x10) != 0);
                op.SetFrequencyMultiplier(val & 15);
                break;
            }

            // Key Scale / Output Level
            case 0x40: {
                op.SetKeyScale(val >> 6);
                op.SetOutputLevel(val & 0x3F);
                break;
            }

            // Attack Rate / Decay Rate
            case 0x60: {
                op.SetAttackRate(val >> 4);
                op.SetDecayRate(val & 15);
                break;
            }

            // Sustain Level / Release Rate
            case 0x80: {
                op.SetSustainLevel(val >> 4);
                op.SetReleaseRate(val & 15);
                break;
            }

            // Waveform
            case 0xE0: {
                op.SetWaveform(val & 7);
                break;
            }
        }
    }
}



//==================================================================================================
// Set panning on the channel designated by the register number.
// This is extended functionality.
//==================================================================================================
void Opal::Pan(uint16_t reg_num, uint8_t pan)
{
    uint8_t high = (reg_num >> 8) & 1;
    uint8_t regm = reg_num & 0xff;
    Chan[9 * high + (regm & 0x0f)].SetPan(pan);
}



//==================================================================================================
// Generate sample.  Every time you call this you will get two signed 16-bit samples (one for each
// stereo channel) which will sound correct when played back at the sample rate given when the
// class was constructed.
//==================================================================================================
void Opal::Sample(int16_t *frame) {

    // If the destination sample rate is higher than the OPL3 sample rate, we need to skip ahead
    while (SampleAccum >= SampleRate) {

        LastOutput[0] = CurrOutput[0];
        LastOutput[1] = CurrOutput[1];

        Output(CurrOutput);

        SampleAccum -= SampleRate;
    }

    // Mix with the partial accumulation
    int32_t omblend = SampleRate - SampleAccum;
    frame[0] = (LastOutput[0] * omblend + CurrOutput[0] * SampleAccum) / SampleRate;
    frame[1] = (LastOutput[1] * omblend + CurrOutput[1] * SampleAccum) / SampleRate;

    SampleAccum += OPL3SampleRate;
}



//==================================================================================================
// Produce final output from the chip.  This is at the OPL3 sample-rate.
//==================================================================================================
void Opal::Output(int16_t *frame) {

    int32_t leftmix = 0, rightmix = 0;

    // Sum the output of each channel
    for (int i = 0; i < NumChannels; i++) {

        int16_t chan_frame[2];
        Chan[i].Output(chan_frame);

        leftmix += chan_frame[0];
        rightmix += chan_frame[1];
    }

    // Clamp
    int16_t left;
    if (leftmix < -0x8000)
        left = -0x8000;
    else if (leftmix > 0x7FFF)
        left = 0x7FFF;
    else
        left = leftmix;

    int16_t right;
    if (rightmix < -0x8000)
        right = -0x8000;
    else if (rightmix > 0x7FFF)
        right = 0x7FFF;
    else
        right = rightmix;

    Clock++;

    // Tremolo.  According to this post, the OPL3 tremolo is a 13,440 sample length triangle wave
    // with a peak at 26 and a trough at 0 and is simply added to the logarithmic level accumulator
    //      http://forums.submarine.org.uk/phpBB/viewtopic.php?f=9&t=1171
    TremoloClock = (TremoloClock + 1) % 13440;
    TremoloLevel = ((TremoloClock < 13440 / 2) ? TremoloClock : 13440 - TremoloClock) / 256;
    if (!TremoloDepth)
        TremoloLevel >>= 2;

    // Vibrato.  This appears to be a 8 sample long triangle wave with a magnitude of the three
    // high bits of the channel frequency, positive and negative, divided by two if the vibrato
    // depth is zero.  It is only cycled every 1,024 samples.
    VibratoTick++;
    if (VibratoTick >= 1024) {
        VibratoTick = 0;
        VibratoClock = (VibratoClock + 1) & 7;
    }

    frame[0] = left;
    frame[1] = right;
}



//==================================================================================================
// Channel constructor.
//==================================================================================================
Opal::Channel::Channel() {

    Master = 0;
    Freq = 0;
    Octave = 0;
    PhaseStep = 0;
    KeyScaleNumber = 0;
    FeedbackShift = 0;
    ModulationType = 0;
    FourOp = 0;
    Enable = true;
}

//==================================================================================================
// Associate operators with the channel.
//==================================================================================================
void Opal::Channel::SetOperators(int8_t pair1, int8_t pair2) {

    OpPair1 = pair1;
    OpPair2 = pair2;

    Operator &op1 = Master->Op[pair1];
    Operator &op2 = *(&op1 + 3);
    Operator &op3 = *(&op2 + 3);
    Operator &op4 = *(&op3 + 3);

    op1.SetChannel(this);
    op2.SetChannel(this);
    if (pair2 != -1) {
        op3.SetChannel(this);
        op4.SetChannel(this);
    }
}



//==================================================================================================
// Produce output from channel.
//==================================================================================================
void Opal::Channel::Output(int16_t *frame) {

    // Has the channel been disabled?  This is usually a result of the 4-op enables being used to
    // disable the secondary channel in each 4-op pair
    if (!Enable) {
        frame[0] = frame[1] = 0;
        return;
    }

    Opal *master = Master;

    int16_t vibrato = (Freq >> 7) & 7;
    vibrato >>= !master->VibratoDepth;

    // 0  3  7  3  0  -3  -7  -3
    uint16_t clk = master->VibratoClock;
    vibrato >>= clk & 1;                      // Odd positions are half the magnitude
    vibrato = (clk & 4) ? -vibrato : vibrato; // The second half positions are negative
    vibrato = (clk & 3) ? vibrato : 0;        // Position 0 and 4 is zero

    vibrato <<= Octave;

    // Running in 4-op mode?
    bool is4op = FourOp;

    Operator &op1 = master->Op[OpPair1];
    Operator &op2 = *(&op1 + 3);
    Operator &op3 = *(&op1 + (is4op ? 6 : 0));
    Operator &op4 = *(&op2 + (is4op ? 6 : 0));
    Channel &ch2 = *(this + (is4op ? 3 : 0));

    /* uint16_t key_scale_number = KeyScaleNumber; */
    uint32_t phase_step = PhaseStep;
    int conn = ModulationType;
    conn |= is4op << 2;
    conn |= (is4op & ch2.ModulationType) << 1;

    int16_t vib1 = op1.GetVibratoEnable() ? vibrato : 0;
    int16_t vib2 = op2.GetVibratoEnable() ? vibrato : 0;
    int16_t vib3 = op3.GetVibratoEnable() ? vibrato : 0;
    int16_t vib4 = op4.GetVibratoEnable() ? vibrato : 0;

    // Feedback?  In that case we modulate by a blend of the last two samples
    int16_t feedback = op1.GetFeedback(FeedbackShift);

    // Combine individual operator outputs
    int16_t out = 0;
    int16_t acc;

    switch (conn) {

        case 0:

            // Frequency modulation (well, phase modulation technically)
            out = op1.Output(/* key_scale_number, */ phase_step + vib1, feedback);
            out = op2.Output(/* key_scale_number, */ phase_step + vib2, out);
            break;

        case 1:

            // Additive
            out  = op1.Output(/* key_scale_number, */ phase_step + vib1, feedback);
            out += op2.Output(/* key_scale_number, */ phase_step + vib2, 0);
            break;

        case 4:

           // feedback -> modulator -> modulator -> modulator -> carrier
           out  = op1.Output(/* key_scale_number, */ phase_step + vib1, feedback);
           out  = op2.Output(/* key_scale_number, */ phase_step + vib2, out);
           out  = op3.Output(/* key_scale_number, */ phase_step + vib3, out);
           out  = op4.Output(/* key_scale_number, */ phase_step + vib4, out);
           break;

        case 5:

            // (feedback -> carrier) + (modulator -> modulator -> carrier)
            out  = op1.Output(/* key_scale_number, */ phase_step + vib1, feedback);
            acc  = op2.Output(/* key_scale_number, */ phase_step + vib2, 0);
            acc  = op3.Output(/* key_scale_number, */ phase_step + vib3, acc);
            out += op4.Output(/* key_scale_number, */ phase_step + vib4, acc);
            break;

        case 6:

            // (feedback -> modulator -> carrier) + (modulator -> carrier)
            out  = op1.Output(/* key_scale_number, */ phase_step + vib1, feedback);
            out  = op2.Output(/* key_scale_number, */ phase_step + vib2, out);
            acc  = op3.Output(/* key_scale_number, */ phase_step + vib3, 0);
            out += op4.Output(/* key_scale_number, */ phase_step + vib4, acc);
            break;

        case 7:

            // (feedback -> carrier) + (modulator -> carrier) + carrier
            out  = op1.Output(/* key_scale_number, */ phase_step + vib1, feedback);
            acc  = op2.Output(/* key_scale_number, */ phase_step + vib2, 0);
            out += op3.Output(/* key_scale_number, */ phase_step + vib3, acc);
            out += op4.Output(/* key_scale_number, */ phase_step + vib4, 0);
            break;
    }

    int16_t left = LeftEnable ? out : 0;
    int16_t right = RightEnable ? out : 0;

    left = left * LeftPan / 65536;
    right = right * RightPan / 65536;

    frame[0] = left;
    frame[1] = right;
}



//==================================================================================================
// Set phase step for operators using this channel.
//==================================================================================================
void Opal::Channel::SetFrequencyLow(uint8_t freq) {

    Freq = (Freq & 0x300) | (freq & 0xFF);
    ComputePhaseStep();
}
//--------------------------------------------------------------------------------------------------
void Opal::Channel::SetFrequencyHigh(uint8_t freq) {

    Freq = (Freq & 0xFF) | ((freq & 3) << 8);
    ComputePhaseStep();

    // Only the high bits of Freq affect the Key Scale No.
    ComputeKeyScaleNumber();
}



//==================================================================================================
// Set the octave of the channel (0 to 7).
//==================================================================================================
void Opal::Channel::SetOctave(uint8_t oct) {

    Octave = oct & 7;
    ComputePhaseStep();
    ComputeKeyScaleNumber();
}



//==================================================================================================
// Keys the channel on/off.
//==================================================================================================
void Opal::Channel::SetKeyOn(bool on) {

    Operator &op1 = Master->Op[OpPair1];
    Operator &op2 = *(&op1 + 3);
    op1.SetKeyOn(on);
    op2.SetKeyOn(on);
}



//==================================================================================================
// Enable left stereo channel.
//==================================================================================================
void Opal::Channel::SetLeftEnable(bool on) {

    LeftEnable = on;
}



//==================================================================================================
// Enable right stereo channel.
//==================================================================================================
void Opal::Channel::SetRightEnable(bool on) {

    RightEnable = on;
}



//==================================================================================================
// Pan the channel to the position given.
//==================================================================================================
void Opal::Channel::SetPan(uint8_t pan)
{
    pan &= 127;
    LeftPan = PanLawTable[pan];
    RightPan = PanLawTable[127 - pan];
}



//==================================================================================================
// Set the channel feedback amount.
//==================================================================================================
void Opal::Channel::SetFeedback(uint8_t val) {

    FeedbackShift = val ? 9 - val : 15;
}



//==================================================================================================
// Set frequency modulation/additive modulation
//==================================================================================================
void Opal::Channel::SetModulationType(uint8_t type) {

    ModulationType = type;
}



//==================================================================================================
// Compute the stepping factor for the operator waveform phase based on the frequency and octave
// values of the channel.
//==================================================================================================
void Opal::Channel::ComputePhaseStep() {

    PhaseStep = uint32_t(Freq) << Octave;
}



//==================================================================================================
// Compute the key scale number and key scale levels.
//
// From the Yamaha data sheet this is the block/octave number as bits 3-1, with bit 0 coming from
// the MSB of the frequency if NoteSel is 1, and the 2nd MSB if NoteSel is 0.
//==================================================================================================
void Opal::Channel::ComputeKeyScaleNumber() {

    Opal *master = Master;

    uint16_t lsb = master->NoteSel ? Freq >> 9 : (Freq >> 8) & 1;
    KeyScaleNumber = Octave << 1 | lsb;

    // Get the channel operators to recompute their rates as they're dependent on this number.  They
    // also need to recompute their key scale level
    Operator *op = &master->Op[OpPair1];
    int num_ops = (OpPair2 == -1) ? 2 : 4;

    for (int i = 0; i < num_ops; ++i) {
        op[3 * i].ComputeRates();
        op[3 * i].ComputeKeyScaleLevel();
    }
}



//==================================================================================================
// Operator constructor.
//==================================================================================================
Opal::Operator::Operator() {

    Master = 0;
    Chan = 0;
    Phase = 0;
    Waveform = 0;
    FreqMultTimes2 = 1;
    EnvelopeStage = EnvOff;
    EnvelopeLevel = 0x1FF;
    AttackRate = 0;
    DecayRate = 0;
    SustainLevel = 0;
    ReleaseRate = 0;
    KeyScaleShift = 0;
    KeyScaleLevel = 0;
    Out[0] = Out[1] = 0;
    KeyOn = false;
    KeyScaleRate = false;
    SustainMode = false;
    TremoloEnable = false;
    VibratoEnable = false;
}



//==================================================================================================
// Produce output from operator.
//==================================================================================================
int16_t Opal::Operator::Output(/* uint16_t keyscalenum, */ uint32_t phase_step, int16_t mod) {

    // Advance wave phase
    uint32_t phase_counter = Phase + (phase_step * FreqMultTimes2) / 2;
    Phase = phase_counter;

    uint16_t level = EnvelopeLevel;

    if (!TickEnvelope()) {
        Out[0] = Out[1] = 0;
        return 0;
    }

    level += OutputLevel + KeyScaleLevel + (TremoloEnable ? Master->TremoloLevel : 0);
    level <<= 3;

    uint16_t phase = (phase_counter >> 10) + mod;
    uint16_t offset = phase & 0xFF;
    uint16_t logsin;
    uint16_t negate = 0;

    switch (Waveform) {

        //------------------------------------
        // Standard sine wave
        //------------------------------------
        case 0:
            offset ^= (phase & 0x100) ? 0xFF : 0;
            logsin = LogSinTable[offset];
            negate = (phase & 0x200) ? 0xFFFF : 0;
            break;

        //------------------------------------
        // Half sine wave
        //------------------------------------
        case 1:
            offset ^= (phase & 0x100) ? 0xFF : 0;
            offset = (phase & 0x200) ? 0 : offset;
            logsin = LogSinTable[offset];
            break;

        //------------------------------------
        // Positive sine wave
        //------------------------------------
        case 2:
            offset ^= (phase & 0x100) ? 0xFF : 0;
            logsin = LogSinTable[offset];
            break;

        //------------------------------------
        // Quarter positive sine wave
        //------------------------------------
        case 3:
            offset = (phase & 0x100) ? 0 : offset;
            logsin = LogSinTable[offset];
            break;

        //------------------------------------
        // Double-speed sine wave
        //------------------------------------
        case 4:
            offset ^= (phase & 0x80) ? 0xFF : 0;

            offset = (offset + offset) & 0xFF;
            negate = (phase & 0x100) ? 0xFFFF : 0;

            offset = (phase & 0x200) ? 0 : offset;
            negate = (phase & 0x200) ? 0 : negate;

            logsin = LogSinTable[offset];
            break;

        //------------------------------------
        // Double-speed positive sine wave
        //------------------------------------
        case 5:
            offset = (offset + offset) & 0xFF;
            offset ^= (phase & 0x80) ? 0xFF : 0;

            offset = (phase & 0x200) ? 0 : offset;

            logsin = LogSinTable[offset];
            break;

        //------------------------------------
        // Square wave
        //------------------------------------
        case 6:
            logsin = 0;
            negate = (phase & 0x200) ? 0xFFFF : 0;
            break;

        //------------------------------------
        // Exponentiation wave
        //------------------------------------
        default:
            logsin = phase & 0x1FF;
            logsin ^= (phase & 0x200) ? 0x1FF : 0;
            negate = (phase & 0x200) ? 0xFFFF : 0;
            logsin <<= 3;
            break;
    }

    uint16_t mix = logsin + level;
    if (mix > 0x1FFF)
        mix = 0x1FFF;

    // From the OPLx decapsulated docs:
    // "When such a table is used for calculation of the exponential, the table is read at the
    // position given by the 8 LSB's of the input. The value + 1024 (the hidden bit) is then the
    // significand of the floating point output and the yet unused MSB's of the input are the
    // exponent of the floating point output."
    int16_t v = ExpTable[mix & 0xFF];
    v >>= mix >> 8;
    v += v;
    v ^= negate;

    // Keep last two results for feedback calculation
    Out[1] = Out[0];
    Out[0] = v;

    return v;
}



bool Opal::Operator::TickEnvelope()
{
    bool active = true;
    int16_t level = EnvelopeLevel;
    int stage = EnvelopeStage;

    int gen = (!SustainMode && stage == EnvSus) ? EnvRel : stage;

    switch (gen) {

        // Attack stage
        case EnvAtt: {
            uint16_t clock = Master->Clock;
            uint16_t add = ((AttackAdd >> AttackTabAt(clock >> AttackShift & 7)) * ~level) >> 3;
            add = AttackRate ? add : 0;
            add = (clock & AttackMask) ? 0 : add;
            level += add;
            bool stay = level > 0;
            level = stay ? level : 0;
            stage = stay ? stage : EnvDec;
            break;
        }

        // Decay stage
        case EnvDec: {
            uint16_t clock = Master->Clock;
            uint16_t add = DecayAdd >> DecayTabAt(clock >> DecayShift & 7);
            add = DecayRate ? add : 0;
            add = (clock & DecayMask) ? 0 : add;
            level += add;
            bool stay = level < SustainLevel;
            level = stay ? level : SustainLevel;
            stage = stay ? stage : EnvSus;
            break;
        }

        // Sustain stage
        case EnvSus:
            break;

        // Release stage
        case EnvRel: {
            uint16_t clock = Master->Clock;
            uint16_t add = ReleaseAdd >> ReleaseTabAt(clock >> ReleaseShift & 7);
            add = ReleaseRate ? add : 0;
            add = (clock & ReleaseMask) ? 0 : add;
            level += add;
            active = level < 0x1FF;
            level = active ? level : 0x1FF;
            stage = active ? stage : EnvOff;
            break;
        }

        // Envelope, and therefore the operator, is not running
        default:
            active = false;
    }

    EnvelopeLevel = level;
    EnvelopeStage = stage;
    return active;
}



//==================================================================================================
// Trigger operator.
//==================================================================================================
void Opal::Operator::SetKeyOn(bool on) {

    // Already on/off?
    if (KeyOn == on)
        return;
    KeyOn = on;

    if (on) {

        // The highest attack rate is instant; it bypasses the attack phase
        if (AttackRate == 15) {
            EnvelopeStage = EnvDec;
            EnvelopeLevel = 0;
        } else
            EnvelopeStage = EnvAtt;

        Phase = 0;

    } else {

        // Stopping current sound?
        if (EnvelopeStage != EnvOff && EnvelopeStage != EnvRel)
            EnvelopeStage = EnvRel;
    }
}



//==================================================================================================
// Enable amplitude vibrato.
//==================================================================================================
void Opal::Operator::SetTremoloEnable(bool on) {

    TremoloEnable = on;
}



//==================================================================================================
// Enable frequency vibrato.
//==================================================================================================
void Opal::Operator::SetVibratoEnable(bool on) {

    VibratoEnable = on;
}



//==================================================================================================
// Sets whether we release or sustain during the sustain phase of the envelope.  'true' is to
// sustain, otherwise release.
//==================================================================================================
void Opal::Operator::SetSustainMode(bool on) {

    SustainMode = on;
}



//==================================================================================================
// Key scale rate.  Sets how much the Key Scaling Number affects the envelope rates.
//==================================================================================================
void Opal::Operator::SetEnvelopeScaling(bool on) {

    KeyScaleRate = on;
    ComputeRates();
}



//==================================================================================================
// Multiplies the phase frequency.
//==================================================================================================
void Opal::Operator::SetFrequencyMultiplier(uint8_t scale) {

    FreqMultTimes2 = FrequencyMultiplierTableMul2[scale & 15];
}



//==================================================================================================
// Attenuates output level towards higher pitch.
//==================================================================================================
void Opal::Operator::SetKeyScale(uint8_t scale) {

    /* libADLMIDI: KSL computation fix */
    const uint8_t KeyScaleShiftTable[4] = {8, 1, 2, 0};
    KeyScaleShift = KeyScaleShiftTable[scale];

    ComputeKeyScaleLevel();
}



//==================================================================================================
// Sets the output level (volume) of the operator.
//==================================================================================================
void Opal::Operator::SetOutputLevel(uint8_t level) {

    OutputLevel = level * 4;
}



//==================================================================================================
// Operator attack rate.
//==================================================================================================
void Opal::Operator::SetAttackRate(uint8_t rate) {

    AttackRate = rate;

    ComputeRates();
}



//==================================================================================================
// Operator decay rate.
//==================================================================================================
void Opal::Operator::SetDecayRate(uint8_t rate) {

    DecayRate = rate;

    ComputeRates();
}



//==================================================================================================
// Operator sustain level.
//==================================================================================================
void Opal::Operator::SetSustainLevel(uint8_t level) {

    SustainLevel = level < 15 ? level : 31;
    SustainLevel *= 16;
}



//==================================================================================================
// Operator release rate.
//==================================================================================================
void Opal::Operator::SetReleaseRate(uint8_t rate) {

    ReleaseRate = rate;

    ComputeRates();
}



//==================================================================================================
// Assign the waveform this operator will use.
//==================================================================================================
void Opal::Operator::SetWaveform(uint8_t wave) {

    Waveform = wave & 7;
}



//==================================================================================================
// Compute actual rate from register rate.  From the Yamaha data sheet:
//
// Actual rate = Rate value * 4 + Rof, if Rate value = 0, actual rate = 0
//
// Rof is set as follows depending on the KSR setting:
//
//  Key scale   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
//  KSR = 0     0   0   0   0   1   1   1   1   2   2   2   2   3   3   3   3
//  KSR = 1     0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
//
// Note: zero rates are infinite, and are treated separately elsewhere
//==================================================================================================
void Opal::Operator::ComputeRates() {

    int combined_rate = AttackRate * 4 + (Chan->GetKeyScaleNumber() >> (KeyScaleRate ? 0 : 2));
    int rate_high = combined_rate >> 2;
    int rate_low = combined_rate & 3;

    AttackShift = rate_high < 12 ? 12 - rate_high : 0;
    AttackMask = (1 << AttackShift) - 1;
    AttackAdd = (rate_high < 12) ? 1 : 1 << (rate_high - 12);
    AttackTab = RateTables[rate_low];

    // Attack rate of 15 is always instant
    if (AttackRate == 15)
        AttackAdd = 0xFFF;

    combined_rate = DecayRate * 4 + (Chan->GetKeyScaleNumber() >> (KeyScaleRate ? 0 : 2));
    rate_high = combined_rate >> 2;
    rate_low = combined_rate & 3;

    DecayShift = rate_high < 12 ? 12 - rate_high : 0;
    DecayMask = (1 << DecayShift) - 1;
    DecayAdd = (rate_high < 12) ? 1 : 1 << (rate_high - 12);
    DecayTab = RateTables[rate_low];

    combined_rate = ReleaseRate * 4 + (Chan->GetKeyScaleNumber() >> (KeyScaleRate ? 0 : 2));
    rate_high = combined_rate >> 2;
    rate_low = combined_rate & 3;

    ReleaseShift = rate_high < 12 ? 12 - rate_high : 0;
    ReleaseMask = (1 << ReleaseShift) - 1;
    ReleaseAdd = (rate_high < 12) ? 1 : 1 << (rate_high - 12);
    ReleaseTab = RateTables[rate_low];
}



//==================================================================================================
// Compute the operator's key scale level.  This changes based on the channel frequency/octave and
// operator key scale value.
//==================================================================================================
void Opal::Operator::ComputeKeyScaleLevel() {

    // This uses a combined value of the top four bits of frequency with the octave/block
    uint8_t i = (Chan->GetOctave() << 4) | (Chan->GetFreq() >> 6);
    KeyScaleLevel = KeyScaleLevelTable[i] >> KeyScaleShift;
}
