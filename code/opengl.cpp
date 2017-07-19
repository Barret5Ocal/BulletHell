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

void LoadAssets()
{
    float cubeVertices[] = {
        // positions         
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f, 
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        
        -0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, 
        
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 
        
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 
        
        -0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f,  
    };
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    char *VertexShaderSource = 
        R"Ver(
        #version 330 core 
        layout (location = 0) in vec3 Pos;
        
        uniform mat4 MVP;
        void main()
        {
        
        gl_Position = MVP * vec4(Pos, 1.0f); 
        }
        )Ver";
    char *FragmentShaderSource = 
        R"Fra(
        #version 330 core 
        
        uniform vec4 InColor;
        
        out vec4 FragColor;
        
        void main()
        {
        FragColor = InColor; 
        }
        )Fra";
    
    Program = LoadShaders(VertexShaderSource, FragmentShaderSource);
    
    MatrixID = glGetUniformLocation(Program, "MVP");
    ColorID = glGetUniformLocation(Program, "InColor");
}


void SetMatrix(m4 *MVP, v3 PosObj, v3 PosCam, v3 Dim, float FoV, v2 ScreenDim, float Angle)
{
    m4 Projection;
    gb_mat4_perspective(&Projection, FoV, ScreenDim.x / ScreenDim.y, 0.1f, 100.0f);
    
    m4 View;
    gb_mat4_identity(&View); 
    gb_mat4_look_at(&View, PosCam, PosObj, {0.0f, 1.0f, 0.0f});
    
    m4 Model;
    gb_mat4_identity(&Model);
    m4 ModelTrans;
    gb_mat4_translate(&ModelTrans, {PosObj.x, PosObj.y,  PosObj.z});
    m4 ModelRotate; 
    gb_mat4_identity(&Model);
    v3 Axis = {1.0f, 0.0f, 0.0f};
    
    gb_mat4_rotate(&ModelRotate, Axis, Angle);
    m4 ModelScale;
    gb_mat4_scale(&ModelScale, {Dim.x, Dim.y, Dim.z});
    Model = ModelTrans * ModelScale * ModelRotate; 
    
    *MVP = Projection * View * Model;
}

void RunRenderBuffer(v2 ScreenDim, float dt)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    v3 CamPos = {0.0f, 0.0f, -5.0f};
    v3 ObjPos = {0.0f, 0.0f, 0.0f};
    v3 ObjDim = {1.0f, 1.0f, 1.0f};
    static float Angle;
    Angle += dt;
    m4 MVP;
    SetMatrix(&MVP, ObjPos, CamPos, ObjDim, 45.0f, ScreenDim, Angle);
    
    
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP.e[0]);
    
    glUniform4f(ColorID, 1.0f, 1.0f, 1.0f, 1.0f); 
    
    glUseProgram(Program);
    glBindVertexArray(cubeVAO);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
