precision mediump float;
uniform sampler2D u_TextureY;
uniform sampler2D u_TextureU;
uniform sampler2D u_TextureV;
varying vec2 v_TextureCoordinates;
void main() {
    mediump float y;
    mediump float u;
    mediump float v;
    lowp vec3 rgb;
    mat3 convmatrix = mat3(vec3(1.164, 1.164, 1.164),
    vec3(0.0, -0.392, 2.017),
    vec3(1.596, -0.813, 0.0));

    y = (texture2D(u_TextureY, v_TextureCoordinates).r - (16.0 / 255.0));
    u = (texture2D(u_TextureU, v_TextureCoordinates).r - (128.0 / 255.0));
    v = (texture2D(u_TextureV, v_TextureCoordinates).r - (128.0 / 255.0));
    rgb = convmatrix * vec3(y, u, v);
    gl_FragColor = vec4(rgb, 1.0);
    //    gl_FragColor=v_Color;
}