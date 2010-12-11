uniform float time, neighborDist;
uniform int ripples_count;

uniform vec4 ripples[50];
// x: x-coordinate; y: z-coordinate; z: amplitude; w: elapsed time

varying vec3 normal, view, lightDir;

const vec4 light = vec4(0, 20, 0, 1);
const float PI = 3.141593;

const float AMPLITUDE = 1.5;
const float SPEED = .01;
const float ENERGY = 700.0;

const float AMBIENT_AMPLITUDE = 0.3;
const float AMBIENT_SPEED = .01;
const float AMBIENT_ENERGY = 700.0;

void main() {
     	vec4 pos = gl_Vertex;

	// ripple and normal calculation
	pos.y = 0.0;	
	vec3 accNorm = vec3(0, 1, 0);
	for (int i = 0; i < ripples_count; i++) {
	    vec2 dVector = vec2(pos.x - ripples[i].x, pos.z - ripples[i].z);
	    float dist = sqrt(dVector.x * dVector.x + dVector.y * dVector.y);

	    float argument = dist - ripples[i].w * SPEED;
	    if (argument <= 0.0) {
	       float dampening = exp(-ripples[i].w / ENERGY);

	       pos.y += sin(argument) * AMPLITUDE * dampening;

	       accNorm.x += 2.0 * AMPLITUDE * dVector.x * dampening * cos(argument) / dist;
	       accNorm.z += 2.0 * AMPLITUDE * dVector.y * dampening * cos(argument) / dist;
	    }
	}

	// this is the background noise
	/*
	float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
	float arg = dist - time * AMBIENT_SPEED;
	pos.y += sin(arg) * AMBIENT_AMPLITUDE;
	accNorm.x += 2.0 * AMBIENT_AMPLITUDE * pos.x * cos(arg) / dist;
	accNorm.z += 2.0 * AMBIENT_AMPLITUDE * pos.z * cos(arg) / dist;
	*/
	normal = normalize(accNorm);

	// comment in the below for still water
	//normal = vec3(0, 1, 0);	

	lightDir = light.xyz - pos.xyz;

	// this is a hack -- the second term represents eye position
	view = pos.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz;

	gl_Position = gl_ModelViewProjectionMatrix * pos;
}
