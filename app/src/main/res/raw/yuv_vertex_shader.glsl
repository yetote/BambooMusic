attribute vec4 a_Position;
attribute vec2 a_TextureCoordinates;
varying  vec2  v_TextureCoordinates;
attribute vec4 a_Color;
varying vec4 v_Color;
void main() {
    gl_Position =a_Position;
    v_TextureCoordinates=a_TextureCoordinates;
//    v_Color=a_Color;
}
