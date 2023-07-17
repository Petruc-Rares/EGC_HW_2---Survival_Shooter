#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_color;
out vec3 normal_color;

#define PI 3.1415926538

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    // TODO(student): Send output to fragment shader
    frag_color = v_color;
    normal_color = v_normal;

    float noise = 0 + sin (2* PI * 1 / 12 * Time + PI/2);
    vec3 newPosition = v_position;
    //newPosition.y *= noise;
    //newPosition.z *= noise;
    //newPosition.x *= noise;
    
    //v_normal += frag_color;

    /*
    if (v_position.x > 0) {
        newPosition.x += noise;
    } else {
        newPosition.x -= noise;
    }
    
    if (v_position.z > 0) {
        newPosition.z += noise;
    } else {
        newPosition.z -= noise;
    }

    if (v_position.y > 0) {
        newPosition.y -= noise;
    }*/

    
    float x = rand(vec2(v_position.x, v_position.z)) + noise * rand(vec2(v_position.z, v_position.y));
    newPosition *= x;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(newPosition, 1);
}
