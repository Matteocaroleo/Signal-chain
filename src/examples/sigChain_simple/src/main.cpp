// TO DO: 
//      - add const char with instantiation (e.g. name of param of set param)

/// @file main.cpp
/// @brief  Example code to show an easy signal chain implementation.
///         Also uses a i2c display: undef ther DISPLAY constant to remove code that uses it

#include "sigChain/sigChain_handler.hpp"
#include "effect/effectWrapper.cpp"
#include "display/displayHandler.hpp"
#include "daisy_seed.h"
#include "daisysp.h"

#define DISPLAY

using namespace daisy;
DaisySeed hw;
float outSample;

/* SIGNAL CHAIN CREATION */
// instantiation of the chain
sigChain_handler chain1;

/* MAIN BLOCKS INSTANTIATION */
// here the block of the signal chain are instantiated
daisysp::Oscillator lfo1;


daisysp::Tremolo trem1;


#ifdef DISPLAY 
MyOledDisplay realDisp;

// display handling
DisplayHandler disp1 (&realDisp);
#endif  // DISPLAY

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
        // process sample through chain
        outSample = chain1.Process();

        #ifdef DISPLAY
        // updates display (even if not displaying waveform)
        disp1.pushAudioSample (outSample);
        #endif // DISPLAY

        // output audio
        out[0][i] = outSample;
		out[1][i] = outSample;
	}
}


int main (){

	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
    int sample_rate = 48000;

    /* MAIN BLOCKS INITIALIZATION */
    // here the blocks of the signal chain are initialized
    lfo1.Init(sample_rate);
    // MAKE THIS WORK
    // lfo1.SetWaveform(Oscillator::WAVE_SAW);
    
    trem1.Init(sample_rate);

    #ifdef DISPLAY
    // init real display, PINS
    // - SDA  i2c_config.pin_config.scl = Pin(PORTB, 8);
    // - SCL i2c_config.pin_config.sda = Pin(PORTB, 9);
    MyOledDisplay::Config disp_cfg;
    realDisp.Init(disp_cfg);
    #endif //  DISPLAY

    /* WRAPPER INSTANTIATION*/

    // instantiation of effect wrapper:
    // - the first templated argument tells it the return value
    //   of the process function
    // - the second templated argument is the type of process arg
    // - the third templated argument tells it the argument value of
    //   the setter function, in this case SetFreq(float);
    // - the fourth templated argument is not needed now but would be
    //   the second argument of the setter function that takes 2 args
    // - the fifth templated argument would be the return type of setters   
    EffectWrapper <daisysp::Oscillator, float, float> oscWrapper;
    
    // creation of function pointers to pass to effect wrapper
    // can be done before to use constructor (cleaner)     
    void (daisysp::Oscillator::* setPtr)(float) = &daisysp::Oscillator::SetFreq;
    float (daisysp::Oscillator::* processPtr)() = &daisysp::Oscillator::Process; 

    // setting of internal pointers of wrapper
    oscWrapper.AddSetParam(setPtr);
    oscWrapper.AddProcess (processPtr);
    oscWrapper.AddEffectPtr(&lfo1);

    // setting parameter
    float frequency = 500;
    oscWrapper.SetParam(frequency);


    // the wrapper can be created on a oneliner
    EffectWrapper <daisysp::Tremolo,float,float> tremWrapper (
        &trem1, "Tremolo",
        &daisysp::Tremolo::SetFreq, "Frequency",
        &daisysp::Tremolo::Process
    );

    
    float tremTime = 1.f; // [Hz]
    tremWrapper.SetParam(tremTime);

    // first block inserted: works because of POLYMORPHISM
    chain1.addEffect (&oscWrapper);
    chain1.addEffect (&tremWrapper);


    #ifdef DISPLAY
    // set display state at start
    disp1.SetState(DisplayState::STANDBY);

    // set also signal chain
    disp1.SetSigChain (&chain1);
    unsigned int state = 0;  
    #endif // DISPLAY

    while (1)
    {
        hw.DelayMs(10);

        // should do a glissando up to 440Hz
        oscWrapper.SetParam(frequency);
        frequency = ((int)frequency + 5) % 880;
       
        #ifdef DISPLAY
        // Switches between states every cycle of audio
        if (frequency == 875){
            if (state == 0){
                disp1.SetState(DisplayState::WAVEFORM_VIEWER);
            }
            else if (state == 1){
                disp1.SetState(DisplayState::STANDBY);
            }
            else {
                disp1.SetState(DisplayState::SIGNAL_CHAIN);
            }
            // update display state
            state = (state % 3) + 1; 
        }
        disp1.Update();
        #endif // DISPLAY 
    }

    return 0;
}