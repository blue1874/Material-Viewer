#pragma once

#include <type_traits>

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mgl/uniform.h"
#include "mgl/shader.h"
#include "tool/path.h"

#define MACRO_TO_STR(M) #M

class baseToGui
{
private:
public:
    std::string name;
    baseToGui(std::string _name) : name(_name) {}
    virtual void toGui() {}
};

template<class T>
class enumToGui : public baseToGui
{
private:
    std::vector<std::string> optionStr;

public:
    size_t selected_index;
    T selected_value;
    std::vector<T> options;
    enumToGui(std::string&& _name, std::vector<std::string>&& _optionStr, std::vector<T>&& _options, size_t _index = 0) :
        baseToGui(_name), optionStr(_optionStr), options(_options), selected_index(_index)
    {
        selected_value = options[selected_index];
    }
    void toGui()
    {
        if (ImGui::BeginCombo(name.c_str(), optionStr[selected_index].c_str()))
        {
            for (size_t n = 0; n < optionStr.size(); n++)
            {
                const bool is_selected = (selected_index == n);
                if (ImGui::Selectable(optionStr[n].c_str(), is_selected))
                {
                    selected_value = options[n];
                    selected_index = n;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
};

class boolToGui : public baseToGui
{
private:
public:
    bool value;
    boolToGui(std::string&& _name, bool defaultValue = false) : baseToGui(_name), value(defaultValue) {}
    void toGUi()
    {
        ImGui::Checkbox(name.c_str(), &value);
    }
};

class colorToGui : public baseToGui
{
private:
public:
    glm::vec4 color;
    colorToGui(std::string&& _name, glm::vec4 defaultValue) : baseToGui(_name), color(defaultValue) {}
    void toGUi()
    {
        // why color wiget not show ?!
        ImGui::ColorEdit4(name.c_str(), (float*)&color);
        1;
    }
};

class fileGui : public baseToGui
{
private:
public:
	std::string filePath;
	ImGui::FileBrowser fileBrowser;
	bool needReload = false;
    fileGui(std::string&& _name, const std::string &defaultFilePath = "") : baseToGui(_name), filePath(defaultFilePath) {
        fileBrowser.SetTitle(_name);
        fileBrowser.SetPwd(pathLoader::cwd);
    }
    void toGUi()
    {
        ImGui::Text((name + " path: " + filePath).c_str());
        if (ImGui::Button((name + "file browser").c_str())) 
            fileBrowser.Open();
        ImGui::SameLine(300);
        if (ImGui::Button((name + "clear").c_str())) {
            filePath = "";
            needReload = true;
        }
        fileBrowser.Display();
        if (fileBrowser.HasSelected())
        {
            if (filePath.compare(fileBrowser.GetSelected().string())) {
                filePath = fileBrowser.GetSelected().string();
                needReload = true;
            }
            fileBrowser.ClearSelected();
        }
    }
};

namespace RenderOption {

    inline boolToGui debugDraw("debug");
    // using RenderPointer = std::shared_ptr<baseToGui>;
    // select a polygon rasterization mode
    inline enumToGui polygonMode("polygonMode",
        std::vector<std::string>{"GL_POINT", "GL_LINE", "GL_FILL"},
        std::vector<size_t>{GL_POINT, GL_LINE, GL_FILL}, 2);

    inline boolToGui drawCubeMap("drawCubemap", false);

    // depth test
    inline boolToGui depthTest("depthTest", true);
    inline boolToGui depthMask("depthMask", true);
    inline enumToGui depthFunc("glDepthFunc", 
    std::vector<std::string>{"GL_NEVER", "GL_LESS", "GL_EQUAL", "GL_LEQUAL", "GL_GREATER", "GL_NOTEQUAL", "GL_GEQUAL", "GL_ALWAYS"},
    std::vector<size_t>{GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS}, 3);

    // stencil test
    inline boolToGui stencilTest("stencilTest", false);

    // color buffer
    inline colorToGui clearColor("clearColor", glm::vec4(0.45f, 0.55f, 0.60f, 1.00f));

    // cull face
    inline boolToGui cullFace("cullFace", true);
    inline enumToGui cullFaceFunc("glCullFaceFunc", 
    std::vector<std::string>{"GL_BACK", "GL_FRONT", "GL_FRONT_AND_BACK"},
    std::vector<size_t>{GL_BACK, GL_FRONT, GL_FRONT_AND_BACK}, 0);

	// model file
	// inline fileGui modelFile("select model file", DefaultWorkFlow::MODEL_PATH);

    inline void preRenderOption()
    {
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode.selected_value);
        glClearColor(clearColor.color.x, clearColor.color.y, clearColor.color.z, clearColor.color.w);

        depthTest.value ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        glDepthMask(depthMask.value);
        glDepthFunc(depthFunc.selected_value);

        cullFace.value ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        glCullFace(cullFaceFunc.selected_value);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    };
    inline void fboRenderOption()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
    };
    inline void postRenderOption()
    {

    };
};



inline auto hookfunction(std::shared_ptr<Shader> shader, std::string name)
{
    return [=](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>)
        {
            int _arg = arg;
            ImGui::InputInt(name.c_str(), &_arg);
            shader->updateUniform(name, _arg);
        }
        else if constexpr (std::is_same_v<T, size_t>)
        {
            int _arg = arg;
            ImGui::InputInt(name.c_str(), &_arg);
            shader->updateUniform(name, _arg);
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            float _arg = arg;
            ImGui::InputFloat(name.c_str(), &_arg);
            shader->updateUniform(name, _arg);
        }
        else if constexpr (std::is_same_v<T, glm::vec3>)
        {
            glm::vec3 _arg = arg;
            ImGui::InputFloat3(name.c_str(), (float*)&_arg);
            shader->updateUniform(name, _arg);
        }
        else
        {

        }
    };
}

