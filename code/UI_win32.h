#include "..\..\imgui\imgui.cpp"
#include "..\..\imgui\imgui.h"
//#include "..\..\imgui\imgui_demo.cpp"
#include "..\..\imgui\imgui_draw.cpp"
#include "..\..\imgui\imgui_internal.h"
#include "..\..\imgui\imconfig.h"

void UIRenderer(ImDrawData* draw_data)
{
    ImGuiIO& io { ImGui::GetIO() };
    int fb_width { (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x) };
    int fb_height { (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y) };
    
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);
    
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    
    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
        const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));
        
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
#undef OFFSETOF
    // Restore modified state
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void Win32InitGUI(int Width, int Height, float dt)
{
    unsigned char* pixels;
    int width,
    height,
    display_width,
    display_height;
    GLuint g_FontTexture;
    // TODO(Barret5Ocal): Figure out how to rasterize a font 
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);
    
    io.DisplaySize = {(float)Width, (float)Height};
    io.DeltaTime = dt;
    io.RenderDrawListsFn = UIRenderer;
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
    
    glBindTexture(GL_TEXTURE_2D, last_texture);
    
    
}

char buf[256];
float f;

void Win32RunGUI(int LeftMouseClick, float MousePos[2])
{
    // Input 
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = {MousePos[0], MousePos[1]};
    //io.MouseDown[0] = 0;
    io.MouseDown[0] = LeftMouseClick;
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui::NewFrame();
    
    ImGui::SetNextWindowSize(ImVec2(500,500)); 
    ImGui::Begin("Window Title Here");
    //ImGui::Text("Hello, world %d", 123);
    
    if(ImGui::Button("OK")){Assert(false);}
    
    ImGui::Text("%.09f %.09f", MousePos[0], MousePos[1]);
    //ImGui::InputText("string", buf, 256);
    //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ///ImGui::Text("Hello, world!");
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    
}
