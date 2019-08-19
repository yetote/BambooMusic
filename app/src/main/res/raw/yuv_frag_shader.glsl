precision mediump float;
uniform sampler2D u_TexY;
uniform sampler2D u_TexU;
uniform sampler2D u_TexV;
varying vec2 v_TextureCoordinates;

void main() {
    mediump float y;
    mediump float u;
    mediump float v;
    lowp  vec3 rgb;
    //    mat3 convmatrix = mat3(vec3(1.164,  1.164, 1.164),
    //                              vec3(0.0,   -0.392, 2.017),
    //                              vec3(1.596, -0.813, 0.0));
    mat3 convmatrix = mat3(vec3(1.164, 1.164, 1.164),
    vec3(0.0, -0.392, 2.017),
    vec3(1.596, -0.813, 0.0));

    y = (texture2D(u_TexY, v_TextureCoordinates).r - (16.0 / 255.0));
    u = (texture2D(u_TexU, v_TextureCoordinates).r - (128.0 / 255.0));
    v = (texture2D(u_TexV, v_TextureCoordinates).r - (128.0 / 255.0));

    rgb = convmatrix * vec3(y, u, v);
    gl_FragColor = vec4(rgb, 1.0);
}