#version 330 core
layout (location = 0) in vec3 aPos;   // позиция атрибута, содержащего переменные координаты, устанавливается равной 0
layout (location = 1) in vec3 aColor; // позиция атрибута, содержащего переменные цвета, устанавливается равной 1
layout (location = 2) in vec2 aTexCoord;
  
out vec3 ourColor; // на выходе – значение цвета, передаваемое во фрагментный шейдер
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
 
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor; // присваиваем переменной ourColor значение цвета, которое мы получили из данных вершины
    TexCoord = aTexCoord;
}
