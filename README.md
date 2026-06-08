# Variable signal chain

This project stems from the [PSP-Plant Signal Processor](https://github.com/PoliTeK/PSP-PlantSignalProcessing), developed with [Politek](https://github.com/PoliTeK).

The idea is to have a signal chain in the style of digital guitar multifx, with a linear chain where blocks can be moved at the user request. 
The objective is to achieve this with the power of object oriented programming while working on constrained hardware like the daisy seed, but can be run on any microcontroller that supports C++.
The structure is the following:


![alt text](https://github.com/Matteocaroleo/signal-chain/blob/main/src/Classes/sigChain/sigChain_handler.png?raw=true)
