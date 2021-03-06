#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
using namespace std;

SDL_Window *win;
GLuint program;

float pseudoscalar(const vec2 a, const vec2 b) { return a.x*b.y - b.x*a.y; }

vec3 hline(vec2 a, vec2 b) {
    const vec2 c = vec2(0., 0.);

    if (pseudoscalar(b - a, c - a) > 0) swap(a, b);

    float ah = length(b - a) / 2.;
    float cz = abs(dot(((a + b) / vec2(2.) - c), a - b)) / length(a - b);
    float zh = abs(dot(a - c, vec2(-(b - a).y, (b - a).x))) / length(b - a);

    float oh = (1. + ah*ah - cz*cz - zh*zh) / (2. * zh);

    vec2 o = (a + b) / vec2(2.) + normalize(vec2(-(b - a).y, (b - a).x)) * oh;

    return vec3(o, length(o - a));
}

vec3 line(const vec2 a, const vec2 b) { return vec3(a.y - b.y, b.x - a.x, a.x*b.y - b.x*a.y); }

void mainpart() {
    // Get uniform locations
    GLuint uTime = glGetUniformLocation(program, "time");
    GLuint uResolution = glGetUniformLocation(program, "resolution");

    GLuint uPoints = glGetUniformLocation(program, "points");
    GLuint uPointsSize = glGetUniformLocation(program, "pointsSize");

    GLuint uCircles = glGetUniformLocation(program, "circles");
    GLuint uCirclesSize = glGetUniformLocation(program, "circlesSize");

    GLuint uLines = glGetUniformLocation(program, "lines");
    GLuint uLinesSize = glGetUniformLocation(program, "linesSize");

    // Mainloop
    SDL_Event e;
    for(;;) {
        bool quit = 0;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        if (quit) break;

        // Set the time uniform
        float time = SDL_GetTicks();
        glUniform1f(uTime, time);

        // Set the resolution uniform
        int W, H;
        SDL_GetWindowSize(win, &W, &H);
        glViewport(0, 0, W, H);
        glUniform2i(uResolution, W, H);

        // Set the cicles uniform
        vec2 points[] = {
            vec2(0.4, -0.1), vec2(0.3, -0.6),
            vec2(0.2, -0.3), vec2(0.3,  0.5),
            vec2(0.1, -0.1), vec2(0.6, -0.6),
        };
        int pointsSize = sizeof(points) / sizeof(points[0]);

        int circlesSize = 0;
        int linesSize = 0;
        vec3 circles[100];
        vec3 lines[100];

        for (int i = 0; i + 1 < pointsSize; i += 2) {
            if (pseudoscalar(points[i], points[i + 1]) == 0)
                lines[linesSize++] = line(points[i], points[i + 1]);
            else
                circles[circlesSize++] = hline(points[i], points[i + 1]);
        }

        glUniform2fv(uPoints, pointsSize, value_ptr(points[0]));
        glUniform1i(uPointsSize, pointsSize);

        glUniform3fv(uCircles, circlesSize, value_ptr(circles[0]));
        glUniform1i(uCirclesSize, circlesSize);

        glUniform3fv(uLines, linesSize, value_ptr(lines[0]));
        glUniform1i(uLinesSize, linesSize);


        glClear(GL_COLOR_BUFFER_BIT);
        glRectf(-1., -1., 1., 1.);

        SDL_GL_SwapWindow(win);
    }
}

int main(int argc, char **argv) {
    // SDL2 init
    SDL_Init(SDL_INIT_VIDEO);

    win = SDL_CreateWindow("",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            800, 600,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_GLContext cont = SDL_GL_CreateContext(win);
    
    // Load shader code from file
    FILE *fragment_shader_fp = fopen("shader.frag", "r");
    fseek(fragment_shader_fp, 0L, SEEK_END);
    long int fragment_shader_fs = ftell(fragment_shader_fp) + 1;
    rewind(fragment_shader_fp);

    char *fragment_shader = (char*) malloc(fragment_shader_fs * sizeof(char));
    for (int i = 0; i < fragment_shader_fs; ++i) {
        fragment_shader[i] = fgetc(fragment_shader_fp);
    }
    fragment_shader[fragment_shader_fs - 1] = 0;

    fclose(fragment_shader_fp);

    // Shader
    GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, (const GLchar**)&fragment_shader, NULL);
    glCompileShader(shader);

    // Check shader for errors
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

        char *errlog = (char*) malloc(len * sizeof(char));
        glGetShaderInfoLog(shader, len, &len, errlog);

        cerr << errlog << '\n';

        glDeleteShader(shader);
        SDL_GL_DeleteContext(cont);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return -1;
    }

    // Shader program
    program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glUseProgram(program);

    glDisable(GL_DEPTH_TEST);

    // Main part
    mainpart();

    // Quit
    SDL_GL_DeleteContext(cont);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
