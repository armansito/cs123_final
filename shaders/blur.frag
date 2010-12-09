const int MAX_KERNEL_SIZE = 25;
uniform sampler2D tex;
uniform vec2 offsets[MAX_KERNEL_SIZE]; 
uniform float kernel[MAX_KERNEL_SIZE];
void main(void) { 
   vec4 sum = vec4(0,0,0,0);
   for (int i = 0; i < MAX_KERNEL_SIZE; i++) {
      sum += kernel[i] * texture2D(tex,gl_TexCoord[0].st + offsets[i]);
   }  
   gl_FragColor = sum;
}
