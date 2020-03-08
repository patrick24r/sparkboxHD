#include "SparkboxAudio.h"

SparkboxAudio::SparkboxAudio(void)
{
  DAC_ChannelConfTypeDef sConfig = {0};
  // Generate a blank audio file that won't play to fill
  // the activeAudio vector
  AudioFile blankAudio = AudioFile((uint8_t*)void, 0);
  blankAudio.sampleRate = 1;
  blankAudio.isPlaying = 0;
  blankAudio.numberOfSamples = 1;

  // Generate a blank audio tracker
  SparkboxAudioTracker initTracker;

  // Reserve up to 256 total audio files
  // Do not add dummy audio files here to be able to keep track of the
  // total number of audio files in the audio bank
  totalImportedAudioBytes = 0;
  audioBank.reserve(MAX_AUDIO_BANK_SIZE);
  

  // Reserve up to 4 active audio slots, for now make them blank audio
  activeAudio.resize(MAX_AUDIO_CHANNELS, blankAudio);

  // Initialize all audio trackers
  for (uint8_t i = 0; i < MAX_AUDIO_CHANNELS; i++) {
    initTracker = SparkboxAudioTracker(SINGLE_AUDIO_BUFFER_BYTES);
    audioTrackers.push_back(initTracker);
  }

  // Have active samles on DAC be 0 for now
  activeSamples.resize(MAX_AUDIO_BANK_SIZE, 0);

  // Max volume by default
  masterVolume = 1.0;
  
  
  // Initialize DACs ---------------------------------------------------
  hdac.Instance = DAC;
  HAL_DAC_Init(&hdac);
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  /* DAC channel OUT1 config */
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1);
  /* DAC channel OUT2 config */
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

  // Initialize Timers - both have base clocks of 84 MHz ----------------
  // Timer 11 - 44.1kHz = (84e6 / 1905)
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 0;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 1904;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim11);

  // Timer 14 - 48 kHz = (84e6 / 1750)
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 0;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 1749;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim14);
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
 * @brief Skip a number of samples on one audio channel. Negative numbers
 * will skip backwards and positive numbers will skip forwards
 * @param audioChannel The audio channel
 * @param numberOfSamples The number of samples to skip
 */
int32_t skipAudio(uint8_t audioChannel, int32_t numberOfSamples)
{

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
 * @brief Set the active audio on a channel to a file from the audio bank
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
  uint8_t sampleIsNew = 0;

  // Only sample rates evenly divisible by timer interrupt frequencies are allowed
  for (uint8_t channel = 0; channel < activeAudio.size() && channel < MAX_AUDIO_CHANNELS; channel++) {
    chSampleRate = activeAudio.at(channel).sampleRate;

    // Do nothing if the audio is not playing
    if (!audioTrackers.at(channel).isPlaying) continue;

    // The sample rate for the current channel evenly divides,
    // but it may not be identical
    if (itSampleRate % chSampleRate == 0) {
      // Compare timer interrupt sample rate to the actual sample rate
      if (itSampleRate == chSampleRate) {
        getNextSample(channel);
        sampleIsNew = true;
      } else if (itSampleRate / chSampleRate >= audioTrackers.at(channel).lowerSampleRateCounter) {
        getNextSample(channel);
        rateCounter.at(channel) = 1;
        sampleIsNew = true;
      } else {
        rateCounter.at(channel)++;
      }
    }
  }

  // Mix audio and write to DAC if a sample updated
  if (sampleIsNew) writeNewSampleDacs();
  
  // Update all audio streams, set up DMA queue
  updateAudioStreams();
}

// Update the auio trackers to get the current sample
void SparkboxAudio::getNextSample(uint8_t channel)
{
  uint32_t newSample = 0;

  // Wait until the current internal buffer is ready
  while (!audioTrackers.at(i).currentHalfBufferReady);

  // Using audio tracker, identify the next sample in the internal data stream
  newSample = *(uint32_t*)(audioTrackers.at(channel).nextInternalSampleToPlay);

  // If an audio file just finished, update number of repeats
  if (audioTrackers.at(channel).) {
    if (audioTrackers.at(channel).numberOfRepeats > 1) {
      audioTrackers.at(channel).numberOfRepeats -= 1;
    } else if (audioTrackers.at(channel).numberOfRepeats == 1) {
      // Single play done, stop playing the audio
      audioTrackers.at(channel).numberOfRepeats = 0;
      audioTrackers.at(channel).isPlaying = 0;
    }
  }

  // Save the new active sample for the new channel
  activeSamples.at(channel) = newSample;
}

// Mix audio and write the combined samples to the DACs
void SparkboxAudio::writeNewSampleDacs(void) 
{
  uint32_t activeChannelSample = 0;
  uint16_t leftChannelSample = 0;
  uint16_t rightChannelSample = 0;

  for (uint8_t i = 0; i < MAX_AUDIO_CHANNELS; i++) {
    // Mix audio from active samples (Assuming 16 bit stereo, unsigned data)
    activeChannelSample = activeSamples.at(i);

    // Sum all channel audio, scaling by the number of channels to prevent clipping
    leftChannelSample += (uint16_t)(uncorrectedSample) / MAX_AUDIO_CHANNELS;
    rightChannelSample += (uint16_t)(uncorrectedSample >> 16) / MAX_AUDIO_CHANNELS;
  }

  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_L, leftChannelSample); 
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_L, rightChannelSample);
}


void SparkboxAudio::dmaCompleteCallback(void)
{
  // DMA complete, if queue is not empty, begin next transfer(s)
}

void SparkboxAudio::updateAudioStreams(void)
{
  // Add a request to the dma queue
  // Add mmultiple requests to the queue in the case that
  // the file is ending and we need to transfer samples fromthe start of
  // the waveform
  // Begin a dma transfer if no dma transfer is active
}