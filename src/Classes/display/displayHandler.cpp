#include "displayHandler.hpp"


DisplayHandler::DisplayHandler (MyOledDisplay* displayPtr, bool triggerEnabled){
    // should clean circular buffer
    memset((void*) _circBuffer_ptr, 0, sizeof(_circBuffer_ptr));
    
    // sets pointer of display
    _displayPtr = displayPtr;
    _triggerEnabled = triggerEnabled;

}

void DisplayHandler::Update(){
    switch (_currentState){
        case DisplayState::STANDBY:
            drawStandbyScreen();
            break;

        case DisplayState::WAVEFORM_VIEWER:
            drawWaveForm();
            break;

        case DisplayState::SIGNAL_CHAIN:
            drawSignalChain ();
            break;

        case DisplayState::POT_VALUE:
            drawPotValue();
            break;
    }
    _displayPtr->Update();
}


/* NEEDS HEAVY TUNING*/
void DisplayHandler::drawPotValue(){

    // rescaling float val -> NEEDS DEBUGGING ON BOARD TO TRIM THIS VAL
    int rescaledVal = (int) *_potValuePtr * 10.f;
    
    // chosen to be four
    char intToString_rescaledValue [4];
    sprintf(intToString_rescaledValue, "%d",rescaledVal);
    
    // write to display the string containig the resacled val
    _displayPtr->WriteString((const char*)intToString_rescaledValue,Font_16x26, true);
  
}


void DisplayHandler::SetState(DisplayState newState){

    // clears display at each new state
    _displayPtr->Fill(false);
    _currentState = newState;
}


// fills buffer for waveform viewer: to be called in audio callback
void DisplayHandler::pushAudioSample(float sample){
    _circBuffer_ptr[_writeHead] = sample;

    // Implements circular buffer logic
    _writeHead = (_writeHead + 1) % BUFFER_SIZE;
}

// Trigger logic
int DisplayHandler::findTrigger(){
    float currVal = 0;
    float nextVal= 0;
    static int trigIdx = 0;

    // _writeHead is the most recent
    int oldestSampleIdx = _writeHead + 1;

    // what?
    // if (trigIdx < 0) trigIdx = BUFFER_SIZE - trigIdx;
    
    if (_triggerEnabled==true){
        

        // finds the offset from oldest sample from where to get the trigger
        for (int i = oldestSampleIdx; i < oldestSampleIdx + _windowSize; i++){
            currVal = _circBuffer_ptr[i  % BUFFER_SIZE];
            nextVal = _circBuffer_ptr[ (i+1)  % BUFFER_SIZE];
             
            // trigger condition: rising edge
            if (currVal < 0.f && nextVal >= 0.f){
                trigIdx = i  % BUFFER_SIZE;
                break; // CHANGE THIS BECAUSE BREAK IS UGLY
            }
        }
    }           
    return trigIdx;
}

/* DISPLAY FUNCTONS */

// the idea is to plot it with drawLine and decimation to avoid lagging
// STILL IN DEVELOPMENT
void DisplayHandler::drawWaveForm (){
    
    int zeroOfScreen = 32; // because is 64 pixel in heigth
    int y1, y2;
    int idx = 0;

    _displayPtr->SetCursor(0, zeroOfScreen);
    _displayPtr->Fill(false);

    idx = findTrigger();
    // plot whole buffer -> TAKES CIRCULARITY INTO ACCOUNT
    for (int i = idx; i < idx + _windowSize; i++){
        // *100 is for rescaling -> NEEDS TUNING
        
        // TODO check if compiler optimizes clamping
        y1 = zeroOfScreen - (_circBuffer_ptr[i % BUFFER_SIZE]* 40);
        if (y1 >= 63) y1 = 63;
        if (y1 <= 0) y1 = 0; 
        y2 = zeroOfScreen - (_circBuffer_ptr[(i + 1) % BUFFER_SIZE] * 40);
        if (y2 >= 63) y2 = 63;
        if (y2 <= 0) y2 = 0;
        _displayPtr->DrawLine(i-idx, y1, (i+1)-idx, y2, true);
    }   


}



// Standby screen drawing function
void DisplayHandler::drawStandbyScreen(){
  _displayPtr->SetCursor(0,0);
  _displayPtr->WriteString("STANDBY", Font_16x26, true);  
}

// Draws signal chain
void DisplayHandler::drawSignalChain(){
    _displayPtr->SetCursor(0,0);
  
    // checks if signal chain was given
    if (_sigChain_handlerPtr != nullptr){

        // prints the name of all effects
        for (unsigned int i = 0; i < _sigChain_handlerPtr->GetEffectsCounter(); i++){
            _displayPtr->SetCursor(0, 10*i);
            _displayPtr->WriteString(_sigChain_handlerPtr->GetEffectName(i), Font_7x10, true);
        } 
    }
    else {
        _displayPtr->WriteString ("NO 5 CHAIN", Font_16x26, true); 
    }
}