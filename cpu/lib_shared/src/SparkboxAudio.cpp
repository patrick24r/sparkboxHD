#include "SparkboxAudio.h"

SparkboxAudio::SparkboxAudio(void)
{
  // Generate a blank audio file that won't play to fill
  // the activeAudio vector
  AudioFile blankAudio = AudioFile((uint8_t*)void, 0);
  blankAudio.sampleRate = 1;
  blankAudio.isPlaying = 0;
  blankAudio.numberOfSamples = 1;

  // Reserve up to 4 active audio slots
  activeAudio.reserve(MAX_AUDIO_CHANNELS);
  for (uint8_t i = 0; i < MAX_AUDIO_CHANNELS; i++) {
    activeAudio.push_back(blankAudio);
  }

  // Reserve a channel sample for each channel
  channelSamples.reserve(MAX_AUDIO_CHANNELS);
  for (uint8_t i = 0; i < MAX_AUDIO_CHANNELS; i++) {
    channelSamples.push_back(0);
  }

  rateCounter.reserve(MAX_AUDIO_CHANNELS);
  for (uint8_t i = 0; i < MAX_AUDIO_CHANNELS; i++) {
    rateCounter.push_back(1);
  }

  // Reserve up to 256 total audio files
  // Do not add dummy audio files here to be able to keep track of the
  // total number of audio files in the audio bank
  audioBank.reserve(MAX_AUDIO_BANK_SIZE);
  totalImportedAudioBytes = 0;

  // Initialize DACs
  HAL_DAC_Init(&hdac);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

  // Initialize Timers

}

SparkboxAudio::~SparkboxAudio(void)
{
  HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
  HAL_DAC_Stop(&hdac, DAC_CHANNEL_2);
  HAL_DAC_DeInit(&hdac);
}

// Add all audio files in a directory from disk
int32_t addAllAudio(std::string directory)
{
  // List all files in the directory
}

// Add a single audio file from disk
int32_t addAudioFile(std::string filePath)
{
  // Need at bare minimum "a.wav" or "b.mp3" - 5 characters
  if (filePath.length() < 5) return UNSUPPORTED_AUDIO_FILE;
  std::string ext = filePath.substr(filePath.length() - 4);

  // Check file extension to call correct add audio function
  if !(ext.compare(".wav") && ext.compare(".WAV")) return addWavFile(filePath);
  else if !(ext.compare(".mp3") && ext.compare(".MP3")) return addMp3File(filePath);
  else return UNSUPPORTED_AUDIO_FILE;
}

// Audio controlling functions
int32_t SparkboxAudio::playAudio(uint8_t audioChannel)
{
  // Play the audio file 1 time
  return playAudio(audioChannel, 1);
}

/*!
 * \brief Play an audio file that is active on a given channel
 * @param audioChannel the audio channel to play audio on (0:MAX_AUDIO_CHANNELS-1)
 */
int32_t SparkboxAudio::playAudio(uint8_t audioChannel, int32_t numberOfPlays)
{
  // Play an audio file on the given channel
  if (audioChannel >= activeAudio.size()) return AUDIO_INVALID_CHANNEL;

  // Determine current status of the audio file
}

int32_t SparkboxAudio::pauseAudio(uint8_t audioChannel)
{
  if (audioChannel >= activeAudio.size()) return AUDIO_INVALID_CHANNEL;
  activeAudio.at(audioChannel).isPlaying = AUDIO_NOT_PLAYING;
}

int32_t resumeAudio(uint8_t audioChannel)
{
  if (audioChannel >= activeAudio.size()) return AUDIO_INVALID_CHANNEL;
  activeAudio.at(audioChannel).isPlaying = AUDIO_PLAYING;
}

// Rewind audio called with no arguments, rewind to beginning
int32_t SparkboxAudio::rewindAudio(uint8_t audioChannel)
{
  if (audioChannel >= activeAudio.size()) return AUDIO_INVALID_CHANNEL;

  activeAudio.at(audioChannel).nextSample = 0;
  return 0;
}

int32_t SparkboxAudio::rewindAudio(uint8_t audioChannel, float numberOfSeconds)
{
  return skipAudio(audioChannel, 0.0 - numberOfSeconds);
}

