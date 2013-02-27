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

#include "audioalsa.h"
#include "restypes.h"
#include <alsa/asoundlib.h>

using namespace std;
using namespace ost;

namespace agh {

/* AudioAlsa implementation */

AudioAlsa::AudioAlsa(Transceiver *t)
{
	this->t = t;

	devMgr = new DeviceFactoryAlsa();

	setInputDevice(devMgr->getDefaultInputDevice());
	setOutputDevice(devMgr->getDefaultOutputDevice());

	playback_handle = NULL;
	capture_handle = NULL;
	outputBuffer = NULL;
	inputBuffer = NULL;

	initTime();
	resetTimerMs();
}

AudioAlsa::~AudioAlsa()
{
	delete inputBuffer;
	delete outputBuffer;
	delete devMgr;
}

void AudioAlsa::setTransceiver(Transceiver *t)
{
	this->t = t;
}

vector<IDevice*> AudioAlsa::getAvailableInputDevices() const
{
	vector<IDevice*> v;

	return v;
}

vector<IDevice*> AudioAlsa::getAvailableOutputDevices() const
{
	vector<IDevice*> v;

	return v;
}

int AudioAlsa::setInputDevice(const IDevice& dev)
{
	inputDevice = &dev;

	return 0;
}

int AudioAlsa::setInputDevice(const int id)
{
	//inputDevice = &devMgr->getDevice(id);

	return 0;
}

int AudioAlsa::setOutputDevice(const IDevice& dev)
{
	outputDevice = &dev;

	return 0;
}

int AudioAlsa::setOutputDevice(const int id)
{
	//outputDevice = &devMgr->getDevice(id);

	return 0;
}

int AudioAlsa::start()
{
	cout << "Opening streams" << endl;
	openStream();

printf("framespb: %ld, packetsize: %d\n", framesPerBuffer, t->packetSize); fflush(stdout);
	outputBuffer = new RingBuffer(framesPerBuffer*200, t->packetSize);
	inputBuffer = new RingBuffer(framesPerBuffer*200, t->packetSize);
	sampleRate = 8000.0;

	int err = snd_pcm_start(capture_handle);
	if(err < 0) cout << "Alsa error : cannot start capture stream : " << snd_strerror(err) << endl;
	else cout << "started capture stream" << endl;

	initTime();

	return 0;
}

int AudioAlsa::stop()
{
	closeStream();
	return 0;
}

void AudioAlsa::openStream()
{
	int err;
	snd_output_t *log;

	snd_output_stdio_attach(&log, stderr, 0);

	//err = snd_pcm_open(&capture_handle, inputDevice->getName().c_str(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
	err = snd_pcm_open(&capture_handle, "plughw:0,0", SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);

	if(err < 0) cout << "Alsa error : cannot open capture device (" << inputDevice->getName() << ") : " << snd_strerror(err) << endl;

	alsa_set_params(capture_handle, 0);

	cout << "[Capture device]:" << endl;
	snd_pcm_dump(capture_handle, log);

	err = snd_pcm_open(&playback_handle, outputDevice->getName().c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
	//err = snd_pcm_open(&playback_handle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

	if(err < 0) cout << "Alsa error : cannot open playback device (" << outputDevice->getName() << "): " << snd_strerror(err) << endl;

	alsa_set_params(playback_handle, 1);

	cout << "[Playback device]:" << endl;
	snd_pcm_dump(playback_handle, log);

}

void AudioAlsa::closeStream()
{
	snd_pcm_close(playback_handle);
}

snd_pcm_t* AudioAlsa::alsa_set_params(snd_pcm_t *pcm_handle, int rw)
{
	snd_pcm_hw_params_t *hwparams = NULL;
	snd_pcm_sw_params_t *swparams = NULL;
	int dir;
	uint exact_uvalue;
	unsigned long exact_ulvalue;
	int channels = 1;
	int err;
	int rate = 8000;

	int periodsize = 160;
	int periods = 8;
	snd_pcm_format_t format;
	if(!rw) format = SND_PCM_FORMAT_S32;
	else format = SND_PCM_FORMAT_S16;

	snd_pcm_hw_params_alloca(&hwparams);

	err = snd_pcm_hw_params_any(pcm_handle, hwparams);
	if(err < 0) cout << "Alsa error: cannot initialize hw params structure : " << snd_strerror(err) << endl;
	err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if(err < 0) cout << "Alsa error: cannot set access type : " << snd_strerror(err) << endl;
	err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, format);
	if(err < 0) cout << "Alsa error: cannot set sample format : " << snd_strerror(err) << endl;
	err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels);
	if(err < 0) cout << "Alsa error: cannot set channel count : " << snd_strerror(err) << endl;

