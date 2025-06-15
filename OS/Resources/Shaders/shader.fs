#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D image;

void main() {
    FragColor = vec4(1.0, 0.0, 1.0, 1.0) * texture(image, TexCoords); // Green triangle
}