#include "daisy_pod.h"
#include "daisysp.h"
#include <math.h>
#include <random>

using namespace daisy;
using namespace daisysp;

std::random_device rd;     // Only used once to initialise (seed) engine
std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
std::uniform_int_distribution<int> uni(1,12); // Guaranteed unbiased





DaisyPod hw;
Oscillator osc;
Parameter knob1;
Parameter knob2;

Color led1Color[4];
Color led2Color[101];
int color = 0;
int color2 = 0;
float osc_freq = 440.f;

void UpdateControls();
void SetOsc();

void InitOsc()
{
	knob1.Init(hw.knob1, -0.4999, 6.4999, Parameter::LINEAR);
	osc.Init(hw.AudioSampleRate());
}

void InitLED()
{
    led1Color[0].Init(0,0,0);
	led1Color[1].Init(0,0,1);
	led1Color[2].Init(0,0.67,0.67);
	led1Color[3].Init(0.5,0.5,0.5);

	for(int count = 0; count <= 100; count++)
	{
		led2Color[count].Init(count*0.01, 1-count*0.01, 0);
	}

	knob2.Init(hw.knob2, 0, 100, Parameter::LINEAR);
}


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	UpdateControls();

	SetOsc();

	hw.ProcessAllControls();

	for (size_t i = 0; i < size; i++)
	{
		//out[0][i] = in[0][i];
		//out[1][i] = in[1][i];

		out[0][i] = osc.Process();
		out[1][i] = out[0][i];

	}
}

int main(void)
{
	hw.Init();
	
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();
	hw.StartAudio(AudioCallback);

	InitLED();
	InitOsc();

	while(1)
	{

	}
}

void UpdateEncoder()
{
	color = color + hw.encoder.Increment();

	if(color < 0)
	{
		color = 3;
	}
	else if(color > 3)
	{
		color = 0;
	}

	if(hw.encoder.RisingEdge())
    {
        color = 0;
    }

}

void UpdateKnobs()
{
	color2 = (int)knob2.Process();
	
}

void UpdateLeds()
{
	hw.led1.SetColor(led1Color[color]);
	hw.led2.SetColor(led2Color[color2]);
	hw.UpdateLeds();
}

void UpdateControls()
{
	hw.ProcessAnalogControls();
	hw.ProcessDigitalControls();

	UpdateEncoder();
	UpdateKnobs();
	UpdateLeds();

	
}

void SetOsc()
{
	osc.SetWaveform((int)round(knob1.Process()));
	osc.SetAmp(0.5);
	
	 

	if(hw.button2.RisingEdge())
	{
		int random_integer = uni(rng);
		float random_float = random_integer * 0.08333;
		osc_freq = osc_freq * pow(2,random_float);
	}
    
	if(hw.button1.RisingEdge())
	{
		int random_integer = uni(rng);
		float random_float = random_integer * 0.08333;
		osc_freq = osc_freq / pow(2,random_float);
	}
	osc.SetFreq(osc_freq);
        

}
