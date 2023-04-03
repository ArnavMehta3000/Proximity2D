#include "enginepch.h"
#include "Audio/AudioEngine.h"

namespace Proximity::Audio
{
    // Find chunk in a RIFF (.wav) file
    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;

        while (hr == S_OK)
        {
            DWORD dwRead;
            if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;

            default:
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
            }

            dwOffset += sizeof(DWORD) * 2;

            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return S_OK;
            }

            dwOffset += dwChunkDataSize;

            if (bytesRead >= dwRIFFDataSize) return S_FALSE;

        }

        return S_OK;

    }

    // Read data from chunk
    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwRead;
        if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }



	bool AudioEngine::Init()
	{
		HRESULT hr = XAudio2Create(m_xAudio.ReleaseAndGetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
		PRX_ASSERT_HR(hr, "Audio Engine: XAudio2Create failed");
		
		hr = m_xAudio->CreateMasteringVoice(&m_masteringVoice);
		PRX_ASSERT_HR(hr, "Audio Engine: failed to create mastering voice");
		
		m_xAudio->StartEngine();


        CREATE_ZERO(WAVEFORMATEXTENSIBLE, wfx);
        CREATE_ZERO(XAUDIO2_BUFFER, buffer);
        
        auto file = TEXT("Test/MyWav.wav");
        HANDLE hFile = CreateFileW(
            file,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
            hr = HRESULT_FROM_WIN32(GetLastError());
		
        PRX_ASSERT_HR(hr, "Audio Engine: Invalid handle");

        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            hr = HRESULT_FROM_WIN32(GetLastError());

        PRX_ASSERT_HR(hr, "Audio Engine: Invalid file pointer");


        DWORD dwChunkSize;
        DWORD dwChunkPosition;
        //check the file type, should be fourccWAVE or 'XWMA'
        FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
        DWORD filetype;
        ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
        if (filetype != fourccWAVE)
            return false;

        FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
        ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

        FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
        BYTE* pDataBuffer = new BYTE[dwChunkSize];
        ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

        buffer.AudioBytes = dwChunkSize;      // size of the audio buffer in bytes
        buffer.pAudioData = pDataBuffer;      // buffer containing audio data
        buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

        IXAudio2SourceVoice* pSourceVoice;
        if (FAILED(hr = m_xAudio->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx)))
            PRX_ASSERT_HR(hr, "Failed to create source");

        if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
            PRX_ASSERT_HR(hr, "Failed to submit source");

        if (FAILED(hr = pSourceVoice->Start(0)))
            PRX_ASSERT_HR(hr, "Failed to start source");

		return true;
	}

   	void AudioEngine::Shutdown()
	{
		m_xAudio->StopEngine();
		m_xAudio->Release();
	}
}