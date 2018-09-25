

/****************************************************************************
**
** Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company.
** Author: Giuseppe D'Angelo
** Contact: info@kdab.com
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 position;
out vec3 normal;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;

void main()
{
    normal = normalize( normalMatrix * vertexNormal );
    position = vec3( modelViewMatrix * vec4( vertexPosition, 1.0 ) );

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

smooth out vec3 theColor;

gl_Position = projectionMatrix * modelViewMatrix * vec4(inPosition , 1.0);

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;

out vec2 texCoord;


void main()
{
    gl_PointSize = 16;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(inPosition , 1.0);

    texCoord = inCoord;
}

****************************************************************************/
//uniform mat4 projectionMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 modelMatrix;


//layout (location = 0) in vec3 inPosition;




//void main()
//{
//    gl_PointSize = 2;
//    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPosition , 1.0);


//}

attribute highp vec4 vertex;
attribute mediump vec3 normal;
uniform mediump mat4 matrix;
varying mediump vec4 color;
void main(void)
{
    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));
    float angle = max(dot(normal, toLight), 0.0);
    vec3 col = vec3(0.40, 1.0, 0.0);
    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);
    color = clamp(color, 0.0, 1.0);
    gl_Position = matrix * vertex;
}
