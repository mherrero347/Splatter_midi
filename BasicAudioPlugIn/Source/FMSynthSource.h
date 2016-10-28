/*
  ==============================================================================

    FMSynthSource.h
    Created: 22 Oct 2016 10:19:07am
    Author:  Matthew Herrero

  ==============================================================================
*/

#ifndef FMSYNTHSOURCE_H_INCLUDED
#define FMSYNTHSOURCE_H_INCLUDED

// Not really using this here, but we need it for things to work
struct FMSound : public SynthesiserSound
{
    FMSound() {}
    
    ~FMSound() {}
    bool appliesToNote (int /*midiNoteNumber*/) override        { return true; }
    bool appliesToChannel (int /*midiChannel*/) override        { return true; }
};

// The FM synth voice. The FM synth is hardcoded here but ideally it should be in its own class
// to have a clear hierarchy (Sine -> FMSynth -> FMVoice)
class FMVoice : public SynthesiserVoice
{
public:
    FMVoice()
    {
        sawOsc.init(getSampleRate());
        sawOsc.buildUserInterface(&sawControl);
    }
    
    ~FMVoice() {
        
    }
    
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<FMSound*> (sound) != nullptr;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        double note_freq = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        sawControl.setParamValue("/saw/freq",note_freq);
        sawControl.setParamValue("/saw/level", velocity);
    }
    
    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        sawControl.setParamValue("/saw/level", 0);
        
    }
    
    void pitchWheelMoved (int /*newValue*/) override
    {
        // Pitch wheel is an important standard MIDI feature that should be implemented
    }
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
        // Thats where you would change the value of the modulator index and frequency
        // if you wanted to control them with MIDI controllers
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        float** sample_pass = new float*[1];
        sample_pass[0] = new float[1];
        
        // only compute block if note is on!
        while (--numSamples >= 0){
            
            sawOsc.compute(1, sample_pass, sample_pass);
            for (int i = outputBuffer.getNumChannels(); --i >= 0;){
                outputBuffer.addSample (i, startSample, sample_pass[0][0]);
            }
            ++startSample;
        }
        
        delete [] sample_pass[0];
        delete [] sample_pass;
    }
    
private:
    Saw sawOsc;
    MapUI sawControl;
};

#endif  // FMSYNTHSOURCE_H_INCLUDED