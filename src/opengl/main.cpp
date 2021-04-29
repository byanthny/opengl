#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

//Create and compile a shader (ie. vertext shader, fragment shader, etc...)
static unsigned int compileShader(const std::string& source, unsigned int type) {
    unsigned int id = glCreateShader(type); //create a "buffer" to hold the shader
    const char* src = source.c_str(); //convert to raw string, source string must exist still
    glShaderSource(id, 1 /* How much source code */, &src, nullptr);
    glCompileShader(id);
    
    //Error Handling
    //Get result or state from glCompileShader and query result for errors
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(!result) //can be written as result == GL_FALSE, has a result
    {
        
        //Get Error Length
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        
        //Get Message
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        
        //Print
        std::cout << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " failed to compile :(" << std::endl;
        std::cout << message << std::endl;
        
        //Clean Up
        glDeleteShader(id);
        return 0;
    }
    
    return id;
    
}

//Provide opengl with shader source code and compile two shaders into one. Then get get an identifer for combined shaders and use it.
static unsigned int createShader (const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int prog = glCreateProgram();
    unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
    
    //Link the two shaders into one "program"
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glValidateProgram(prog);
    
    //Delete "raw" shader since it has been compiled and linked in the program.
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return prog;
}

//Resize viewport to match window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

//Process Input
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true); //close window on escape key
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //For MacOS, allows for use of 3.30

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    //Set Viewport for window
    glViewport(0, 0, 800, 600);
    
    //Tell GLFW to use this callback when the window resizes, updating viewport.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(glewInit() != GLEW_OK)
        std::cout << "Glew Error" << std::endl;
    
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    //Define triangle vertices
    float vertices[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    
    //Create a vertex buffer
    unsigned int buffer, vabuffer;
    glGenVertexArrays(1, &vabuffer);
    glGenBuffers(1, &buffer);
    glBindVertexArray(vabuffer);
    //What kind of buffer?
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //Add data to buffer
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), vertices, GL_STATIC_DRAW);
    //Define what's in the buffer. Howe are the vertices set up: 2d, 3d, with texture data?, etc.....
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
    //Enable Vertex Atrrib
    glEnableVertexAttribArray(0);

    
    //Create Shaders
    std::string vertexShader =
        "#version 330 core\
        layout(location = 0) in vec4 position;\
        void main()\
        {\
            gl_Position = position;\
        }";
    
    std::string fragmentShader =
        "#version 330 core\
        out vec4 color;\
        void main()\
        {\
            color = vec4(1.0, 0.0, 0.0, 1.0);\
        }";
    
    unsigned int shader = createShader(vertexShader, fragmentShader);
    glUseProgram(shader);
    
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Process input
        processInput(window);
        
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw buffer
        glBindVertexArray(vabuffer);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }


    
    glfwTerminate();
    return 0;
}
