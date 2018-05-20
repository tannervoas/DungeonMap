#version 150 core

in vec3 position;

const vec3 inColor = vec3(0.831f,0.686f,0.215f);
const vec3 inpointlightPos = normalize(vec3(0,-30,0));
const float inpointlightMag = 10;
const vec3 inLightDir = normalize(vec3(0,1,0));
const vec3 inLightDir2 = normalize(vec3(-1,-1,1));
in vec3 inNormal;
in vec2 inTexcoord;

out vec3 pointlightPos; //Point light position. Above player always
out float pointlightMag; //Intensity of point light
out vec3 lightDir;
out vec3 lightDir2;
out vec3 Color;
out vec3 normal;
out vec3 pos;
out vec2 texcoord;
out vec3 eyePosOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 no_tran_view;
uniform mat4 proj;
uniform vec3 eyePos;

uniform int texID;

void main() {
   pointlightMag = inpointlightMag;
   Color = inColor;
   if (texID == 0) {
    gl_Position = proj * no_tran_view * model * vec4(position,1.0);
    pos = position;
   } else {
    gl_Position = proj * view * model * vec4(position,1.0);
    pos = (view * model * vec4(position,1.0)).xyz;
   }
   lightDir = (view * vec4(inLightDir,0)).xyz;  //(Light is a vector) Transform light into to view spaceaaa
   lightDir2 = (view * vec4(inLightDir2,0)).xyz;  //(Light is a vector) Transform light into to view spaceaaa
   eyePosOut = (view * vec4(eyePos, 0.0)).xyz;
   pointlightPos = (view * vec4(eyePos + vec3(0,5,0), 0.0)).xyz;
   vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal,0.0);
   normal = normalize(norm4.xyz);
   texcoord = inTexcoord;
}
