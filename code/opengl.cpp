GLuint LoadShaders(char *VertexShaderCode, char *FragmentShaderCode)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(VertexShaderID, 1, &VertexShaderCode, 0);
    glCompileShader(VertexShaderID);
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *VertexErrorMessage = (char *)malloc(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexErrorMessage);
        Assert(false);
        free(VertexErrorMessage);
    }
    
    // Compile Fragment Shader
    glShaderSource(FragmentShaderID, 1, &FragmentShaderCode , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *FragmentErrorMessage = (char *)malloc(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentErrorMessage);
        Assert(false);
        free(FragmentErrorMessage);
    }
    
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        Assert(false);
    }
    
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}

unsigned int cubeVAO, cubeVBO;
GLuint Program; 
GLuint MatrixID;
GLuint ViewPosID;


GLuint ModelMatID;
GLuint ViewMatID;
GLuint ProjectMatID;


struct gl_material
{
    GLuint Ambient;
    GLuint Diffuse;
    GLuint Specular;
    GLuint Shininess;
};

gl_material Material; 


struct gl_light
{
    GLuint Position;
    GLuint Direction;
    GLuint CutOff;
    GLuint OuterCutOff;
    
    GLuint Ambient;
    GLuint Diffuse;
    GLuint Specular;
    
    GLuint Constant;
    GLuint Linear;
    GLuint Quadratic;
    
};

gl_light Light; 

void SetUniformV3(GLuint Program, char* Name, v3 V3)
{
    glUniform3f(glGetUniformLocation(Program, Name), V3.x, V3.y, V3.z);
}


void SetUniformF(GLuint Program, char* Name, real32 Float)
{
    glUniform1f(glGetUniformLocation(Program, Name), Float);
}

void SetUniformM4(GLuint Program, char* Name, m4* M4)
{
    glUniformMatrix4fv(glGetUniformLocation(Program, Name), 1, GL_FALSE, &M4->e[0]);
}

void LoadAssets()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    char *VertexShaderSource = 
        R"Ver(
        #version 330 core 
        layout (location = 0) in vec3 Pos;
        layout (location = 1) in vec3 InNormal;
        
        out vec3 Normal;
        out vec3 FragPos;
        
        uniform mat4 Model; 
        uniform mat4 View; 
        uniform mat4 Projection; 
        
        void main()
        {
        
        FragPos = vec3(Model * vec4(Pos, 1.0));
        Normal = mat3(transpose(inverse(Model))) * InNormal;
        gl_Position = Projection * View * vec4(FragPos, 1.0f);
        }
        )Ver";
    char *FragmentShaderSource = 
        R"Fra(
        #version 330 core 
        
        struct material
        {
        vec3 Ambient;
        vec3 Diffuse;
        vec3 Specular;
        float Shininess;
        }; 
        
        uniform material Material;
        
        struct light 
        {
        vec3 Position;
        vec3 Direction;
        float CutOff;
        float OuterCutOff;
        
        vec3 Ambient;
        vec3 Diffuse;
        vec3 Specular;
        
        float Constant;
        float Linear;
        float Quadratic;
        };
        
        uniform light Light; 
        
        uniform vec3 ViewPos;
        
        in vec3 Normal;
        in vec3 FragPos;
        
        out vec4 FragColor;
        
        void main()
        {
        vec3 Ambient = Light.Ambient * Material.Ambient; 
        
        vec3 Norm = normalize(Normal);
        vec3 LightDir = normalize(-Light.Direction);//normalize(Light.Position - FragPos);
        float Diff = max(dot(Norm, LightDir), 0.0);
        vec3 Diffuse = Light.Diffuse * (Diff * Material.Diffuse);
        
        vec3 ViewDir = normalize(ViewPos - FragPos);
        vec3 ReflectDir = reflect(-LightDir, Norm);  
        float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), Material.Shininess);
        vec3 Specular = Light.Specular * (Spec * Material.Specular);  
        
        vec3 Result = (Ambient + Diff + Specular);
        FragColor = vec4(Result, 1.0); 
        } 
        )Fra";
    
    Program = LoadShaders(VertexShaderSource, FragmentShaderSource);
    
    ModelMatID = glGetUniformLocation(Program, "Model");
    ViewMatID = glGetUniformLocation(Program, "View");
    ProjectMatID = glGetUniformLocation(Program, "Projection");
    
    ViewPosID = glGetUniformLocation(Program, "ViewPos");
    
    Material.Ambient = glGetUniformLocation(Program, "Material.Ambient");
    Material.Diffuse = glGetUniformLocation(Program, "Material.Diffuse");
    Material.Specular = glGetUniformLocation(Program,  "Material.Specular");
    Material.Shininess = glGetUniformLocation(Program, "Material.Shininess");
    
    Light.Ambient = glGetUniformLocation(Program, "Light.Ambient");
    Light.Diffuse = glGetUniformLocation(Program, "Light.Diffuse");
    Light.Specular = glGetUniformLocation(Program,  "Light.Specular");
    Light.Position = glGetUniformLocation(Program, "Light.Position");
    
    Light.Direction = glGetUniformLocation(Program, "Light.Direction");
    Light.CutOff = glGetUniformLocation(Program, "Light.CutOff");
    Light.OuterCutOff = glGetUniformLocation(Program, "Light.OuterCutOff");
    
    Light.Constant = glGetUniformLocation(Program, "Light.Constant");
    Light.Linear = glGetUniformLocation(Program, "Light.Linear");
    Light.Quadratic = glGetUniformLocation(Program, "Light.Quadratic");
    
}

