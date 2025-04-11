#ifndef WIDGET_IMAGE_H
#define WIDGET_IMAGE_H

#include "stdafx.h"
#include "widget_interface.h"
#include "thread_safe_queue.h"


struct AddImageWidgetInfo_T : public AddWidgetInfo_T {
    std::string imagePath;
};

class WidgetImage_C : public WidgetInterface_I
{
    public:
        WidgetImage_C(const std::shared_ptr<const AddImageWidgetInfo_T>& info);
        ~WidgetImage_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Button; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::Bool; } 

    private:
        void LoadImageFromFile(const std::string &filePath);
        void CreateTexture();
        int _width;
        int _height;
        GLuint _cameraTexture;
        GLenum _glFormat;
        SDL_Surface* _sdlSurface;
};
#endif // WIDGET_IMAGE_H
