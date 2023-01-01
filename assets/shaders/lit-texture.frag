 #version 330

#define MAX_LIGHTS 16

#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation; 
    vec2 cone_angles; 
};

uniform Light lights[MAX_LIGHTS];
uniform int light_count;



struct Material {
    sampler2D albedo_tex;
    sampler2D specular_tex;
    sampler2D ao_tex;
    sampler2D roughness_tex;
    sampler2D emission_tex;
};

uniform Material material;

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view;
    vec3 world;
} fs_in;

struct SkyLight {
    vec3 sky, horizon, ground;
};
vec3 compute_sky_light(vec3 normal, SkyLight sky_light) {
    float y = normal.y;
    float sky_factor = max(0, y);
    float ground_factor = max(0, -y);
    sky_factor *= sky_factor;
    ground_factor *= ground_factor;
    float horizon_factor = 1 - sky_factor - ground_factor;
    return sky_light.sky * sky_factor + sky_light.horizon * horizon_factor + sky_light.ground * ground_factor;
}
out vec4 frag_color;

void main(){
    vec3 view = normalize(fs_in.view);
    vec3 normal = normalize(fs_in.normal);
    int count = min(MAX_LIGHTS, light_count);

    vec3 accumulated_light = vec3(0.0, 0.0, 0.0);

    vec3 material_diffuse = texture(material.albedo_tex, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular_tex, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ao_tex, fs_in.tex_coord).r;
    float material_roughness = texture(material.roughness_tex, fs_in.tex_coord).r;
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;
    vec3 material_emissive = texture(material.emission_tex, fs_in.tex_coord).rgb;


    for(int i = 0; i < count; i++){

        Light light = lights[i];
        vec3 light_direction;

        float attenuation = 1;

        if(light.type == DIRECTIONAL)
            light_direction = light.direction;

        else {

            light_direction = fs_in.world - light.position;
            float distance = length(light_direction);
            light_direction /= distance;
            attenuation *= 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);

            if(light.type == SPOT){
                float angle = acos(dot(light.direction, light_direction));
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
            }

        }

        vec3 reflected = reflect(light_direction, normal);
        float lambert = max(0.0f, dot(normal, -light_direction));
        float phong = pow(max(0.0f, dot(view, reflected)), material_shininess);
        vec3 diffuse = material_diffuse * light.diffuse * lambert;
        vec3 specular = material_specular * light.specular * phong;
        vec3 ambient = material_ambient * light.diffuse;
        accumulated_light += (diffuse + specular) * attenuation + ambient;

    }
    
    frag_color = fs_in.color * vec4(accumulated_light, 1.0f);

}