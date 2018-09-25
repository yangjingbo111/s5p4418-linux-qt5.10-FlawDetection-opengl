#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//uniform mat4 projectionMatrix;
//uniform mat4 viewMatrix;
//uniform mat4 modelMatrix;


attribute vec3 inPosition;


void main()
{
//    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPosition , 1.0);
    gl_Position = vec4(inPosition , 1.0);


}


///////////////////////////////////////////////

//attribute highp vec4 vertex;
//attribute mediump vec3 normal;
//uniform mediump mat4 matrix;
//varying mediump vec4 color;
//void main(void)
//{
//    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));
//    float angle = max(dot(normal, toLight), 0.0);
//    vec3 col = vec3(0.40, 1.0, 0.0);
//    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);
//    color = clamp(color, 0.0, 1.0);
//    gl_Position = matrix * vertex;
//}
