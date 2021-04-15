#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <GL/freeglut.h>

#include <unistd.h>
#include <string>

#include <sys/ioctl.h>

#include "shader_s.h"
#include "camera.h"
//#include "particle.h"

#include <iostream>
#include <cstdlib>
struct particle{
glm::vec3 position;
glm::vec3 velocity;
bool vntr;
};

void charr(unsigned long long *data, int n, std::string x_name, std::string y_name);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
particle Move(float dt, particle part, float a, float b, float d);
void Stolknovenie(particle part, particle* all, int Size);
float Dlina (glm::vec3 v1, glm::vec3 v2);


const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// Камера
Camera camera(glm::vec3(80.0f, 80.0f, 70.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;
const int N = 10000;
int vnutri = N;
float analit = 1.0f/4.0f * N/(40.0f*40.0f*40.0f)*10.0f;

float t = glfwGetTime();

float deltaTime = 0.0f;	// время между текущим кадром и последним кадром
float lastFrame = 0.0f;
float Radius = 0.1;
unsigned long long data[10000] = {0};
//unsigned long long dataln[10000] = {0};
int n = 1;
float dt = 1.0f;

int main()
{
    // glfw: инициализация и конфигурирование
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sphere", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Сообщаем GLFW, чтобы он захватил наш курсор
	 glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Компилирование нашей шейдерной программы
    Shader ourShader("/home/kfash/OpenGL_Project/CodeBlocks/partical.vs", "/home/kfash/OpenGL_Project/CodeBlocks/partical.fs");

    float vertices[] = {
         0.00000f,  0.00000f,  1.00000f, //1
        -0.52573f,  0.72361f,  0.44721f, //2
        -0.85065f, -0.27639f,  0.44721f, //3
         0.00000f, -0.89443f,  0.44721f, //4
         0.85065f, -0.27639f,  0.44721f, //5
         0.52573f,  0.72361f,  0.44721f, //6
         0.52573f, -0.72361f, -0.44721f, //7
         0.85065f,  0.27639f, -0.44721f, //8
         0.00000f,  0.89443f, -0.44721f, //9
        -0.85065f,  0.27639f, -0.44721f, //10
        -0.52573f, -0.72361f, -0.44721f, //11
         0.00000f,  0.00000f, -1.00000f, //12
         };

    unsigned int indices[] = {
        0,  1,  2,
        0,  2,  3,
        0,  3,  4,
        0,  4,  5,
        0,  5,  1,
        1,  8,  9,
        2,  9,  10,
        3,  10, 6,
        4,  6,  7,
        5,  7,  8,
        11, 10, 9,
        11, 9,  8,
        11, 8,  7,
        11, 7,  6,
        11, 6,  10,
        1,  2,  9,
        2,  3,  10,
        3,  4,  6,
        4,  5,  7,
        5,  1,  8,
        };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Сначала связываем объект вершинного массива, затем связываем и устанавливаем вершинный буфер(ы), и затем конфигурируем вершинный атрибут(ы)
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);




    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    unsigned int Size = vnutri;

    particle* all_part = new particle [Size];
    glm::vec3* buf = new glm::vec3 [Size];

    for(unsigned int i = 0; i < Size; i++)
        {
        all_part[i].position = glm::vec3(((float)(rand()%40000) + 20000.0f)/1000, ((float)(rand()%40000) + 20000.0f)/1000, ((float)(rand()%40000) + 20000.0f)/1000);
        all_part[i].velocity = glm::vec3(((float)(rand()%40000) - 20000.0f)/1000, ((float)(rand()%40000) - 20000.0f)/1000, ((float)(rand()%40000) - 20000.0f)/1000);
        all_part[i].vntr = 1;
        //all_part[i].vnutr = 1;
        //std::cout << " x = " << std::endl;
        }

    // Цикл рендеринга
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    while (!glfwWindowShouldClose(window))
    {
       // Логическая часть работы со временем для каждого кадра
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // Обработка ввода
        processInput(window);

        // Рендеринг

        // Очищаем цветовой буфер
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //очищвем буффер глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setMat4("projection", projection);
        // Примечание: В настоящее время мы устанавливаем матрицу проекции для каждого кадра, но поскольку матрица проекции редко меняется, то рекомендуется устанавливать её (единожды) вне основного цикла
        glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);
		vnutri = Size;

        glBindVertexArray(VAO); // поскольку у нас есть только один VАО, то нет необходимости связывать его каждый раз (но мы сделаем это, чтобы всё было немного организованнее)
        for(unsigned int i = 0; i < Size; i++)
        {
            glm::mat4 model_scale = glm::mat4(1.0f);
            glm::mat4 model_trans = glm::mat4(1.0f);
            glm::mat4 model = glm::mat4(1.0f);
            all_part[i]= Move(deltaTime, all_part[i], 20.0f, 60.0f, 5.0f);
            //Stolknovenie(all_part[i], all_part, Size);

            model_scale = glm::scale(model, glm::vec3(Radius, Radius, Radius));

            model_trans = glm::translate(model, all_part[i].position);

            model =  model_trans * model_scale;
            //glm::vec4 buf = glm::vec4(1.0f);
            //buf = model * buf;
            /*if (all_part[i].vntr == 0)
            {
                vnutri--;
            }
            if(buf.z > 70.0f)
            {
                all_part[i].vntr = 0;
            }*/


            //float angle = 20.0f * i  + (float)glfwGetTime() * 50.0f;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f * i, 0.5f));
            ourShader.setMat4("model", model);
            ourShader.setVec3("speed", all_part[i].velocity);
            //printf("%d \n", vnutri);

            glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
        }
        float t1 = glfwGetTime();
        if ((t1 - t)/dt > n)
        {
            data[n] = N - vnutri;
            //dataln[n] = log(data[n]);
            n++;
        }
        // glBindVertexArray(0); // не нужно каждый раз его отвязывать

        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода\вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Опционально: освобождаем все ресурсы, как только они выполнили свое предназначение
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: завершение, освобождение всех ранее задействованных GLFW-ресурсов
    glfwTerminate();
    return 0;
}

// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании кнопки мыши в данном кадре и соответствующая обработка данных событий
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        float t1 = glfwGetTime();
        for(int i = 0; i < n; i++)
        {
            printf("%d :: %d \n", i, data[i]);
        }
        printf("Model :: %d \nAnalitic :: %f \n n = %d \n", N - vnutri, (t1 - t)*analit/deltaTime, n);
        charr(data, n, "t", "N");
        //charr(dataln, n, "t", "ln(n)");
    }


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Убеждаемся, что окно просмотра соответствует новым размерам окна.
    // Обратите внимание, что высота будет значительно больше, чем указано, на Retina-дисплеях
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // перевернуто, так как y-координаты идут снизу вверх

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

