uniform float time;
varying vec3 normal, lightDir, r, I;
const vec3 L = vec3(0.,0.,1.);

void main()
{	
	vec4 pos = gl_Vertex;
	
	float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
	pos.y = sin(12.0 * dist + time / 250) * 0.1;

	gl_Position = gl_ModelViewProjectionMatrix * pos;		


	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(L - vVertex);
	vec4 eyeVec = gl_ProjectionMatrixInverse*vec4(0,0,-1,0);
	
	normal = normalize( gl_NormalMatrix * gl_Normal );
	I = normalize(vVertex - eyeVec.xyz); // Eye to vertex
 	r = refract(I,normal, 0.9);

}