	exact_uvalue = rate;
	dir = 0;
	err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_uvalue, &dir);
	if(err < 0) cout << "Alsa error: cannot set sample rate : " << snd_strerror(err) << endl;
	if(dir != 0) cout << "Alsa error: " << rate <<
		" Hz sample rate is not supported by your hardware. Using " << exact_uvalue << " Hz instead." << endl;

	periodsize = periodsize*(rate/8000);
	exact_ulvalue=periodsize;
	dir=0;
	err = snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, &exact_ulvalue, &dir);
	if(err < 0) cout << "Alsa error: cannot set period size : " << snd_strerror(err) << endl;
	if(dir != 0) cout << "Alsa error: " << periodsize <<
		" period size is not supported by your hardware. Using " << exact_ulvalue << " instead." << endl;
	periodsize = exact_ulvalue;
	this->framesPerBuffer = periodsize;

	exact_uvalue=periods;
	dir = 0;
	err = snd_pcm_hw_params_set_periods_near(pcm_handle, hwparams, &exact_uvalue, &dir);
	if(err < 0) cout << "Alsa error: cannot set periods : " << snd_strerror(err) << endl;
	if(dir != 0) cout << "Alsa error: " << periods <<
		" periods is not supported by your hardware. Using " << exact_uvalue << " instead." << endl;

	err = snd_pcm_hw_params(pcm_handle, hwparams);
	if(err < 0) cout << "Alsa error: cannot hw parameters : " << snd_strerror(err) << endl;

	if(rw) {
		snd_pcm_sw_params_alloca(&swparams);
		snd_pcm_sw_params_current(pcm_handle, swparams);
		err = snd_pcm_sw_params_set_start_threshold(pcm_handle, swparams, periodsize*2);
		if(err < 0) cout << "Alsa error: cannot start threshold : " << snd_strerror(err) << endl;

		err = snd_pcm_sw_params_set_stop_threshold(pcm_handle, swparams, periodsize*periods);
		if(err < 0) cout << "Alsa error: cannot stop threshold : " << snd_strerror(err) << endl;

		err = snd_pcm_sw_params(pcm_handle, swparams);
		if(err < 0) cout << "Alsa error: cannot start sw params : " << snd_strerror(err) << endl;

	}

	return 0;
}

bool AudioAlsa::alsa_can_read(snd_pcm_t *dev, int frames)
{
	snd_pcm_sframes_t avail;
	int err;

	avail = snd_pcm_avail_update(dev);
	if(avail < 0) {
		snd_pcm_drain(dev);
		err = snd_pcm_recover(dev, avail, 0);
		if(err) cout << "Alsa error: snd_pcm_recover failed on capture device : " << snd_strerror(err) << endl;
		err = snd_pcm_start(dev);
		if(err) cout << "Alsa error: snd_pcm_start failed after recover on capture device : " << snd_strerror(err) << endl;
	}

	return avail >= frames;
}

int AudioAlsa::alsa_read(snd_pcm_t *handle, unsigned char* buf, int nsamples)
{
	int err;
	err = snd_pcm_readi(handle, buf, nsamples);
	if(err < 0) {
		if(err == -EPIPE) {
			snd_pcm_prepare(handle);
			err = snd_pcm_readi(handle, buf, nsamples);
			if(err < 0) cout << "Alsa error : snd_pcm_readi failed : " << snd_strerror(err) << endl;
		} else if(err == 0) {
			cout << "snd_pcm_readi return 0" << endl;
		}
	}

	return err;
}

