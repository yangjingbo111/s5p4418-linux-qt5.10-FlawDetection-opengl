#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform float r;
uniform float g;
uniform float b;


void main()
{
    gl_FragColor = vec4(r,g,b,1.0);
}


//////////////////////////////////////////

//varying mediump vec4 color;
//void main(void)
//{
//    gl_FragColor = color;
//}
