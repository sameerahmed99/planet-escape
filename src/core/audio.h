#ifndef AUDIO_INCLUDE
#define AUDIO_INCLUDE

#define MAX_AUDIO_BUFFERS 16


typedef struct AudioStream AudioStream;

struct AudioStream{
  r32 volumeScale;
  string8 filePath;
  b32 copying;
  mi fileCursorFrames;
};

typedef struct AudioBuffer AudioBuffer;

struct AudioBuffer {
  u8 *data;
  i32 channels;
  i32 sampleRate;
  i32 bitDepth;
  i32 bytesPerFrame;
  i32 sizeInFrames;
  i32 sizeInBytes;
};

typedef struct QueuedAudioBuffer QueuedAudioBuffer;

struct QueuedAudioBuffer {
  AudioBuffer *buffer;
  r32 volumeScale;
  mi cursorFrames;
  b32 copying;
};



typedef struct AudioState AudioState;

struct AudioState{
  Arena *arena;
  AudioBuffer master;
  QueuedAudioBuffer *buffers;

  mi writtenFramesPrev;
  mi writePosFrames;
  mi writeLengthFrames;
  r32 testWaveFrequency;


};


void audio_init(AudioState *state, Arena *arena, i32 channels, i32 sampleRate, i32 bitDepth, mi bufferSizeFrames);

// return buffer and stream pointer to user, so that they can cancel it or change volume and such
QueuedAudioBuffer *audio_queue_buffer(AudioState *state, AudioBuffer *buffer, r32 volumeScale);
AudioStream *audio_stream_from_file(AudioState *state, string8 path, r32 volumeScale);

AudioBuffer audio_buffer_from_file_path_ogg_vorbis_16bit(Arena *arena, string8 path);

i32 audio_bytes_to_frames(i32 frames, i32 channels, i32 bitDepth);



#endif // CORE_AUDIO_INCLUDE
