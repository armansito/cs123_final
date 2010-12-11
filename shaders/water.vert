uniform float time;
varying vec3 normal, lightDir, r;
uniform int ripples_count;
uniform vec4 ripples[50];
const vec3 L = vec3(0.,0.,1.);

void main()
{	
	vec4 pos = gl_Vertex;
	
	float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
	pos.y = sin(12.0 * dist + time / 250.0) * 0.2;
	
	// ripple calculation
	vec4 ripple;
	for (int i = 0; i < ripples_count; i++) {
		ripple = ripples[i];
		dist = sqrt((pos.x - ripple.x)*(pos.x - ripple.x) + (pos.z - ripple.z)*(pos.z - ripple.z));
		pos.y = pos.y + sin(dist - ripple.w / 100.0) * exp(-ripple.w/2000.0) ;
  }

	gl_Position = gl_ModelViewProjectionMatrix * pos;		


	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(L - vVertex);
	vec4 eyeVec = gl_ProjectionMatrixInverse*vec4(0,0,-1,0);
	
	normal = normalize( gl_NormalMatrix * gl_Normal );
	vec3 I = normalize(vVertex - eyeVec.xyz); // Eye to vertex
 	r = refract(I,normal, 0.9);

}

