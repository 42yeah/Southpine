#version 330 core

in vec3 pos;
in vec3 normal;
in vec2 texCoord;


out vec4 color;

uniform vec3 sunPos;
uniform vec3 sunColor;
uniform vec3 eye;

uniform sampler2D tex;


void main() {
    vec4 texColor = texture(tex, texCoord);
    if (texColor.a == 0) { discard; }
    vec3 original = vec3(texColor);
    vec3 ambient = sunColor * 0.1;
    vec3 sunDir = normalize(sunPos - pos);
    vec3 norm = normalize(normal);
    vec3 diffuse = max(dot(sunDir, norm), 0.0) * sunColor;
    vec3 eyeDir = normalize(eye - pos);
    vec3 refl = normalize(reflect(-sunDir, norm));
    vec3 specular = 0.5 * pow(max(dot(eyeDir, refl), 0.0), 32.0) * sunColor;
    
    color = vec4(2.0 * original * (ambient + diffuse + specular), 1.0);
}