int AudioAlsa::alsa_write(snd_pcm_t *handle, unsigned char* buf, int nsamples)
{
	int err;
	if ((err = snd_pcm_writei(handle, buf, nsamples)) < 0) {
		if(err == -EPIPE) {
			snd_pcm_prepare(handle);
			alsa_fill_w(handle);
			err = snd_pcm_writei(handle, buf, nsamples);
			if(err < 0) cout << "Alsa error : failed writing " << nsamples << " samples on output device : " << snd_strerror(err) << endl;
		} else if(err != -EWOULDBLOCK) {
			cout << "Alsa error : snd_pcm_writei failed : " << snd_strerror(err) << endl;
		}
	} else if(err!=nsamples) {
		cout << "Alsa warning : only " << err << " samples written instead of " << nsamples << endl;
	}
	return err;
}

void AudioAlsa::alsa_fill_w(snd_pcm_t *pcm_handle)
{
	snd_pcm_hw_params_t *hwparams=NULL;
	int channels;
	snd_pcm_uframes_t buffer_size;
	int buffer_size_bytes;
	void *buffer;

	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_hw_params_current(pcm_handle, hwparams);

	snd_pcm_hw_params_get_channels(hwparams, (unsigned int*)&channels);
	snd_pcm_hw_params_get_buffer_size(hwparams, &buffer_size);
	buffer_size /= 2;
	buffer_size_bytes = buffer_size*channels*2;
	/*
	 * TODO: here buffer is allocated but the memory is not freed later
	 * 		check if we can free it right after snd_pcm_writei
	 */
	buffer = alloca(buffer_size_bytes);
	memset(buffer, 0, buffer_size_bytes);
	snd_pcm_writei(pcm_handle, buffer, buffer_size);
}

void AudioAlsa::initTime() {
	gettimeofday(&czas_start, NULL);
}

void AudioAlsa::printTime() {
	struct timeval czas_teraz;

	gettimeofday(&czas_teraz, NULL);

	printf("%3ld.%3ld [s] ", czas_teraz.tv_sec - czas_start.tv_sec, czas_teraz.tv_usec/1000);
}

void AudioAlsa::resetTimerMs() {
	gettimeofday(&startMs, NULL);
}

long AudioAlsa::getTimeMs() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec - startMs.tv_sec)*1000 + tv.tv_usec/1000;
}

/*
 * gets data from audio input, it will copy data from the input buffer to the
 * memory region pointed by dest, size of the requested data is set with setPacketSize
 * @param[out] dest pointer to the memory region to which data is to be copied
 */
bool AudioAlsa::getData(void* dest, long size)
{
	return inputBuffer->getData((char*)dest, size);
}

/*
 * puts data into the output buffer
 * @param[in] src pointer to the memory region from which data is to be read
 * @param size size of the data [B]
 */
void AudioAlsa::putData(void* src, long size)
{
	outputBuffer->putData((char*)src, size);
}

void AudioAlsa::moveData(RingBuffer* dest, long size)
{
	outputBuffer->moveData(dest, size);
}

void AudioAlsa::flush()
{
	if(outputBuffer->getReadyCount() >= framesPerBuffer) {
		char buf[2048];
		outputBuffer->peekData(buf, framesPerBuffer);

	//	float nbuf[1024];
	//	for(int i=0;i<framesPerBuffer;i++) {
	//		nbuf[i] = (float) ( (float)( ( ( int16_t* ) buf )[i]) / (float)32768.0);
	//	}

		int err = alsa_write(playback_handle, (unsigned char*)buf, framesPerBuffer);
		if(err > 0) {
			outputBuffer->skipData(err);
		} else {
			printf("couldn't write to the output, omitting %ld frames\n", framesPerBuffer); fflush(stdout);
			outputBuffer->skipData(framesPerBuffer);
		}
	}
}

void AudioAlsa::read()
{
	if(alsa_can_read(capture_handle, framesPerBuffer*(int)(sampleRate/8000.0))) {
		char buf[2048];
		int err = alsa_read(capture_handle, (unsigned char*)buf, (int)framesPerBuffer*(int)(sampleRate/8000.0));

		if(err <= 0) {
			cout << "Failed to read samples from capture device : " << snd_strerror(err) << endl;
		} else {
			int16_t nbuf[1024];

			for(int i=0;i<err;i++) {
				nbuf[i] = (int16_t) (  ( ( int32_t* ) buf )[i] >> 16 );
			}

			inputBuffer->putData((char*)nbuf, err);
		}
	}
}

}
