uniform float time;
varying vec3 normal, lightDir, r;
uniform int ripples_count;
uniform vec4 ripples[ripples_count];
const vec3 L = vec3(0.,0.,1.);

void main()
{	
	vec4 pos = gl_Vertex;
	
	float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
	pos.y = sin(12.0 * dist + time / 250.0) * 0.1;
	
	// ripple calculation
//	dist = sqrt((pos.x - ripple.x)*(pos.x - ripple.x) + (pos.z - ripple.z)*(pos.z - ripple.z));
	pos.y = pos.y + sin(dist - time / 250.0) ;

	gl_Position = gl_ModelViewProjectionMatrix * pos;		


	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(L - vVertex);
	vec4 eyeVec = gl_ProjectionMatrixInverse*vec4(0,0,-1,0);
	
	normal = normalize( gl_NormalMatrix * gl_Normal );
	vec3 I = normalize(vVertex - eyeVec.xyz); // Eye to vertex
 	r = refract(I,normal, 0.9);

}

