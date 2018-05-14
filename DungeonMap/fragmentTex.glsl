#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec2 texcoord;
in vec3 pointlightPos;
in float pointlightMag;
in vec3 lightDir;
in vec3 lightDir2;
in vec3 eyePosOut;

out vec4 outColor;
//All the textures
uniform samplerCube tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;

uniform vec3 point;
uniform vec2 texScale;

uniform int texID;

const float ambient = 0.2;//0.1;
void main() {
   vec3 color;
   vec3 viewDir = normalize(pos - eyePosOut);
   vec3 reflectDir = reflect(viewDir,normal);
   if (texID == -1)
   	 color = Color;
   else if (texID == 0) {
     outColor = vec4(texture(tex0, pos).rgb, 1);
     return;
   }
   else if (texID == 1) {
     color = texture(tex1, texcoord * texScale).rgb + 0.05 * texture(tex0, reflectDir).rgb;
   }
   else if (texID == 2) {
     color = texture(tex2, texcoord * texScale).rgb + 0.4 * texture(tex0, reflectDir).rgb;
   }
   else if (texID == 3) {
     color = texture(tex3, texcoord * (0.5 * texScale)).rgb;
   }
   else if (texID == 4) {
     color = texture(tex4, texcoord * texScale).rgb + 0.8 * texture(tex0, reflectDir).rgb + texture(tex2, texcoord * texScale).rgb;
   }
   else if (texID == 5) {
     color = texture(tex5, texcoord * (0.5 * texScale)).rgb;
   }
   else{
   	 outColor = vec4(1,0,0,1);
   	 return; //This was an error, stop lighting!
   }
   vec3 pointDir = pointlightPos - pos;
   float magnitude = pointDir.x * pointDir.x + pointDir.y * pointDir.y + pointDir.z * pointDir.z;
   if (magnitude < 1.0) magnitude = 1.0;
   float intensity = pointlightMag / magnitude;
   vec3 diffuseC = 0.3*color*max(dot(pointDir,normal),0.0)*intensity + 0.3*Color*max(dot(-lightDir,normal),0.0) + + 0.1*Color*max(dot(-lightDir2,normal),0.0);
   vec3 ambC = color*ambient;
   vec3 reflectDir2 = reflect(lightDir,normal);
   vec3 reflectDir3 = reflect(lightDir2,normal);
   float spec = max(normalize(dot(reflectDir,pointDir)),0.0);
   float spec2 = max(dot(reflectDir,viewDir),0.0);
   float spec3 = max(dot(reflectDir2,viewDir),0.0);
   if (dot(-pointDir,normal) <= 0.0)spec = 0;
   vec3 specC = 0.5*vec3(1.0,1.0,1.0)*pow(spec,4)*intensity*1.0 + .2*vec3(1.0,1.0,1.0)*pow(spec2,4) + .05*vec3(1.0,1.0,1.0)*pow(spec3,4);
   vec3 oColor = ambC+diffuseC+specC;
   outColor = vec4(oColor,1);
}
