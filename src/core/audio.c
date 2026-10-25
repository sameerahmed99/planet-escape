    
void audio_init(AudioState *state, Arena *arena, i32 channels, i32 sampleRate, i32 bitDepth, mi bufferSizeFrames){
  // we only support 16 bit depth for now
  Assert(bitDepth == 16);

  state->arena = arena;

  state->master.channels = channels;
  state->master.bitDepth = bitDepth;
  state->master.sampleRate = sampleRate;
  state->master.sizeInFrames = bufferSizeFrames;
  state->master.bytesPerFrame = channels*bitDepth / 8;
  state->master.sizeInBytes = state->master.bytesPerFrame * bufferSizeFrames;


  state->master.data = (u8*)arena_push(state->arena, sizeof(u8)*state->master.sizeInBytes, state->master.bytesPerFrame, True);

  state->buffers=PushArray(state->arena, QueuedAudioBuffer, MAX_AUDIO_BUFFERS);
}

QueuedAudioBuffer *audio_queue_buffer(AudioState *state, AudioBuffer *buffer, r32 volumeScale){
  QueuedAudioBuffer *q;
  for(int i=0;i<MAX_AUDIO_BUFFERS;i++){
    QueuedAudioBuffer *buf=&state->buffers[i];
    if(!buf->copying){
      q = buf;
      break;
    }
  }


  q->buffer = buffer;
  q->volumeScale = volumeScale;
  q->cursorFrames = 0;
  q->copying = True;
  return q;
}




void audio_on_update(AudioState *state, r32 dt){
  state->writePosFrames = (state->writePosFrames + state->writtenFramesPrev)
    % state->master.sizeInFrames;

  mi writePos = state->writePosFrames;
  mi write = state->writeLengthFrames;




  // bytes per frame
  mi bpf = state->master.bytesPerFrame;

  {
    mi length1 = write;
    mi length2 = 0;

    if(length1+writePos > state->master.sizeInFrames){
      length1 = state->master.sizeInFrames - writePos;
      length2 = write - length1;
    }
    MemorySet(state->master.data+writePos*bpf, 0, length1*bpf);
    if(length2>0){
      MemorySet(state->master.data, 0, length2*bpf);
    }
  }
  
  /* { */
  /* //////////////////////////// */
  /* // ~sameer: SQUARE WAVE TEST, @NOTE: Assumes 16 bit depth. Modify if using other bit depths */

  /*   r32 frequency = state->testWaveFrequency; */
  /*   local_persist r32 phase = 0.0f; */
  /*   r32 phaseOffsetPerFrame =  1.0f / state->master.sampleRate; */
  /*   phaseOffsetPerFrame *=frequency; */


  /*   for(int i=0;i<write;i++){ */
  /*     writePos %=state->master.sizeInFrames; */
  /*     u8 *dst = state->master.data+writePos*bpf; */
  /*     phase+=phaseOffsetPerFrame*2; // because we're going from -1 to 1 (range length of 2) instead of 0 to 1, */
  /*     if(phase>=1.0f){ */
  /* 	phase -=2.0f; */
  /*     } */

  /*     i16 val = 30000/4 * (phase >= 0 ? 1 : -1); */
  /*     i16 *d = (i16*) dst; */
  /*     for(int y=0;y<state->master.channels;y++){ */

  /* 	*d = val; */
  /* 	d++; */
  /*     } */
  /*     writePos++; */
  /*   } */
  /* } */
  for(int i=0;i<MAX_AUDIO_BUFFERS;i++){
    QueuedAudioBuffer *buff=&state->buffers[i];
    if(!buff->copying) continue;






    r32 ampScale = buff->volumeScale;
    /* mi bytePos = writePos * bpf; */


    /* mi writeNumBytes = Min(write*bpf, buff->buffer->sizeInBytes - buff->cursorFrames*bpf); */
    /* mi writeNumFrames = writeNumBytes/bpf; */


    mi framesToCopy = Min(write, buff->buffer->sizeInFrames - buff->cursorFrames);

    mi futureWritePosFrames = writePos + framesToCopy;
    /* mi futurePos = bytePos + writeNumBytes; */


    mi write1 = framesToCopy;
    mi write2 = 0;
    if(futureWritePosFrames>state->master.sizeInFrames){
      write1 = state->master.sizeInFrames - writePos;
      write2=framesToCopy - write1;
    }


    {
      i16 *target = (i16*)(state->master.data+writePos*bpf);
      i16 *read = (i16*)(buff->buffer->data+buff->cursorFrames*bpf);
      for(i32 d = 0;d<write1*state->master.channels;d++){
	i16 val = (i16)(read[d]*ampScale);
	target[d]+=val;
      }
    }

    if(write2>0){
      {
	i16 *target = (i16*)(state->master.data);
	i16 *read = (i16*)(buff->buffer->data+(buff->cursorFrames+write1)*bpf);
	for(i32 d = 0;d<write2 * state->master.channels;d++){
	  i16 val = (i16)(read[d]*ampScale);
	  target[d]+=val;
	}
      }
    }

    buff->cursorFrames+= framesToCopy;



    
    if(buff->cursorFrames==buff->buffer->sizeInFrames){
      buff->copying = False;
    }
    else{
      Assert(buff->cursorFrames < buff->buffer->sizeInFrames);
    }
    
  }

  state->writtenFramesPrev = write;

}
AudioBuffer audio_buffer_from_file_path_ogg_vorbis_16bit(Arena *arena, string8 path){

  //@TODO, enforce/scale bit depth and sample rate to always match master buffer
  
  string8 data = file_read_from_path_into_buffer(arena, path);
  i32 channels, sampleRate;

  // stb_vorbis_decode_memory's decoded parameter is short **
  // short is 16 bit, so assume returned bit depth is 16
  // 16 bit is enough, if you need higher bit depth for some reason, create another function with 24bit in the name
  // and load floats from vorbis and then scale them to 24bit max value

  i16 *decoded;
  i32 framesDecoded = stb_vorbis_decode_memory(data.ptr, data.length, &channels, &sampleRate, &decoded);


  // keep everything stereo for simplification
  Assert(channels == 2);

  AudioBuffer buffer;
  buffer.channels = channels;
  buffer.sampleRate = sampleRate;
  buffer.bitDepth = 16;
  buffer.sizeInBytes = audio_bytes_to_frames(framesDecoded, channels, 16);
  buffer.sizeInFrames = framesDecoded;
  buffer.bytesPerFrame = 16*channels/8;
  buffer.data = (u8*)arena_push(arena, buffer.sizeInBytes, sizeof(u8),True);
  MemoryCopy(buffer.data, decoded, buffer.sizeInBytes);
  free(decoded);
  return buffer;

}

i32 audio_bytes_to_frames(i32 frames, i32 channels, i32 bitDepth){
  return frames*channels*bitDepth/8;
}
