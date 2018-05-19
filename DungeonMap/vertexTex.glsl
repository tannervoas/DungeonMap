#version 150 core
in vec3 position;
//"in vec3 inColor;
const vec3 inColor = vec3(0.f,0.7f,0.f);
const vec3 inlightDir = normalize(vec3(1,0,0));  //we should really pass this in
in vec3 inNormal;
out vec3 Color;
out vec3 normal;
out vec3 pos;
out vec3 eyePos;
out vec3 lightDir;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
void main() {
   Color = inColor;
   gl_Position = proj * view * model * vec4(position,1.0);
   vec4 pos4 = (view * model * vec4(position,1.0));
   pos = pos4.xyz/pos4.w;  //Convert vec4 pos to vec3 ... typically pos4.w and you don't need the divide
   vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal,0.0);  //OR don't use the inverse-Transpose ... but you'll have to normalize the normal
   normal = norm4.xyz;  //Convert the vec4 to a vec3 ... we don't need to normalize if the above line is correct
   lightDir = (view * vec4(inlightDir,0)).xyz;  //Transform light into to view space
}
