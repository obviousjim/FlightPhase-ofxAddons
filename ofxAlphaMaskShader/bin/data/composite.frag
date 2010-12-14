uniform sampler2DRect Tex0, Tex1; //these are our texture names, set in openFrameworks on the shader object in set up

void main (void)
{
	//sample both textures
	vec4 image = texture2DRect(Tex0, gl_TexCoord[0].st);
	vec4 composite = texture2DRect(Tex1, gl_TexCoord[1].st);
	
	//use the color from the image, but use the r channel of the mask as the alpha channel of our output
	gl_FragData[0] = vec4(image.rgb,composite.r);  
	
}
