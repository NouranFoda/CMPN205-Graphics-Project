#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;

void main(){

    // Calculate noise
    float mdf = 0.1; // noise amount 
    float noise = (fract(sin(dot(tex_coord, vec2(12.9898,78.233)*2.0)) * 43758.5453));

    frag_color = texture(tex, tex_coord);
    frag_color = frag_color - noise * mdf;
}