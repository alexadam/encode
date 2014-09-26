#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define C_WIDTH 200
#define C_HEIGHT 2000

int randint(int start, int end) {
	int v1 = end - start + 1;

	if (v1 == 0) v1 = 1;

	int r = rand() / (RAND_MAX / (end - start + 1) + 1);
	return r;
}

void initCA(char ** ca, int width, int height) {
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			ca[i][j] = initGlider(i, j);
//			if (randint(0, 200) < 25)
//				ca[i][j] = 1;
//			else
//				ca[i][j] = 0;
		}
	}
}

int initGlider(int x, int y) {
	if (x == 1 && y == 5)
		return 1;
	if (x == 1 && y == 6)
		return 1;
	if (x == 2 && y == 5)
		return 1;
	if (x == 2 && y == 6)
		return 1;
	if (x == 11 && y == 5)
		return 1;
	if (x == 11 && y == 6)
		return 1;
	if (x == 11 && y == 7)
		return 1;
	if (x == 12 && y == 4)
		return 1;
	if (x == 12 && y == 8)
		return 1;
	if (x == 13 && y == 3)
		return 1;
	if (x == 14 && y == 3)
		return 1;
	if (x == 13 && y == 9)
		return 1;
	if (x == 14 && y == 9)
		return 1;
	if (x == 15 && y == 6)
		return 1;
	if (x == 16 && y == 4)
		return 1;
	if (x == 16 && y == 8)
		return 1;
	if (x == 17 && y == 5)
		return 1;
	if (x == 17 && y == 6)
		return 1;
	if (x == 17 && y == 5)
		return 1;
	if (x == 17 && y == 6)
		return 1;
	if (x == 17 && y == 7)
		return 1;
	if (x == 18 && y == 6)
		return 1;
	if (x == 21 && y == 3)
		return 1;
	if (x == 21 && y == 4)
		return 1;
	if (x == 21 && y == 5)
		return 1;
	if (x == 22 && y == 3)
		return 1;
	if (x == 22 && y == 4)
		return 1;
	if (x == 22 && y == 5)
		return 1;
	if (x == 23 && y == 2)
		return 1;
	if (x == 23 && y == 6)
		return 1;
	if (x == 25 && y == 1)
		return 1;
	if (x == 25 && y == 2)
		return 1;
	if (x == 25 && y == 6)
		return 1;
	if (x == 25 && y == 7)
		return 1;
	if (x == 35 && y == 3)
		return 1;
	if (x == 35 && y == 4)
		return 1;
	if (x == 36 && y == 3)
		return 1;
	if (x == 36 && y == 4)
		return 1;
	return 0;
}

void nextGenCA(char ** ca, char ** newCa, int width, int height) {

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {

			int neighbors = 0;

			if (i > 0) {
				if (j > 0)
					neighbors += ca[i - 1][j - 1];

				if (j < height - 1)
					neighbors += ca[i - 1][j + 1];

				neighbors += ca[i - 1][j];
			}

			if (i < width - 1) {
				if (j > 0)
					neighbors += ca[i + 1][j - 1];

				if (j < height - 1)
					neighbors += ca[i + 1][j + 1];

				neighbors += ca[i + 1][j];
			}

			if (j > 0)
				neighbors += ca[i][j - 1];

			if (j < height - 1)
				neighbors += ca[i][j + 1];

			if (ca[i][j] == 1) {
				if (neighbors == 2 || neighbors == 3)
					newCa[i][j] = 1;
				else
					newCa[i][j] = 0;
			} else {
				if (neighbors == 3)
					newCa[i][j] = 1;
			}
		}
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

			int freq = (int)(freqConst * (width - x - 1) );

			rez += (int)(volume * sin(freq * 3.1415 * 2 * s / samplingRate));
		}

		tmpData[s] = rez;

		if (abs(rez) > maxValue)
			maxValue = abs(rez);
	}

	if (maxValue == 0) maxValue = 1;

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

	char ** newCells = calloc(C_WIDTH, sizeof(char * ));

	for (int i = 0; i < C_WIDTH; ++i) {
		newCells[i] = calloc(C_HEIGHT, sizeof(char));
	}

	initCA(cells, C_WIDTH, C_HEIGHT);

	for (int i = 0; i < 1000; ++i) {
		nextGenCA(cells, newCells, C_WIDTH, C_HEIGHT);

		for (int j = 0; j <= C_HEIGHT; ++j) {
			for (int i = 0; i < C_WIDTH; ++i) {
				cells[i][j] = newCells[i][j];
			}
		}
	}

	int durationSeconds = 32;

	short int * audioData = createAudioData(newCells, C_WIDTH, C_HEIGHT, 44100, (44100 * durationSeconds));

	writeWav("gol.wav", (44100 * durationSeconds), audioData, 44100);

}
