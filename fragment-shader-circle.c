/*
 * $ cc -Wall -pedantic -Wextra fragment-shader-circle.c -o fragment-shader-circle -lGL -lSDL2
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>


const GLchar* vertexSource =
"attribute vec4 position;                                   \n"
"attribute vec4 color;                                      \n"
"varying vec4 v_color;                                      \n"
"void main()                                                \n"
"{                                                          \n"
"    v_color = color;                                       \n"
"    gl_Position = vec4(position.xyz, 1.0);                 \n"
"}                                                          \n";

const GLchar *fragmentSource =	
"varying vec4 v_color;                                      \n"
"uniform vec2 u_resolution;                                 \n"
"                                                           \n"
"float draw_circle(vec2 coord, float radius) {              \n"
"    return step(length(coord), radius);                    \n"
"}                                                          \n"
"                                                           \n"
"void main() {                                              \n"
"    vec2 coord = gl_FragCoord.xy / u_resolution;           \n"
"    vec2 offset = vec2(0.5, 0.5);                          \n"
"    float circle = draw_circle(coord - offset, 0.2);       \n"
"    vec4 xcolor = circle * v_color;                        \n"
"                                                           \n"
"    gl_FragColor = vec4(xcolor);                           \n"
"}                                                          \n";

GLuint
LoadShader(const char *shaderSrc, GLenum type)
{
	GLuint shader;
	GLint compiled;
	/* Create the shader object */
	shader = glCreateShader(type);
	if(shader == 0) {
		return 0;
	}
	/* Load the shader source */
	glShaderSource(shader, 1, &shaderSrc, NULL);
	/* Compile the shader */
	glCompileShader(shader);
	/* Check the compile status */
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if(infoLen > 1){
			char *infoLog = malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling shader:\n%s\n",
				infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

int
main()
{
	SDL_Window *wnd;
	SDL_GLContext glc;
	SDL_Renderer *rdr;
	GLint attr_pos = 0, attr_color = 1;
	GLint uloc;

	wnd = SDL_CreateWindow("OpenGL ES test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		600, 600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetSwapInterval(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	glc = SDL_GL_CreateContext(wnd);

	rdr = SDL_CreateRenderer(wnd, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	GLuint vertexShader = LoadShader(vertexSource, GL_VERTEX_SHADER);
	if (!vertexShader) {
		return EXIT_FAILURE;
	}

	GLuint fragmentShader = LoadShader(fragmentSource, GL_FRAGMENT_SHADER);
	if (!fragmentShader) {
		return EXIT_FAILURE;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	glBindAttribLocation(shaderProgram, attr_pos, "position");
	glBindAttribLocation(shaderProgram, attr_color, "color");
	glLinkProgram(shaderProgram);

	uloc = glGetUniformLocation(shaderProgram, "u_resolution");
	if (uloc < 0) {
		fprintf(stderr, "glGetUniformLocation() failed\n");
		return EXIT_FAILURE;
	}

	glUniform2f(uloc, 600.0f, 600.0f);

	while (1) {
		static const GLfloat verts[3*2][2] = {
			{ -1.0, -1.0 },
			{  1.0, -1.0 },
			{ -1.0,  1.0 },

			{ -1.0,  1.0 },
			{  1.0, -1.0 },
			{  1.0,  1.0 }
		};
		static const GLfloat colors[3*2][3] = {
			{ 1.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 1.0 },

			{ 1.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0 },
			{ 0.0, 0.0, 1.0 }
		};

		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				exit(EXIT_SUCCESS);
			}
		}

		glClearColor(0.0f, 0.5f, 0.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);

		glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(attr_color, 3, GL_FLOAT, GL_FALSE, 0, colors);
		glEnableVertexAttribArray(attr_pos);
		glEnableVertexAttribArray(attr_color);

		glDrawArrays(GL_TRIANGLES, 0, 3*2);

		glDisableVertexAttribArray(attr_pos);
		glDisableVertexAttribArray(attr_color);

		SDL_GL_SwapWindow(wnd);
	}

	return EXIT_SUCCESS;
}

