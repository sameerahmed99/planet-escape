//@TODO, remove unecessary conversions from frames to bytes and vice versa, use frames for all things except when required (such as when copying bytes)

internal void win32_init_wasapi(DesiredAppSettings desired, Win32State *state) {

  _log("Initializing WASAPI\n");  
  i32 numChannels = desired.audioChannelsCount;
  i32 bitDepth = desired.audioBitDepth;
  i32 sampleRate = desired.audioSampleRate;
  r32 bufferDurationSec = desired.audioBufferSizeInSeconds;

  IMMDevice *device;

  IMMDeviceEnumerator *enumerator;
  HRESULT hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void **)&enumerator);

  if (hr != S_OK) {
    _log("WASAPI Init failed\n");
    Assert(hr==S_OK);  
  }

  _log("Win32::WASAPI initializing\n");
  hr = enumerator->lpVtbl->GetDefaultAudioEndpoint(enumerator, eRender, eMultimedia, &device);

  if(hr!=S_OK){
    _log("Win32::WASAPI couldn't get audio endpoint\n");
  }
  _log("Win32::WASAPI got audio endpoint\n");
  void *ppInterface;
  device->lpVtbl->Activate(device, &IID_IAudioClient, CLSCTX_ALL, NULL, &ppInterface);
  _log("Win32::WASAPI device activated\n");

  WAVEFORMATEX format;

  mi bytesPerFrame = numChannels*bitDepth/8;
  
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nChannels = numChannels;
  format.nSamplesPerSec = sampleRate;
  format.nAvgBytesPerSec = bytesPerFrame * sampleRate;
  format.nBlockAlign = bytesPerFrame;
  format.wBitsPerSample = bitDepth;
  format.cbSize = 0;



  IAudioClient *audioClient = (IAudioClient *)ppInterface;

  hr = audioClient->lpVtbl->Initialize(audioClient,

                                       AUDCLNT_SHAREMODE_SHARED, 0, (u64)Win32SecToReftimeUnits(bufferDurationSec), 0, &format, NULL);

  if (hr != S_OK) {
    _log("Sumtin wong audioClient.Initialize\n");
    Assert(hr==S_OK);  
  }

  _log("Win32::WASAPI initialized audio client\n");

  IAudioRenderClient *renderClient = NULL;
  audioClient->lpVtbl->GetService(audioClient, &IID_IAudioRenderClient, (void **)&renderClient);

  audioClient->lpVtbl->Start(audioClient);
  _log("Win32::WASAPI audio service started\n");

  state->wasapiAudioClient = audioClient;
  state->wasapiRenderClient = renderClient;

  mi bufferSizeFrames;

  hr = state->wasapiAudioClient->lpVtbl->GetBufferSize(state->wasapiAudioClient, (u32*)&bufferSizeFrames);




  _log("Win32::WASAPI buffer first release\n");
  state->app.audioArena = arena_create(MB(1),MB(100));
  
  audio_init(&state->app.audio,state->app.audioArena, numChannels, sampleRate, bitDepth, bufferSizeFrames);

  _log("WASAPI initialized\n");  
}
internal void win32_wasapi_on_frame_start(Win32State *state){

  u32 wasapiNumFramesPadding;

  state->wasapiAudioClient->lpVtbl->GetCurrentPadding(state->wasapiAudioClient, &wasapiNumFramesPadding);

  u32 numFramesToWrite = state->app.audio.master.sizeInFrames - wasapiNumFramesPadding;

  state->app.audio.writeLengthFrames = numFramesToWrite;
  
  //state->wasapiWriteLengthFrames = numFramesToWrite;
  //memset(state->audioBufferPointer, 0, state->wasapiBufferSizeInBytes);
}
internal void win32_wasapi_copy_buffer(Win32State *state) {

  if (state->app.audio.writeLengthFrames == 0)
    return;


  AudioState audio = state->app.audio;
  mi bpf = audio.master.bytesPerFrame;
  
  u8 *data;
  HRESULT hr = state->wasapiRenderClient->lpVtbl->GetBuffer(state->wasapiRenderClient, audio.writeLengthFrames, &data);

  if(hr == AUDCLNT_E_DEVICE_INVALIDATED){
    _log("WASAPI::Audio device invalidated\n");
  }
  else if(hr ==AUDCLNT_E_NOT_INITIALIZED ){
    _log("WASAPI::Audio device not initialized\n");
  }
  else if(hr ==AUDCLNT_E_RESOURCES_INVALIDATED ){
    _log("WASAPI::Audio resources invalidated\n");
  }
  else if(hr ==AUDCLNT_E_SERVICE_NOT_RUNNING ){
    _log("WASAPI::Audio service not running\n");
  }



  i32 dif  =(audio.master.sizeInFrames) - (audio.writePosFrames +audio.writeLengthFrames);
  if( dif< 0){
    

    u32 writeLength1 = (audio.master.sizeInFrames - audio.writePosFrames);
    u32 writeLength2 = (audio.writeLengthFrames - writeLength1);
    
    MemoryCopy(data, audio.master.data + audio.writePosFrames * bpf, writeLength1*bpf);
    MemoryCopy(data+writeLength1*bpf, audio.master.data, writeLength2*bpf);
  }

  else{
    MemoryCopy(data, audio.master.data + audio.writePosFrames*bpf, audio.writeLengthFrames*bpf);
  }
  

  hr = state->wasapiRenderClient->lpVtbl->ReleaseBuffer(state->wasapiRenderClient, audio.writeLengthFrames, 0);
  if (hr != S_OK) {
    exit(1);
  }
}

