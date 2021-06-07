#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

struct Shaders
{
    std::string vertex;
    std::string fragment;
};

//Parse combined shader file into fragment and vertex shader strings.
static Shaders parseShader(const std::string& filepath) {
    std::ifstream stream(filepath); //TODO Replace fstream reading can be slow
    
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
        
    while (getline(stream, line)) {
        if(line.find("shader") != std::string::npos) {
            if (line.find("#vertex") != std::string::npos) { //vertex shader found
                type = ShaderType::VERTEX;
            } else if  (line.find("#fragment") != std::string::npos) { //fragment shader found
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }
    
    return {ss[0].str(), ss[1].str()};
    
}

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
    float vertices[] = {
        -0.5f, -0.5f, //0
         0.5f, -0.5f, //1
         0.5f,  0.5f, //2
        -0.5f,  0.5f, //3
    };
    
    //used for index buffer, using vertices above
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    //Create a vertex buffer
    unsigned int buffer, vabuffer;
    glGenVertexArrays(1, &vabuffer); //Fixed black screen, what does it actually do?
    glGenBuffers(1, &buffer);
    glBindVertexArray(vabuffer); //Fixed black screen, what does it actually do?
    //What kind of buffer?
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //Add data to buffer
    glBufferData(GL_ARRAY_BUFFER, 2*6*sizeof(float), vertices, GL_STATIC_DRAW);
    //Define what's in the buffer. Howe are the vertices set up: 2d, 3d, with texture data?, etc.....
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
    //Enable Vertex Atrrib
    glEnableVertexAttribArray(0);
    
    unsigned int indexBuffer; //can be unsigned char, short, etc, char will only have 255 indices.
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indices, GL_STATIC_DRAW);

    Shaders source = parseShader("res/shaders/basic.shader");
    
    //std::cout << source.vertex << std::endl;
    //std::cout << source.fragment << std::endl;
    
    unsigned int shader = createShader(source.vertex, source.fragment);
    glUseProgram(shader);
    
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Process input
        processInput(window);
        
        /* Render here */
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    //glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
