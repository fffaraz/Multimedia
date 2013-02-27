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

#include "devicepa.h"
#include <portaudio.h>

using namespace std;

namespace agh {

/*
 *  PortAudio have to be initialized before running this c'tor
 */
DevicePa::DevicePa(int index)
{
	const   PaDeviceInfo *deviceInfo;
    PaStreamParameters inputParameters, outputParameters;
    PaError err;
    
    static double standardSampleRates[] = {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 /* negative terminated  list */
    };    
    
    deviceInfo = Pa_GetDeviceInfo( index );

	name = deviceInfo->name;
	id = index;
	hostAPI = Pa_GetHostApiInfo( deviceInfo->hostApi )->name;
	channelCountInput = deviceInfo->maxInputChannels;
	channelCountOutput = deviceInfo->maxOutputChannels;
	defLowInputLat = deviceInfo->defaultLowInputLatency;
	defLowOutputLat = deviceInfo->defaultLowOutputLatency;
	defHighInputLat = deviceInfo->defaultHighInputLatency;
	defHighOutputLat = deviceInfo->defaultHighOutputLatency;
	defSampleRate = deviceInfo->defaultSampleRate;	

	/* poll for standard sample rates */
	inputParameters.device = index;
    inputParameters.channelCount = deviceInfo->maxInputChannels;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    outputParameters.device = index;
    outputParameters.channelCount = deviceInfo->maxOutputChannels;
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
    outputParameters.hostApiSpecificStreamInfo = NULL;

    if( inputParameters.channelCount > 0 ) {
    	for(int i=0; standardSampleRates[i] > 0; i++ ) {
        	err = Pa_IsFormatSupported( &inputParameters, NULL, standardSampleRates[i] );
        	if( err == paFormatIsSupported )
        		sampleRatesHalfDupInput.push_back(standardSampleRates[i]);
    	}
    }

    if( outputParameters.channelCount > 0 ) {
    	for(int i=0; standardSampleRates[i] > 0; i++ ) {
        	err = Pa_IsFormatSupported( NULL, &outputParameters, standardSampleRates[i] );
        	if( err == paFormatIsSupported )
        		sampleRatesHalfDupOutput.push_back(standardSampleRates[i]);
    	}
    }

    if( inputParameters.channelCount > 0 && outputParameters.channelCount > 0 ) {
		for(int i=0; standardSampleRates[i] > 0; i++ ) {
        	err = Pa_IsFormatSupported( &inputParameters, &outputParameters, standardSampleRates[i] );
        	if( err == paFormatIsSupported )
        		sampleRatesFullDup.push_back(standardSampleRates[i]);
    	}
    }
}

}