struct matrix_set 
{
    m4 Model;
    m4 Projection;
    m4 View;
};

void SetMatrix(matrix_set *MVP, v3 PosObj, v3 Scale,  v3 Axis, float Angle, v3 CamPos, v3 ViewDir, float FoV, v2 ScreenDim, float dt)
{
    
    gb_mat4_perspective(&MVP->Projection, FoV, ScreenDim.x / ScreenDim.y, 0.1f, 100.0f);
    
    m4 CamRot; 
    gb_mat4_identity(&CamRot);
    
#if 0 
    v3 Target = {0.0f, 0.0f, 0.0f};
    v3 For = Target - CamPos; 
    gb_vec3_norm(&For, For);
    v3 Left;
    gb_vec3_cross(&Left, For, {0.0f, 1.0f, 0.0f});
    gb_vec3_norm(&Left, Left);
    v3 Up;
    gb_vec3_cross(&Up, Left, For);
    CamRot.e[0] = Left.x; 
    CamRot.e[4] = Left.y; 
    CamRot.e[8] = Left.z;
    CamRot.e[1] = Up.x; 
    CamRot.e[5] = Up.y; 
    CamRot.e[9] = Up.z; 
    CamRot.e[2] = For.x; 
    CamRot.e[6] = For.y; 
    CamRot.e[10] = For.z; 
#endif 
    
    m4 CamTran;
    gb_mat4_translate(&CamTran, CamPos);
    //gb_mat4_rotate();
    MVP->View = CamTran * CamRot;
#if 0
    v3 Look = CamPos + v3{0.0f, 0.0f, 1.0f};
    //v3 Look = ViewDir - CamPos;
    gb_mat4_identity(&MVP->View); 
    v3 Right; 
    gb_vec3_cross(&Right, Look, {0.0f, -1.0f, 0.0f});
    v3 Up; 
    gb_vec3_cross(&Up, Look, Right);
    gb_vec3_norm(&Up, Up);
    //gb_mat4_look_at(&MVP->View, CamPos, ViewDir, Up);
    gb_mat4_look_at(&MVP->View, CamPos, Look, Up);
#endif 
    
    gb_mat4_identity(&MVP->Model);
    m4 ModelTrans;
    gb_mat4_translate(&ModelTrans, {PosObj.x, PosObj.y,  PosObj.z});
    m4 ModelRotate; 
    gb_mat4_identity(&ModelRotate);
    
    gb_mat4_rotate(&ModelRotate, Axis, Angle);
    m4 ModelScale;
    gb_mat4_scale(&ModelScale, Scale);
    MVP->Model = ModelTrans * ModelScale * ModelRotate; 
}

void RunRenderBuffer(v2 ScreenDim, float dt, memory_arena *RenderBuffer)
{
    
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(Program);
    
    v3 LightColor = {1.0f, 1.0f, 1.0f};
    v3 DiffuseColor = LightColor * v3{0.5f,0.5f,0.5f};
    v3 AmbientColor = LightColor * v3{0.2f,0.2f,0.2f};
    glUniform3f(Light.Ambient, 0.1f, 0.1f, 0.1f); 
    glUniform3f(Light.Diffuse, 0.8f, 0.8f, 0.8f); 
    glUniform3f(Light.Specular, 1.0f, 1.0f, 1.0f); 
    glUniform3f(Light.Position, -1.2f, 1.0f, -5.0f); 
    glUniform3f(Light.Direction, -0.2f, -1.0f, -0.3f); 
    glUniform1f(Light.CutOff, gb_to_radians(12.5f)); 
    glUniform1f(Light.OuterCutOff, gb_to_radians(17.5f)); 
    
    glUniform1f(Light.Constant, 1.0f); 
    glUniform1f(Light.Linear, 0.09f); 
    glUniform1f(Light.Quadratic, 0.032f); 
    
    glBindVertexArray(cubeVAO);
    
    render_setup *Setup = (render_setup *)RenderBuffer->Memory;
    
    glUniform3f(ViewPosID, Setup->CameraPos.x, Setup->CameraPos.y, Setup->CameraPos.z); 
    
    render_element *FirstElement = (render_element *)(Setup + 1);
    for(uint32 Index = 0;
        Index < Setup->Count;
        ++Index)
    {
        render_element *Element = FirstElement + Index;
        matrix_set MVP;
        SetMatrix(&MVP, Element->Position, Element->Scale, Element->Axis, Element->Angle, Setup->CameraPos, Setup->ViewDir, 45.0f, ScreenDim, dt);
        
        render_material *RenMaterial = &Element->Material;
        glUniform3f(Material.Ambient, RenMaterial->Ambient.x, RenMaterial->Ambient.y,RenMaterial->Ambient.z); 
        glUniform3f(Material.Diffuse, RenMaterial->Diffuse.x, RenMaterial->Diffuse.y,RenMaterial->Diffuse.z); 
        glUniform3f(Material.Specular, RenMaterial->Specular.x, RenMaterial->Specular.y,RenMaterial->Specular.z); 
        glUniform1f(Material.Shininess, RenMaterial->Shininess); 
        
        glUniformMatrix4fv(ViewMatID, 1, GL_FALSE, &MVP.View.e[0]);
        glUniformMatrix4fv(ProjectMatID, 1, GL_FALSE, &MVP.Projection.e[0]);
        glUniformMatrix4fv(ModelMatID, 1, GL_FALSE, &MVP.Model.e[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    Setup->Count = 0;
    ResetArena(RenderBuffer);
}
