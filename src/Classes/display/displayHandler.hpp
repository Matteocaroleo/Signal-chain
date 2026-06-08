/// @file displayHandler.hpp
/// @brief handler for ssd130x

/// @todo make buffer size statically asserted as a power of 2
/// @todo improve drawPotValue screen with parameter name
/// @todo check dma functionality

#include "../../../../libs/libDaisy/src/dev/oled_ssd130x.h"
#include "../../../../libs/libDaisy/src/daisy_seed.h"

#include "../sigChain/sigChain_handler.hpp"

#define BUFFER_SIZE 256
#define WINDOW_SIZE 128
// Setting the display type (should be changed based on display) 
using MyOledDisplay = daisy::OledDisplay<daisy::SSD130xI2c128x64Driver>;

// The display is implemented as an FSM

enum class DisplayState : uint8_t {
    STANDBY,
    WAVEFORM_VIEWER,
    SIGNAL_CHAIN,
    POT_VALUE
};

class DisplayHandler {
private:

    // pointer to true display object
    MyOledDisplay* _displayPtr = nullptr;

    DisplayState _currentState = DisplayState::STANDBY;

    // used for waveform viewer
    float _circBuffer_ptr [BUFFER_SIZE];
    
    // used as local stable version of circ buffer
    float _dispBuffer_ptr [BUFFER_SIZE];

    int _windowSize = WINDOW_SIZE;
    int _writeHead = 0;
    bool _triggerEnabled;

    // might not be needed      
    float* _potValuePtr;

    // ptr to signal chain  
    sigChain_handler* _sigChain_handlerPtr = nullptr;
    
    // gives access to the display to the effects pointer to access names and such
    friend class sigChain_handler;

    int findTrigger();
    
    /* internal drawing functions */
    
        
    void drawStandbyScreen();
    void drawWaveForm();
    void drawSignalChain();
    // RIGHT NOW IT ONLY SHOWS THE NUMBER
    void drawPotValue();
    
public:

    DisplayHandler (MyOledDisplay* displayPtr, bool triggerEnabled = true) ;


    // Update fsm
    void Update();

    // Set new fsm state
    void SetState (DisplayState newState);

    // Sets the internal ptr to signal chain
    void SetSigChain (sigChain_handler* sigChain_handlerPtr) {_sigChain_handlerPtr = sigChain_handlerPtr;}

    // Sends audio sample to oscilloscope
    void pushAudioSample (float sample);

};
