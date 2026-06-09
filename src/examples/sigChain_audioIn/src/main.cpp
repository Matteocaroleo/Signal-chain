/// @file main.cpp
/// @brief  Example code to show signal chain with audio input on ADC0.
///         Also uses a i2c display: undef ther DISPLAY constant to remove code that uses it

#include "sigChain/sigChain_handler.hpp"
#include "effect/effectWrapper.cpp"
#include "display/displayHandler.hpp"
#include "daisy_seed.h"
#include "daisysp.h"

#define DISPLAY
#define AUDIO_INPUT_PIN daisy::seed::A0

using namespace daisy;
DaisySeed hw;
float outSample;

/* SIGNAL CHAIN CREATION */
// instantiation of the chain
sigChain_handler chain1;

/* MAIN BLOCKS INSTANTIATION */ 
daisysp::Tremolo trem1;
daisysp::Overdrive od1;
daisysp::PitchShifter pitchshift1;

#ifdef DISPLAY 
MyOledDisplay realDisp;
DisplayHandler disp1 (&realDisp);
#endif  // DISPLAY

// global
float audioIn;
bool decimate = false;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
        // process audio through chain
        outSample = chain1.Process(in[0][i]);

        #ifdef DISPLAY
        // updates display (even if not displaying waveform)
        if(decimate) disp1.pushAudioSample (outSample *4);
        else decimate = !decimate;
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

    // adc setup
    // AdcChannelConfig adc_config;
    //adc_config.InitSingle (AUDIO_INPUT_PIN);
    //hw.adc.Init(&adc_config, 1);
    //hw.adc.Start();
    
    /* MAIN BLOCKS INITIALIZATION */
    pitchshift1.Init (sample_rate);
    trem1.Init(sample_rate);
    od1.Init ();

    #ifdef DISPLAY
    // init real display, PINS
    // - SDA  i2c_config.pin_config.scl = Pin(PORTB, 8);
    // - SCL i2c_config.pin_config.sda = Pin(PORTB, 9);
    MyOledDisplay::Config disp_cfg;
    realDisp.Init(disp_cfg);
    #endif //  DISPLAY

    /* WRAPPER INSTANTIATION*/

    // todo: how to add pitchshifter?

    // the wrapper can be created on a oneliner
    EffectWrapper <daisysp::Tremolo,float,float> tremWrapper (
        &trem1, "Tremolo",
        &daisysp::Tremolo::SetFreq, "Frequency",
        &daisysp::Tremolo::Process
    );


    EffectWrapper <daisysp::Overdrive,float,float> odWrapper (
        &od1, "Overdrive",
        &daisysp::Overdrive::SetDrive, "Drive",
        &daisysp::Overdrive::Process
    );

    EffectWrapper <daisysp::Overdrive,float,float> od2Wrapper (
        &od1, "Overdrive 2",
        &daisysp::Overdrive::SetDrive, "Drive",
        &daisysp::Overdrive::Process
    );

    float tremTime = 0.f; // [Hz]
    float driveAmount = .5f; // from 0 to 1
    const float pitchshift = 1.f; //semitones 
    tremWrapper.SetParam(tremTime);
    odWrapper.SetParam(driveAmount);
    chain1.addEffect (&tremWrapper);
    chain1.addEffect (&odWrapper);
    chain1.addEffect (&od2Wrapper);
    
    #ifdef DISPLAY

    // set display state at start
    disp1.SetState(DisplayState::WAVEFORM_VIEWER);
    // set also signal chain
    disp1.SetSigChain (&chain1);
    unsigned int state = 0;  
    
    #endif // DISPLAY

    while (1)
    {
        // reads audio value
        //audioIn = hw.adc.GetFloat (0);
       
        #ifdef DISPLAY
        // Switches between states every cycle of audio
        disp1.Update();
        #endif // DISPLAY 
    }

    return 0;
}