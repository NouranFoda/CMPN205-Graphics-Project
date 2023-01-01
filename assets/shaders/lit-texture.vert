#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

uniform mat4 object_to_world;
uniform mat4 object_to_wolrd_inv_transpose;
uniform mat4 view_projection;
uniform vec3 camera_position;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} vs_out;

void main(){
    vs_out.world = (object_to_world * vec4(position, 1.0)).xyz;
    vs_out.view = camera_position - vs_out.world;
    vs_out.normal = normalize((object_to_wolrd_inv_transpose * vec4(normal, 0.0)).xyz);
    gl_Position = view_projection * vec4(vs_out.world, 1.0);
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
}