int32_t SparkboxAudio::rewindAudio(uint8_t audioChannel, int32_t numberOfSamples)
{
  return skipAudio(audioChannel, 0 - numberOfSamples);
}

int32_t skipAudio(uint8_t audioChannel, float numberOfSeconds)
{
  int32_t samplesSkipped = (int32_t)(activeAudio.at(audioChannel).sampleRate * 
    numberOfSeconds);

  return skipAudio(audioChannel, samplesSkipped);
}

/*! 
 * \brief Skip a number of samples on one audio channel. Negative numbers
 * will skip backwards and positive numbers will skip forwards
 * @param audioChannel The audio channel
 * @param numberOfSamples The number of samples to skip
 */
int32_t skipAudio(uint8_t audioChannel, int32_t numberOfSamples)
{
  uint32_t newSampleIndex;
  int32_t nextSampleIndex;

  // Validate channel
  if (audioChannel >= activeAudio.size()) return AUDIO_INVALID_CHANNEL;
  nextSampleIndex = (int32_t)(activeAudio.at(audioChannel).nextSample);

  // If for some reason the audio file has ove 2 billion samples this
  // error will be called. With a file this large, this should error on memory 
  // limits
  if (nextSampleIndex < 0) return AUDIO_UNSUPPORTED_FILE;

  // Bounds check sample offset - coerce to limits of audio file
  if (numberOfSamples + nextSampleIndex < 0) {
    // Trying to rewind by too much, set next sample to first one
    newSampleIndex = 0;
  } else if (numberOfSamples + nextSampleIndex > (int32_t)activeAudio.at(audioChannel).numberOfSamples) {
    // Skipping past the end of the file, set next sample to last sample
    newSampleIndex = activeAudio.at(audioChannel).numberOfSamples - 1;
  } else {
    newSampleIndex = (uint32_t)(numberOfSamples + nextSampleIndex);
  }
 
  activeAudio.at(audioChannel).nextSample = newSampleIndex;

  return 0;
}

void SparkboxAudio::setChannelVolume(uint8_t audioChannel, float newVolume)
{
  if (audioChannel >= activeAudio.size()) return AUDIO_INVALID_CHANNEL;
  activeAudio.at(audioChannel).setVolume(newVolume);
}

float getMasterVolume(void)
{
  return masterVolume;
}

void setMasterVolume(float newVolume)
{
  // Coerce master volume to [0 1]
  if (newVolume < 0.0) masterVolume = 0.0;
  else if (newVolume > 1.0) masterVolume = 1.0;
  else masterVolume = newVolume; 
}

/*!
 * \brief Set the active audio on a channel to a file from the audio bank
 */
int32_t SparkboxAudio::setActiveAudio(uint8_t audioChannel, uint8_t audioID)
{
  if (audioID > audioBank.size()) return AUDIO_BANK_INVALID_ID;
  if (audioChannel > activeAudio.size()) return AUDIO_INVALID_CHANNEL;
  activeAudio.at(audioChannel) = audioBank.at(audioID);
}

AudioFile SparkboxAudio::getAudioBank(uint8_t audioID)
{
  if (audioID >= audioBank.size()) return *(AudioFile *)(void);
  return audioBank.at(audioID);
}

// Add a WAV file from disk
int32_t SparkboxAudio::addWavFile(std::string filePath)
{
  // Make sure file exists and can fit in memory
  // Convert to unsigned samples
}

// Add a MP3 file from disk
int32_t SparkboxAudio::addMp3File(std::string filePath)
{
  // Make sure file exists and can fit in memory
  // Convert to unsigned, uncompressed audio samples
}