particle Move(float dt, particle part, float a, float b, float d)
{
    part.position += part.velocity * dt;
    //Dlina(part.velocity, dlm::vec3)

        // Затем п0000роверяем, находится ли он за пределами границ окна, и если да, то изменяем его скорость и восстанавливаем правильное положение
    if(part.position.z - 4.0f<= b)
    {
        if (part.position.x - Radius <= a)
        {
            part.velocity.x = -part.velocity.x;
            part.position.x = a + Radius;
        }
        else if (part.position.x + Radius >= b)
        {
            part.velocity.x = -part.velocity.x;
            part.position.x = b - Radius;
        }
        if (part.position.y - Radius <= a)
        {
            part.velocity.y = -part.velocity.y;
            part.position.y = a + Radius;
        }
        else if (part.position.y + Radius >= b)
        {
            part.velocity.y = -part.velocity.y;
            part.position.y = b - Radius;
        }
        if (part.position.z - Radius <= a)
        {
            part.velocity.z = -part.velocity.z;
            part.position.z = a + Radius;
        }
        else if (part.position.z + Radius >= b)
        {
            if (!(part.position.x >= (a + b - d)/2 && part.position.x <= (a + b + d)/2 && part.position.y >= (a + b - d)/2 && part.position.y <= (a + b + d)/2))
            {
                part.velocity.z = -part.velocity.z;
                part.position.z = b - Radius;
            }

        }
    }

    return part;
}

void Stolknovenie(particle part, particle* all, int Size)
{
    for(int i = 0; i < Size; i++)
    {
        float d = Dlina(part.position, all[i].position);
        if (d <= 3* Radius)
        {
            if (!(d >= -0.01f && d <= 0.01f))
            {
                //std::cout << "DO ::iii vx = " << part.velocity.x << " vy = " << part.velocity.y << " vz = " << part.velocity.z << std::endl;
                //std::cout << "DO ::jjj vx = " << all[i].velocity.x << " vy = " << all[i].velocity.y << " vz = " << all[i].velocity.z << std::endl;
                //printf("d_n = %f \n", d);
                glm::vec3 ndeltar = (part.position - all[i].position)/d;
                //std::cout << "nx = " << ndeltar.x << " ny = " << ndeltar.y << " nz = " << ndeltar.z << std::endl;
                glm::vec3 u = (part.velocity - all[i].velocity)/2.0f;
                //std::cout << "ux = " << u.x << " uy = " << u.y << " uz = " << u.z << std::endl;
                glm::vec3 u_par = ndeltar * (u.x*ndeltar.x + u.y*ndeltar.y + u.z*ndeltar.z);
                //std::cout << "u_parx = " << u_par.x << " u_pary = " << u_par.y << " u_parz = " << u_par.z << std::endl;
                glm::vec3 u_sht = - u + 2.0f*u_par;
                //std::cout << "u_shtx = " << u_sht.x << " u_shty = " << u_sht.y << " u_shtz = " << u_sht.z << std::endl;

                part.velocity = (part.velocity + all[i].velocity)/2.0f + u_sht;
                all[i].velocity = (part.velocity + all[i].velocity)/2.0f - u_sht;
                //std::cout << "POSLE ::iii vx = " << part.velocity.x << " vy = " << part.velocity.y << " vz = " << part.velocity.z << std::endl;
                //std::cout << "POSLE ::jjj vx = " << all[i].velocity.x << " vy = " << all[i].velocity.y << " vz = " << all[i].velocity.z << std::endl;
                //std::cout << std::endl;
                //sleep(1);
            }
        }
    }
}

float Dlina (glm::vec3 v1, glm::vec3 v2)
    {
    return sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
    }

