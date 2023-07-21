#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;

Parameter knob2;

Color led1Color[4];
Color led2Color[101];
int color = 0;
int color2 = 0;

void UpdateControls();

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

	hw.ProcessAllControls();
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
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