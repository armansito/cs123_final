uniform float time, neighborDist;
uniform int ripples_count;

uniform vec4 ripples[50];
// x: x-coordinate; y: z-coordinate; z: amplitude; w: elapsed time

uniform vec2 other_vals[50];
// x: energy y: speed

varying vec3 normal, view, lightDir;

const vec4 light = vec4(-40, 50, 40, 1);

const float AMBIENT_AMPLITUDE = 0.1;
const float AMBIENT_SPEED = .005;
const float AMBIENT_ENERGY = 5000000.0;
const vec2 AMBIENT_CENTER = vec2(100.3, 0.0);

void main() {
	vec4 pos = gl_Vertex;
		
	// ripple and normal calculation
	pos.y = 0.0;	
	vec3 accNorm = vec3(0, -1, 0);
	for (int i = 0; i < ripples_count; i++) {
		vec2 dVector = vec2(pos.x - ripples[i].x, pos.z - ripples[i].y);
		float dist = sqrt(dVector.x * dVector.x + dVector.y * dVector.y);
		
		float argument = dist - ripples[i].w * other_vals[i].y;
		if (argument <= 0.0) {
			float dampening = exp(-ripples[i].w / other_vals[i].x) * ripples[i].z;
			
			pos.y += sin(argument) * dampening;
			float devTerm = dampening * cos(argument) / dist;

			accNorm.x += dVector.x * devTerm;
			accNorm.z += dVector.y * devTerm;
		}
	}

	// this is the background noise
	vec2 distVec = vec2(pos.x - AMBIENT_CENTER.x, pos.z - AMBIENT_CENTER.y);
	float dist = sqrt(distVec.x * distVec.x + distVec.y * distVec.y);

	float arg = dist - time * AMBIENT_SPEED;
	float dampen = exp(-time / AMBIENT_ENERGY) * AMBIENT_AMPLITUDE;
	pos.y += sin(arg) * dampen;
	float term = dampen * cos(arg) / dist;

	accNorm.x += distVec.x * term;
	accNorm.z += distVec.y * term;

	normal = normalize(accNorm);
	
	// comment in the below for still water
	//normal = vec3(0, 1, 0);	

	lightDir = light.xyz - pos.xyz;

	// this is a hack -- the second term represents eye position
	view = pos.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz;

	gl_Position = gl_ModelViewProjectionMatrix * pos;
}
