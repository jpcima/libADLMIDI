/*

    The Opal OPL3 emulator.

    Note: this is not a complete emulator, just enough for Reality Adlib Tracker tunes.

    Missing features compared to a real OPL3:

        - Timers/interrupts
        - OPL3 enable bit (it defaults to always on)
        - CSW mode
        - Test register
        - Percussion mode

*/
#ifndef OPAL_HPP
#define OPAL_HPP

#define OPAL_HAVE_SOFT_PANNING 1 /* libADLMIDI */



#include <stdint.h>



//==================================================================================================
// Opal class.
//==================================================================================================
class Opal {

    class Channel;

    // Various constants
    enum {
        OPL3SampleRate      = 49716,
        NumChannels         = 18,
        NumOperators        = 36,

        EnvOff              = -1,
        EnvAtt,
        EnvDec,
        EnvSus,
        EnvRel
    };

    // A single FM operator
    class Operator {

        public:
                            Operator();
            void            SetMaster(Opal *opal) {  Master = opal;  }
            void            SetChannel(Channel *chan) {  Chan = chan;  }

            int16_t         Output(/* uint16_t keyscalenum, */ uint32_t phase_step, int16_t mod);
            bool            TickEnvelope();

            bool            AttackTabAt(unsigned index) const {  return (AttackTab & (1u << index)) != 0;  }
            bool            DecayTabAt(unsigned index) const {  return (DecayTab & (1u << index)) != 0;  }
            bool            ReleaseTabAt(unsigned index) const {  return (ReleaseTab & (1u << index)) != 0;  }

            void            SetKeyOn(bool on);
            void            SetTremoloEnable(bool on);
            void            SetVibratoEnable(bool on);
            void            SetSustainMode(bool on);
            void            SetEnvelopeScaling(bool on);
            void            SetFrequencyMultiplier(uint8_t scale);
            void            SetKeyScale(uint8_t scale);
            void            SetOutputLevel(uint8_t level);
            void            SetAttackRate(uint8_t rate);
            void            SetDecayRate(uint8_t rate);
            void            SetSustainLevel(uint8_t level);
            void            SetReleaseRate(uint8_t rate);
            void            SetWaveform(uint8_t wave);

            void            ComputeRates();
            void            ComputeKeyScaleLevel();

            int16_t         GetFeedback(uint8_t fbshift) const {  return (Out[0] + Out[1]) >> fbshift;  }
            uint8_t         GetVibratoEnable() const {  return VibratoEnable;  }

        protected:
            Opal *          Master;             // Master object
            Channel *       Chan;               // Owning channel
            uint32_t        Phase;              // The current offset in the selected waveform
            uint8_t         Waveform;           // The waveform id this operator is using
            uint8_t         FreqMultTimes2;     // Frequency multiplier * 2
            int             EnvelopeStage;      // Which stage the envelope is at (see Env* enums above)
            int16_t         EnvelopeLevel;      // 0 - $1FF, 0 being the loudest
            uint8_t         OutputLevel;        // 0 - $FF
            uint8_t         AttackRate;
            uint8_t         DecayRate;
            uint8_t         SustainLevel;
            uint8_t         ReleaseRate;
            uint8_t         AttackShift;
            uint16_t        AttackMask;
            uint16_t        AttackAdd;
            uint8_t         DecayShift;
            uint16_t        DecayMask;
            uint16_t        DecayAdd;
            uint8_t         ReleaseShift;
            uint16_t        ReleaseMask;
            uint16_t        ReleaseAdd;
            uint8_t         AttackTab;
            uint8_t         DecayTab;
            uint8_t         ReleaseTab;
            uint8_t         KeyScaleShift;
            uint8_t         KeyScaleLevel;
            int16_t         Out[2];
            struct {
            uint8_t         KeyOn : 1;
            uint8_t         KeyScaleRate : 1;       // Affects envelope rate scaling
            uint8_t         SustainMode : 1;        // Whether to sustain during the sustain phase, or release instead
            uint8_t         TremoloEnable : 1;
            uint8_t         VibratoEnable : 1;
            };
    };

    // A single channel, which can contain two or more operators
    class Channel {

        public:
                            Channel();
            void            SetMaster(Opal *opal) {  Master = opal;  }
            void            SetOperators(int8_t pair1, int8_t pair2);

            void            Output(int16_t *frame);
            void            SetEnable(bool on) {  Enable = on;  }
            void            SetFourOp(bool on) {  FourOp = on;  }

            void            SetFrequencyLow(uint8_t freq);
            void            SetFrequencyHigh(uint8_t freq);
            void            SetKeyOn(bool on);
            void            SetOctave(uint8_t oct);
            void            SetLeftEnable(bool on);
            void            SetRightEnable(bool on);
            void            SetPan(uint8_t pan);
            void            SetFeedback(uint8_t val);
            void            SetModulationType(uint8_t type);

            uint16_t        GetFreq() const {  return Freq;  }
            uint8_t         GetOctave() const {  return Octave;  }
            uint16_t        GetKeyScaleNumber() const {  return KeyScaleNumber;  }
            uint8_t         IsFourOp() const {  return FourOp;  } /* libADLMIDI */

            void            ComputeKeyScaleNumber();

        protected:
            void            ComputePhaseStep();

            Opal *          Master;             // Master object
            int8_t          OpPair1;
            int8_t          OpPair2;
            uint16_t        Freq;               // Frequency; actually it's a phase stepping value
            uint8_t         Octave;             // Also known as "block" in Yamaha parlance
            uint32_t        PhaseStep;
            uint16_t        KeyScaleNumber;
            uint8_t         FeedbackShift;
            struct {
            uint8_t         Enable : 1;
            uint8_t         FourOp : 1;
            uint8_t         ModulationType : 1;
            uint8_t         LeftEnable : 1, RightEnable : 1;
            };
            uint16_t        LeftPan, RightPan;
    };

    public:
                            Opal(int sample_rate);
                            ~Opal();

        void                SetSampleRate(int sample_rate);
        void                Port(uint16_t reg_num, uint8_t val);
        void                Pan(uint16_t reg_num, uint8_t pan);
        void                Sample(int16_t *frame);

    protected:
        void                Init(int sample_rate);
        void                Output(int16_t *frame);

        uint16_t            Clock;
        uint16_t            TremoloClock;
        uint16_t            TremoloLevel;
        uint16_t            VibratoTick;
        uint16_t            VibratoClock;
        struct {
        uint8_t             NoteSel : 1;
        uint8_t             TremoloDepth : 1;
        uint8_t             VibratoDepth : 1;
        };
        int32_t             SampleRate;
        int32_t             SampleAccum;
        int16_t             LastOutput[2], CurrOutput[2];
        Channel             Chan[NumChannels];
        Operator            Op[NumOperators];
//      uint16_t            ExpTable[256];
//      uint16_t            LogSinTable[256];
};

#endif // OPAL_HPP
