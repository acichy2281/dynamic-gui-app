/* Project includes */
#include "stdafx.h"
#include "widget_image.h"

WidgetImage_C::WidgetImage_C(const std::shared_ptr<const AddImageWidgetInfo_T>& info) : WidgetInterface_I(info) 
{
    _isWritable = true;
    _isReadable = true;
    _isInteractable = false;
    _isStatic = false;

    std::cout << "Image path: " << info->imagePath << "\n";
    LoadImageFromFile(info->imagePath);
    std::cout << "Image texture created successfully\n";
    std::cout << "Image width: " << _width << ", height: " << _height << "\n";
}

WidgetImage_C::~WidgetImage_C()
{
    if (_sdlSurface != nullptr) {
        SDL_DestroySurface(_sdlSurface);
        _sdlSurface = nullptr;
    }
    if (_cameraTexture != 0) {
        glDeleteTextures(1, &_cameraTexture);
        _cameraTexture = 0;
    }
}

void WidgetImage_C::ShowWidget()
{
    if (_sdlSurface == nullptr) {
        return;
    }
    CreateTexture();
    if (_cameraTexture == 0) 
    {
        std::cerr << "Failed to create texture from image\n";
        return;
    }
    ImGui::Image((ImTextureID) (intptr_t) _cameraTexture, ImVec2(static_cast<float>(_width), static_cast<float>(_height)));
}

WidgetValueVariant_T WidgetImage_C::GetWidgetValue()
{
    // Return false as the button only indicates a press event and does not have a value. True is indicated when a button event occurs. 
    return false;
}

bool WidgetImage_C::SetWidgetValue(WidgetValueVariant_T val)
{
    // Cannot set value for a button widget
    return false;
}

void WidgetImage_C::LoadImageFromFile(const std::string &filePath)
{
    // Check if file exists
    std::ifstream fileCheck(filePath.c_str());
    if (!fileCheck) {
        fprintf(stderr, "Error: Image file '%s' does not exist\n", filePath.c_str());
        _sdlSurface = nullptr;
        return;
    }
    fileCheck.close();
    
    _sdlSurface = IMG_Load(filePath.c_str());
    if (!_sdlSurface) {
        fprintf(stderr, "IMG_Load failed\n");
        return;
    }
  
    // Retrieve image dimensions.
    _width = _sdlSurface->w;
    _height = _sdlSurface->h;
    
    // In SDL3, surface->format is now an enum value.
    Uint32 pixelFormatEnum = _sdlSurface->format;

    // Determine the number of bytes per pixel using the macro.
    int bpp = SDL_BITSPERPIXEL(pixelFormatEnum);
    int bytesPerPixel = bpp / 8;  // For typical 24-bit or 32-bit images

    // GLenum glFormat;
    switch (pixelFormatEnum)
    {
        // Common 24-bit formats
        case SDL_PIXELFORMAT_RGB24:
            _glFormat = GL_RGB;
            break;
        case SDL_PIXELFORMAT_BGR24:
            _glFormat = GL_BGR;
            break;

        // Common 32-bit formats.
        // Note: SDL_PIXELFORMAT_RGBA32 is an alias for SDL_PIXELFORMAT_ABGR8888 on little endian systems.
        case SDL_PIXELFORMAT_RGBA32:
            _glFormat = GL_RGBA;
            break;
        // In some cases the surface might be returned as BGRA (e.g. SDL_PIXELFORMAT_BGRA32 or SDL_PIXELFORMAT_ARGB8888).
        case SDL_PIXELFORMAT_BGRA32:
            _glFormat = GL_BGRA;
            break;
        default:
            // Fallback based on the bytes per pixel
            if (bytesPerPixel == 3)
                _glFormat = GL_RGB;
            else if (bytesPerPixel == 4)
                _glFormat = GL_RGBA;
            else {
                fprintf(stderr, "Unsupported pixel format: %d bits per pixel\n", bpp);
                SDL_DestroySurface(_sdlSurface);
                return;
            }
            break;
    }

}

void WidgetImage_C::CreateTexture()
{
    glGenTextures(1, &_cameraTexture);
    glBindTexture(GL_TEXTURE_2D, _cameraTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, _glFormat, _width, _height, 0, _glFormat, GL_UNSIGNED_BYTE, _sdlSurface->pixels);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}