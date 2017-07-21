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

void SetMatrix(matrix_set *MVP, v3 PosObj, v3 PosCam, v3 Dim, float FoV, v2 ScreenDim, v3 Axis, float Angle)
{
    
    gb_mat4_perspective(&MVP->Projection, FoV, ScreenDim.x / ScreenDim.y, 0.1f, 100.0f);
    
    gb_mat4_identity(&MVP->View); 
    gb_mat4_look_at(&MVP->View, PosCam, PosObj, {0.0f, 1.0f, 0.0f});
    
    gb_mat4_identity(&MVP->Model);
    m4 ModelTrans;
    gb_mat4_translate(&ModelTrans, {PosObj.x, PosObj.y,  PosObj.z});
    m4 ModelRotate; 
    gb_mat4_identity(&ModelRotate);
    
    gb_mat4_rotate(&ModelRotate, Axis, Angle);
    m4 ModelScale;
    gb_mat4_scale(&ModelScale, {Dim.x, Dim.y, Dim.z});
    MVP->Model = ModelTrans * ModelScale * ModelRotate; 
}

void RunRenderBuffer(v2 ScreenDim, float dt)
{
    v3 CubePositions[] = {
        v3{ 0.0f,  0.0f,  0.0f},
        v3{2.0f,  5.0f, -15.0f},
        v3{-1.5f, -2.2f, -2.5f},
        v3{-3.8f, -2.0f, -12.3f},
        v3{2.4f, -0.4f, -3.5f},
        v3{-1.7f,  3.0f, -7.5f},
        v3{1.3f, -2.0f, -2.5f},
        v3{1.5f,  2.0f, -2.5f},
        v3{1.5f,  0.2f, -1.5f},
        v3{-1.3f,  1.0f, -1.5f}
    };
    
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
    glUniform3f(Light.Position, 1.2f, 1.0f, 2.0f); 
    glUniform3f(Light.Direction, -0.2f, -1.0f, -0.3f); 
    glUniform1f(Light.CutOff, gb_to_radians(12.5f)); 
    glUniform1f(Light.OuterCutOff, gb_to_radians(17.5f)); 
    
    glUniform1f(Light.Constant, 1.0f); 
    glUniform1f(Light.Linear, 0.09f); 
    glUniform1f(Light.Quadratic, 0.032f); 
    
    v3 CamPos = {0.0f, 0.0f, 3.0f};
    glUniform3f(ViewPosID, CamPos.x, CamPos.y, CamPos.z); 
    
    glUniform3f(Material.Ambient, 1.0f, 0.5f, 0.31f); 
    glUniform3f(Material.Diffuse, 1.0f, 0.5f, 0.31f); 
    glUniform3f(Material.Specular, 0.5f, 0.5f, 0.5f); 
    glUniform1f(Material.Shininess, 32.0f); 
    
    glBindVertexArray(cubeVAO);
    for(uint32 Index = 0;
        Index < 10; 
        ++Index)
    {
        v3 ObjDim = {1.0f, 1.0f, 1.0f};
        v3 ObjPos = {CubePositions[Index].x, CubePositions[Index].y, CubePositions[Index].z};
        float Angle = 20.0f * Index;
        
        matrix_set MVP;
        SetMatrix(&MVP, ObjPos, CamPos, ObjDim, 45.0f, ScreenDim, {1.0f, 0.3f, 0.5f}, Angle);
        
        
        glUniformMatrix4fv(ViewMatID, 1, GL_FALSE, &MVP.View.e[0]);
        glUniformMatrix4fv(ProjectMatID, 1, GL_FALSE, &MVP.Projection.e[0]);
        glUniformMatrix4fv(ModelMatID, 1, GL_FALSE, &MVP.Model.e[0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    
    
}
