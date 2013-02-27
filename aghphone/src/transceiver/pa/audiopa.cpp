/*
 * Copyright (C) 2008  Mateusz Kramarczyk <kramarczyk (at) gmail (dot) com>
 * Copyright (C) 2008  Tomasz Kijas <kijasek (at) gmail (dot) com>
 * Copyright (C) 2008  Tomir Kryza <tkryza (at) gmail (dot) com>
 * Copyright (C) 2008  Maciej Kluczny <kluczny (at) fr (dot) pl>
 * Copyright (C) 2008  AGH University of Science and Technology <www.agh.edu.pl>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "audiopa.h"
#include "restypes.h"
#include <ccrtp/rtp.h>
#include <portaudio.h>

using namespace std;
using namespace ost;

namespace agh {

/* AudioPa implementation */

AudioPa::AudioPa(Transceiver *t)
{
	this->t = t;
	
	PaError err = Pa_Initialize();

	if( err != paNoError ) {
		 cout << Pa_GetErrorText(err) << endl;
		 return;
	}
	
	devMgr = new DeviceFactoryPa();
	
	setInputDevice(devMgr->getDefaultInputDevice());
	setOutputDevice(devMgr->getDefaultOutputDevice());
	
	inputStream = NULL;
	outputStream = NULL;
	inputBuffer = NULL;
	outputBuffer = NULL;
	
}

AudioPa::~AudioPa()
{
	delete inputBuffer;
	delete outputBuffer;
	delete devMgr;
}

void AudioPa::setTransceiver(Transceiver *t)
{
	this->t = t;
}

vector<IDevice*> AudioPa::getAvailableInputDevices() const
{
	vector<IDevice*> v;
	
	return v;
}

vector<IDevice*> AudioPa::getAvailableOutputDevices() const
{
	vector<IDevice*> v;
	
	return v;
}

int AudioPa::setInputDevice(const IDevice& dev)
{
	inputDevice = &dev;
	
	return 0;
}

int AudioPa::setInputDevice(const int id)
{
	inputDevice = &devMgr->getDevice(id);
	
	return 0;
}

int AudioPa::setOutputDevice(const IDevice& dev)
{
	outputDevice = &dev;
	
	return 0;
}

int AudioPa::setOutputDevice(const int id)
{
	outputDevice = &devMgr->getDevice(id);
	
	return 0;
}

int AudioPa::start()
{	
		framesPerBuffer = t->codec->getFrameCount();
		sampleRate = t->codec->getFrequency();
		
		openStream();
		
		outputBuffer = new RingBuffer(framesPerBuffer*200, t->packetSize);
		inputBuffer = new RingBuffer(framesPerBuffer*200, t->packetSize);
	
	return TransceiverStartResult::SUCCESS;
}

int AudioPa::stop()
{
	Pa_AbortStream(inputStream);
	Pa_AbortStream(outputStream);
	Pa_CloseStream(inputStream);
	Pa_CloseStream(outputStream);
	cout << "audio stream closed" << endl;
	
	Pa_Terminate();
	cout << "portaudio terminated" << endl;
	
	return 0;
}

void AudioPa::openStream()
{
	cout << "Opening streams, sample size: " << sizeof(sampleType) << ", sample rate: " << 8000 << endl;
	
	PaStreamParameters inputParameters, outputParameters;
	PaError err;
	
	bzero(&inputParameters, sizeof(PaStreamParameters));
	bzero(&outputParameters, sizeof(PaStreamParameters));
	
	
	inputParameters.device = inputDevice->getID(); /* default input device */
  	inputParameters.channelCount = 1;                    /* stereo input */
	inputParameters.sampleFormat = SAMPLE_TYPE;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;
    
    outputParameters.device = outputDevice->getID(); /* default input device */
  	outputParameters.channelCount = 1;                    /* stereo input */
	outputParameters.sampleFormat = SAMPLE_TYPE;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	
	/* SINGLE STREAM */
	
	
	
	/*err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, sampleRate, framesPerBuffer,
						paClipOff, NULL, NULL);
	
	if(err != paNoError) {
		cout << Pa_GetErrorText(err) << endl;
		return;
	}*/
	
	
	 // TWO STREAMS
	
	err = Pa_OpenStream(&inputStream, &inputParameters, NULL, sampleRate, framesPerBuffer,
						paClipOff, NULL, NULL);
	
	if(err != paNoError) {
		cout << "Input stream opening error: " << Pa_GetErrorText(err) << endl;
		return;
	}
	
	err = Pa_OpenStream(&outputStream, NULL, &outputParameters, sampleRate, framesPerBuffer,
						paClipOff, NULL, NULL);
	
	if(err != paNoError) {
		cout << "Output stream opening error: " << Pa_GetErrorText(err) << endl;
		return;
	}

	err = Pa_StartStream(inputStream);
	if(err != paNoError) {
		cout << Pa_GetErrorText(err) << endl;
		return;
	}
	err = Pa_StartStream(outputStream);
	if(err != paNoError) {
		cout << Pa_GetErrorText(err) << endl;
		return;
	}
	
	/*err = Pa_StartStream(stream);
	if(err != paNoError) {
		cout << Pa_GetErrorText(err) << endl;
		return;
	}*/
	

	cout << "Stream(s) opened successfully" << endl;	
}

/*
 * gets data from audio input, it will copy data from the input buffer to the
 * memory region pointed by dest, size of the requested data is set with setPacketSize
 * @param[out] dest pointer to the memory region to which data is to be copied
 */
bool AudioPa::getData(void* dest, long size)
{
	return inputBuffer->getData((char*)dest, size);
}

/*
 * puts data into the output buffer
 * @param[in] src pointer to the memory region from which data is to be read
 * @param size size of the data [B]
 */
void AudioPa::putData(void* src, long size)
{
	outputBuffer->putData((char*)src, size);
}

void AudioPa::moveData(RingBuffer* dest, long size)
{
	outputBuffer->moveData(dest, size);
}

void AudioPa::flush()
{
	if(outputBuffer->getReadyCount() >= framesPerBuffer) {
		if(Pa_GetStreamWriteAvailable(outputStream) > framesPerBuffer) { 
			char buf[2048];
			outputBuffer->peekData(buf, framesPerBuffer);
			//int err = alsa_write(playback_handle, (unsigned char*)buf, framesPerBuffer);
			int err = Pa_WriteStream(outputStream, buf, framesPerBuffer);
			if(err != paNoError) {
				printf("couldn't write to the output, omitting %d frames\n", framesPerBuffer); fflush(stdout);
			}
			outputBuffer->skipData(framesPerBuffer);
		}
	}
}

void AudioPa::read()
{
	long len = Pa_GetStreamReadAvailable(inputStream);
	
	if(len > 0) {
		if(len > framesPerBuffer)
			len = framesPerBuffer;
			
		char buf[2048];
		
		Pa_ReadStream(inputStream, buf, framesPerBuffer);
		
		int err;
		if(err != paNoError) {
			cout << "Failed to read samples from capture device : " << Pa_GetErrorText(err) << endl;
		} else {
			inputBuffer->putData(buf, len);
		}
	}
}

}
