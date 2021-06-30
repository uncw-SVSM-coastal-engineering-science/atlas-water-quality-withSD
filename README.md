# Adaptation of Code for Whitebox Tentacle Shield with Atlas Probes and Arduino
The goal of this project is to replicate portions of the capabilities of the sensor described by [Compas and Wade (2018)](https://theoryandpractice.citizenscienceassociation.org/articles/10.5334/cstp.124/). 

We are using an Arduino Uno and a [Whitebox T2 Mini MkII for Arduino, model T2.21 BNC edition](https://www.whiteboxes.ch/docs/tentacle/t2-mkII/#/?id=whitebox-t2-mini-mkii-for-arduino), along with Atlas RTD (temperature), EC (conductivity), pH, and DO (dissolved oxygen) probes.

We followed the quickstart guide available [here](https://www.whiteboxes.ch/docs/tentacle/t2-mkII/#/quickstart), including putting sensors into I2C mode and subsequently interacting with them via the EZO Console. 

Erin Moran adapted code from https://github.com/Atlas-Scientific/Ezo_I2c_lib (specifically https://github.com/Atlas-Scientific/Ezo_I2c_lib/blob/master/Examples/I2c_lib_examples/temp_comp_example/temp_comp_example.ino) to use EC, DO, and pH probes in addition to the RTD sensor. Also reference https://www.whiteboxes.ch/docs/tentacle/t2-mkII/#/temperature-compensation-example. 