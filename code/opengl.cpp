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
GLuint ColorID;
GLuint LightID; 
GLuint LightPosID;
GLuint ViewPosID;

GLuint ModelMatID;
GLuint ViewMatID;
GLuint ProjectMatID;

void LoadAssets()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
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
        
        uniform vec3 ObjColor;
        uniform vec3 LightColor; 
        uniform vec3 LightPos; 
        uniform vec3 ViewPos;
        
        in vec3 Normal;
        in vec3 FragPos;
        
        out vec4 FragColor;
        
        void main()
        {
        float AmbientStrength = 0.1; 
        vec3 Ambient = AmbientStrength * LightColor; 
        
        vec3 Norm = normalize(Normal);
        vec3 LightDir = normalize(LightPos - FragPos);
        float Diff = max(dot(Norm, LightDir), 0.0);
        vec3 Diffuse = Diff * LightColor;
        
        float SpecularStrength = 0.5;
        vec3 ViewDir = normalize(ViewPos - FragPos);
        vec3 ReflectDir = reflect(-LightDir, Norm);  
        float Spec = pow(max(dot(ViewDir, ReflectDir), 0.0), 32);
        vec3 Specular = SpecularStrength * Spec * LightColor;  
        
        vec3 Result = (Ambient + Diff + Specular) * ObjColor;
        FragColor = vec4(Result, 1.0); 
        } 
        )Fra";
    
    Program = LoadShaders(VertexShaderSource, FragmentShaderSource);
    
    ModelMatID = glGetUniformLocation(Program, "Model");
    ViewMatID = glGetUniformLocation(Program, "View");
    ProjectMatID = glGetUniformLocation(Program, "Projection");
    
    ColorID = glGetUniformLocation(Program, "ObjColor");
    LightID = glGetUniformLocation(Program, "LightColor");
    LightPosID = glGetUniformLocation(Program,  "LightPos");
    ViewPosID = glGetUniformLocation(Program, "ViewPos");
}

struct matrix_set 
{
    m4 Model;
    m4 Projection;
    m4 View;
};

void SetMatrix(matrix_set *MVP, v3 PosObj, v3 PosCam, v3 Dim, float FoV, v2 ScreenDim, float Angle)
{
    
    gb_mat4_perspective(&MVP->Projection, FoV, ScreenDim.x / ScreenDim.y, 0.1f, 100.0f);
    
    gb_mat4_identity(&MVP->View); 
    gb_mat4_look_at(&MVP->View, PosCam, PosObj, {0.0f, 1.0f, 0.0f});
    
    gb_mat4_identity(&MVP->Model);
    m4 ModelTrans;
    gb_mat4_translate(&ModelTrans, {PosObj.x, PosObj.y,  PosObj.z});
    m4 ModelRotate; 
    gb_mat4_identity(&ModelRotate);
    v3 Axis = {0.0f, 1.0f, 0.0f};
    
    gb_mat4_rotate(&ModelRotate, Axis, Angle);
    m4 ModelScale;
    gb_mat4_scale(&ModelScale, {Dim.x, Dim.y, Dim.z});
    MVP->Model = ModelTrans * ModelScale * ModelRotate; 
}

void RunRenderBuffer(v2 ScreenDim, float dt)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glUseProgram(Program);
    v3 CamPos = {0.0f, 1.0f, 3.0f};
    v3 ObjPos = {0.0f, 0.0f, 0.0f};
    v3 ObjDim = {1.0f, 1.0f, 1.0f};
    static float Angle;// = 0.5f;
    Angle += dt;
    matrix_set MVP;
    SetMatrix(&MVP, ObjPos, CamPos, ObjDim, 45.0f, ScreenDim, Angle);
    
    glUniformMatrix4fv(ModelMatID, 1, GL_FALSE, &MVP.Model.e[0]);
    glUniformMatrix4fv(ViewMatID, 1, GL_FALSE, &MVP.View.e[0]);
    glUniformMatrix4fv(ProjectMatID, 1, GL_FALSE, &MVP.Projection.e[0]);
    
    glUniform3f(LightID, 1.0f, 1.0f, 1.0f); 
    glUniform3f(ColorID, 1.0f, 0.5f, 0.31f); 
    glUniform3f(LightPosID, 1.2f, 1.0f, 2.0f); 
    glUniform3f(ViewPosID, CamPos.x, CamPos.y, CamPos.z); 
    
    glBindVertexArray(cubeVAO);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