// An audio timer has triggered, update audio if neccessary
void SparkboxAudio::audioInterruptCallback(uint32_t itSampleRate)
{
  // This function is called on any audio timer interrupt. As of now, 2 audio timers
  // are planned to be used - One triggering interrupts at 44.1kHz and one at 48kHz.
  // If any of the 4 active audio channels have sample rates that are a fraction of 
  // one of those samle rates, this function manages getting new samples for that channel
  // We need to be careful of unorthodox sample rates that are factors of both 44.1kHz 
  // and 48 kHz, such as 300 Hz (gcd of 44.1k and 48k). This is an outlandishly low
  // sample rate and it's not allowed.
  uint32_t chSampleRate;
  uint8_t channel;
  // Only sample rates evenly divisible by timer interrupt frequencies are allowed
  for (channel = 0; channel < activeAudio.size() && channel < MAX_AUDIO_CHANNELS; channel++) {
    chSampleRate = activeAudio.at(channel).sampleRate;

    // The sample rate for the current channel evenly divides,
    // but it may not be identical
    if (itSampleRate % chSampleRate == 0) {
      // Compare timer interrupt sample rate to the actual sample rate
      if (itSampleRate == chSampleRate) {
        getNewSample(channel);
      } else if (itSampleRate / chSampleRate >= rateCounter.at(channel)) {
        getNewSample(channel);
        rateCounter.at(channel) = 1;
      } else {
        rateCounter.at(channel)++;
      }
    }
  }
}

// Update the DAC with a new sample
void SparkboxAudio::getNewSample(uint8_t channel)
{
  auto file = activeAudio.begin() + channel;
  uint32_t newSample;
  int8_t totalBitShift = 0;
  // Audio mixing variables
  uint16_t singleChannelSample;
  uint16_t sampleToDac[2] = {0, 0};

  // If the audio channel is invalid, do nothing
  if (channel >= activeAudio.size()) return;

  // Get the next sample data for a valid channel
  if (file->isPlaying && file->nextSample < file->getNumberOfSamples()) {
    // Sample is valid, get sample data from storage
    if (file->bitsPerSample == BITS_PER_SAMPLE_16) totalBitShift++;
    if (file->numberOfChannels == CHANNEL_STEREO) totalBitShift++;
    newSample = (uint32_t)(*((uint8_t*)(file->getDataAddress()) + 
      file->nextSample << totalBitShift));

    // Account for mono/stereo and 8/16 bit audio
    if (file->numberOfChannels == CHANNEL_MONO) {
      newSample &= ~(0xFFFF0000);
      newSample |= (newSample << 16);

      // If 8 bit audio, shift 8 lsb to 8 msb
      if (file->bitsPerSample == BITS_PER_SAMPLE_8) {
        newSample <<= 8;
      }
    } else if (file->bitsPerSample == BITS_PER_SAMPLE_16) {
      // If 8 bit audio, shift 8 lsb to 8 msb
      newSample <<= 8;
    }

  } else {
    // Valid channel but no new valid sample is next, set to 0
    newSample = 0;
  }

  // Add sample data to the audio buffer
  channelSamples.at(channel) = newSample;

  // Mix audio over all channels
  for (channelNum = 0; channelNum < channelSamples.size(); channelNum++){
    // Unsigned samples are divided by 4 to prevent clipping when adding 4
    // waveforms. The samples for each channel also are multiplied by the audio file's
    // volume

    // Need to individually address left and right channel
    for (LorR = 0; LorR < 2; LorR++) {
      if (LorR)
        // Upper 16 bits 
        singleChannelSample = (uint16_t)(channelSamples.at(channelNum) >> 16);
      else {
        // Lower 16 bits
        singleChannelSample = (uint16_t)(channelSamples.at(channelNum) & 0x0000FFFF);
      }
      // For 4 channels, divide audio volume by 4 before mixing
      sampleToDac[LorR] += (uint16_t)((singleChannelSample >> 2) * file->getVolume() * masterVolume);
    }
  }

  // Write new samples to the DACs
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_L, sampleToDac[0]); 
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_L, sampleToDac[1]);

  // Take care of the case where the waveform has finished playing
  if (file->isPlaying) {
    if(++file->nextSample >= file->getNumberOfSamples())
      // Reset audio to starting sample
      file->nextSample = 0;

      // If audio is on repeat, keep track of the number of repetitions
      if (file->numberOfRepeats > 0) {
        // Any negative value for number of repeats is repeat forever
        file->numberOfRepeats--;

      } else if (file->numberOfRepeats == 0) {
        // Stop playing if no more repeats
        file->isPlaying = AUDIO_NOT_PLAYING;
      }
    }
  }
}