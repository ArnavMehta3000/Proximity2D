#include "enginepch.h"
#include "Engine/Modules/TextureLoader.h"
#include "Graphics/D3DManager.h"

namespace Proximity::Modules
{
	bool TextureLoader::CreateBinaryFromImage(const std::string& imageFile, const std::string& outputFile)
	{
        Timer loadTime;
        // Get image data
        std::ifstream file;
        file.open(imageFile, std::ios::binary | std::ios::ate);

        if (!file.is_open())
        {
            PRX_LOG_ERROR("Failed to open image file [%s]", imageFile.c_str());
            return false;
        }
        loadTime.Start();

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size))
        {
            PRX_LOG_ERROR("Failed to read image data [%s]", imageFile.c_str());
            return false;
        }

        file.close();

        std::vector<unsigned char> imageData(buffer.begin(), buffer.end());


        // Serialize the image data
        std::ofstream output(outputFile, std::ios::binary);
        output.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());
        output.close();

        loadTime.Stop();
        PRX_LOG_INFO("Converted image [%s] to binary in %.2fms\nOutput: %s", imageFile.c_str(), (loadTime.TotalTime() * 1000.0f), outputFile.c_str());

        return true;
	}

    bool TextureLoader::CreateTextureFromBinary(Graphics::Texture2D* outputTex, const std::string& binFile)
    {
        // Get image file dimensions
        if (!GetImageSize(binFile, &outputTex->Width, &outputTex->Height))
            return false;

        std::ifstream file(binFile, std::ios::binary);
        if (!file.is_open())
            return false;
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
        for (auto& buf : buffer)
        {
            std::cout << buf << std::endl;
        }
        file.close();

        outputTex->TexFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        outputTex->BindFlags = D3D11_BIND_SHADER_RESOURCE;

        CREATE_ZERO(D3D11_TEXTURE2D_DESC, desc);
        desc.Width            = outputTex->Width;
        desc.Height           = outputTex->Height;
        desc.ArraySize        = 1;
        desc.MipLevels        = 1;
        desc.Format           = outputTex->TexFormat;
        desc.Usage            = D3D11_USAGE_DEFAULT;
        desc.BindFlags        = outputTex->BindFlags;
        desc.CPUAccessFlags   = 0;
        desc.SampleDesc.Count = 1;
        desc.MiscFlags        = 0;

        CREATE_ZERO(D3D11_SUBRESOURCE_DATA, initialData);
        initialData.pSysMem          = buffer.data();
        initialData.SysMemPitch      = outputTex->Width * 4;
        initialData.SysMemSlicePitch = 0;


        auto d3d = PRX_RESOLVE(Graphics::D3DManager);
        HRESULT hr = d3d->GetDevice()->CreateTexture2D(&desc, &initialData, outputTex->D3DTexture2D.ReleaseAndGetAddressOf());
        PRX_FAIL_HR(hr);
        buffer.clear();

        return true;
    }

    bool TextureLoader::GetImageSize(const std::string& filename, Math::U32* x, Math::U32* y)
    {
        Timer loadTime;

        FILE* file;
        fopen_s(&file, filename.c_str(), "rb");

        if (file == 0)
        {
            PRX_LOG_ERROR("Invalid image filename");
            return false;
        }

        fseek(file, 0, SEEK_END);
        long len = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (len < 24)
        {
            fclose(file);
            PRX_LOG_ERROR("Failed to get image size. Check filename/filepath [%s]", filename.c_str());
            return false;
        }

        // Strategy:
        // reading GIF dimensions requires the first 10 bytes of the file
        // reading PNG dimensions requires the first 24 bytes of the file
        // reading JPEG dimensions requires scanning through jpeg chunks
        // In all formats, the file is at least 24 bytes big, so we'll read that always
        unsigned char buf[24]; fread(buf, 1, 24, file);

        // For JPEGs, we need to read the first 12 bytes of each chunk.
        // We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
        if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF && buf[3] == 0xE0 && buf[6] == 'J' && buf[7] == 'F' && buf[8] == 'I' && buf[9] == 'F')
        {
            long pos = 2;
            while (buf[2] == 0xFF)
            {
                if (buf[3] == 0xC0 || buf[3] == 0xC1 || buf[3] == 0xC2 || buf[3] == 0xC3 || buf[3] == 0xC9 || buf[3] == 0xCA || buf[3] == 0xCB) break;
                pos += 2 + (buf[4] << 8) + buf[5];
                if (pos + 12 > len) break;
                fseek(file, pos, SEEK_SET); fread(buf + 2, 1, 12, file);
            }
        }

        fclose(file);

        // JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
        if (buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF)
        {
            *y = (buf[7] << 8) + buf[8];
            *x = (buf[9] << 8) + buf[10];
            //cout << *x << endl;
            return true;
        }

        // GIF: first three bytes say "GIF", next three give version number. Then dimensions
        if (buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F')
        {
            *x = buf[6] + (buf[7] << 8);
            *y = buf[8] + (buf[9] << 8);
            return true;
        }

        // PNG: the first frame is by definition an IHDR frame, which gives dimensions
        if (buf[0] == 0x89 && buf[1] == 'P' && buf[2] == 'N' && buf[3] == 'G' && buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A
            && buf[12] == 'I' && buf[13] == 'H' && buf[14] == 'D' && buf[15] == 'R')
        {
            *x = (buf[16] << 24) + (buf[17] << 16) + (buf[18] << 8) + (buf[19] << 0);
            *y = (buf[20] << 24) + (buf[21] << 16) + (buf[22] << 8) + (buf[23] << 0);
            return true;
        }

        loadTime.Stop();
        PRX_LOG_INFO("Read image dimensions took %.2f", (loadTime.TotalTime() * 1000.0f));

        return false;
    }
}