#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define C_WIDTH 200
#define C_HEIGHT 2000

char ca60[8] = {0, 0, 1, 1, 1, 1, 0, 0};
char ca110[8] = {0, 1, 1, 1, 0, 1, 1, 0};

int randint(int start, int end) {
	int r = rand() / (RAND_MAX / (end - start + 1) + 1);
	return r;
}

void initCA(char ** ca, int width, int height) {
	for (int i = 0; i < width; ++i) {
		ca[i][0] = randint(0, 1);

		//Rule 60 init
//		if (i == 0)
//			ca[i][0] = 1;
//		else
//			ca[i][0] = 0;
	}
}

//three neighbors
void nextGenCA(char ** ca, char * rules, int level, int width, int height) {
	int lastLevel = level - 1;

	if (lastLevel < 0)
		lastLevel = height - 1;

	for (int i = 0; i < width; ++i) {
		char pval = 0;

		for (int v = 0; v < 3; ++v) {
			int ci = i + v - 1;

			if (ci >= 0 && ci < width - 1)
				pval += ca[ci][lastLevel] << (2 - v);
		}

		ca[i][level] = rules[pval];
	}
}


//speed 490
short int * createAudioData(char ** ca, int width, int height, int samplingRate, int nrOfSamples) {
	int samplesPerGeneration = (int)(nrOfSamples / height);
	int maxValue = 0;
	int maxFreq = 20000;
	float freqConst = (float)maxFreq / width;

	int * tmpData = calloc(nrOfSamples, sizeof(int));
	short int * data = calloc(nrOfSamples, sizeof(short int));

	for (int s = 0; s < nrOfSamples; ++s) {
		int currentGeneration = (int)(s / samplesPerGeneration);
		int rez = 0;

		for (int x = 0; x < width; ++x) {
			int volume = ca[x][currentGeneration] * 100;

			if (volume == 0)
				continue;

			int freq = (int)(freqConst * x);

			rez += (int)(volume * sin(freq * 3.1415 * 2 * s /samplingRate));
		}

		tmpData[s] = rez;

		if (abs(rez) > maxValue)
			maxValue = abs(rez);
	}

	for (int s = 0; s < nrOfSamples; ++s)
		data[s] = (32767 * tmpData[s] / maxValue);

	return data;
}

void writeLittleEndian(unsigned int word, int num_bytes, FILE * wav_file) {
    unsigned buf;
    while (num_bytes > 0) {
    	buf = word & 0xff;
        fwrite(&buf, 1,1, wav_file);
        num_bytes--;
        word >>= 8;
    }
}

void writeWav(char * filename, unsigned long numSamples, short int * data, int sRate) {
    FILE* wavFile;
    unsigned int sampleRate;
    unsigned int numChannels;
    unsigned int bytesPerSample;
    unsigned int byteRate;
    unsigned long i;    /* counter for samples */

    numChannels = 1;   /* monoaural */
    bytesPerSample = 2;

    if (sRate<=0) sampleRate = 44100;
    else sampleRate = (unsigned int) sRate;

    byteRate = sampleRate * numChannels * bytesPerSample;

    wavFile = fopen(filename, "w");
//    assert(wavFile);   /* make sure it opened */

    /* write RIFF header */
    fwrite("RIFF", 1, 4, wavFile);
    writeLittleEndian(36 + bytesPerSample * numSamples * numChannels, 4, wavFile);
    fwrite("WAVE", 1, 4, wavFile);

    /* write fmt  subchunk */
    fwrite("fmt ", 1, 4, wavFile);
    writeLittleEndian(16, 4, wavFile);   /* SubChunk1Size is 16 */
    writeLittleEndian(1, 2, wavFile);    /* PCM is format 1 */
    writeLittleEndian(numChannels, 2, wavFile);
    writeLittleEndian(sampleRate, 4, wavFile);
    writeLittleEndian(byteRate, 4, wavFile);
    writeLittleEndian(numChannels * bytesPerSample, 2, wavFile);  /* block align */
    writeLittleEndian(8*bytesPerSample, 2, wavFile);  /* bits/sample */

    /* write data subchunk */
    fwrite("data", 1, 4, wavFile);
    writeLittleEndian(bytesPerSample * numSamples*numChannels, 4, wavFile);
    for (i=0; i< numSamples; i++) {\
    	writeLittleEndian((unsigned int)(data[i]),bytesPerSample, wavFile);
    }

    fclose(wavFile);
}

int main(int argc, char **argv) {

	srand(time(NULL));

	char ** cells = calloc(C_WIDTH, sizeof(char * ));

	for (int i = 0; i < C_WIDTH; ++i) {
		cells[i] = calloc(C_HEIGHT, sizeof(char));
	}

	initCA(cells, C_WIDTH, C_HEIGHT);

	for (int i = 0; i < C_HEIGHT; ++i) {
		nextGenCA(cells, ca110, (i+1), C_WIDTH, C_HEIGHT);
	}

	int durationSeconds = 32;

	short int * audioData = createAudioData(cells, C_WIDTH, C_HEIGHT, 44100, (44100 * durationSeconds));

	writeWav("out.wav", (44100 * durationSeconds), audioData, 44100);

}
