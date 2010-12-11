uniform float time, neighborDist;
uniform int ripples_count;
uniform vec4 ripples[50];

varying vec3 normal, view, lightDir;

const vec4 light = vec4(0, 5, -20, 1);
const float PI = 3.141593;

const float WAVELENGTH = 4.0;
const float AMPLITUDE = 0.8;
const float SPEED = .002;
const vec2 CENTER = vec2(0.0, 0.0);
const float ENERGY = 5.0;

float wave(float xcoord, float zcoord) {
      	float frequency = 2.0 * PI / WAVELENGTH;
	float phase = SPEED * frequency;

	vec2 dVector = vec2(xcoord - CENTER.x, zcoord - CENTER.y);
	float dist = sqrt(dVector.x * dVector.x + dVector.y * dVector.y);

	float waveHeight = sin(frequency * dist - time * phase);
	return AMPLITUDE * exp(-(dist / ENERGY)) * waveHeight;
}

float waveHeight(float xcoord, float zcoord) {
      float height = wave(xcoord, zcoord);

      return height;
}

void main() {	
	vec4 pos = gl_Vertex;

	//pos.y = waveHeight(pos.x, pos.z);
	
	float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
	pos.y = sin(12.0 * dist + time / 250.0) * 0.2;

	// ripple calculation
	vec4 ripple;
	for (int i = 0; i < ripples_count; i++) {
		ripple = ripples[i];
		dist = sqrt((pos.x - ripple.x)*(pos.x - ripple.x) + (pos.z - ripple.z)*(pos.z - ripple.z));
		if (dist <= ripple.w / 100.0) pos.y = pos.y + sin(dist - ripple.w / 100.0) * exp(-ripple.w/2000.0);
	}
	
	vec4 right = vec4(pos.x + neighborDist, 0, pos.z, pos.w);
	right.y = waveHeight(right.x, right.z);
	vec4 up = vec4(pos.x, 0, pos.z + neighborDist, pos.w);
	up.y = waveHeight(up.x, up.z);

	normal = normalize(cross(right.xyz - pos.xyz, up.xyz - pos.xyz));
	// comment in the below for still water
	// normal = vec3(0, 1, 0);	

	vec4 worldLight = gl_ModelViewProjectionMatrixInverse * light;
	lightDir = worldLight.xyz - pos.xyz;

	// this is a hack -- the second term represents eye position
	view = pos.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz;

	gl_Position = gl_ModelViewProjectionMatrix * pos;
}
