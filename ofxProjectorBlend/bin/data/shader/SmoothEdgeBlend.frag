/**
 * Sept 20 2010
 * 
 * This shader is ripped out of a Quartz Composer patch by  Matthias Oostrik
 * from http://local.wasp.uwa.edu.au/~pbourke/texture_colour/edgeblend/
 * Original Core Image filter converted by James George ( http://www.jamesgeorge.org/ )
 * for ofxProjectorBlend addon for openFrameworks ( http://www.openframeworks.cc )
 *
 * Originally used in the Infiniti MOI project for projector blending ( http://vimeo.com/14588336 )
 *
 * This shader is capable of blending on all sides, but ofxProjectorBlend currently
 * just uses the shader for two projections at once.
 */

uniform sampler2DRect Tex0;
uniform float width;
uniform float height;
uniform float OverlapLeft;
uniform float OverlapRight; 
uniform float OverlapTop;
uniform float OverlapBottom;
uniform float BlackOutLeft;
uniform float BlackOutRight; 
uniform float BlackOutTop;
uniform float BlackOutBottom;
uniform float BlendPower;
uniform float SomeLuminanceControl;
uniform vec3  GammaCorrection;
uniform float SolidEdgeEnable;
uniform vec4  SolidEdgeColor;

vec4 drawSmoothEdge(float loc, float extend, float blePow, float lumCon, vec3 gamma)
{
	vec4 edge;
	float curve;

	curve = loc / extend;
	curve = (curve < 0.5) ? 
		lumCon * pow(2.0 * curve, blePow) 
		: 
		1.0 - (1.0 - lumCon) * pow(2.0 * (1.0 - curve), blePow);
	
	edge = vec4(pow(curve, 1.0 / gamma.r), 
				pow(curve, 1.0 / gamma.g),
				pow(curve, 1.0 / gamma.b),
				1.0);
				 
	return edge;
}


vec4 drawSmoothEdges(in vec4 overlap, in vec4 blankout, in float blePow, in float lumCon, in vec3 gamma, in vec4 color)
{
	vec2 xy = gl_TexCoord[0].xy;
	vec4 blank = vec4(0.0,0.0,0.0,0.0);	
	
	vec4 sebImage = 
		(blankout.x + overlap.x > xy.x) ? 
			(blankout.x > xy.x) ? 	
				blank 							//leftBlankout
			:	(blankout.z + overlap.z > xy.y) ?
					(blankout.z > xy.y) ? 
						blank					//leftBottomBlankOut
					: 	texture2DRect(Tex0, xy) 	*		//leftBottomBlend
						drawSmoothEdge(xy.x - blankout.x, overlap.x, blePow, lumCon, gamma) * 
						drawSmoothEdge(xy.y - blankout.z, overlap.z, blePow, lumCon, gamma)
				: 	(height - blankout.w - overlap.w < xy.y) ?  
						(height - blankout.w < xy.y) ? 
							blank				//leftTopBlankout
						: 	texture2DRect(Tex0, xy) 	*	//LeftTopBlend
							drawSmoothEdge(xy.x - blankout.x, overlap.x, blePow, lumCon, gamma) * 
							drawSmoothEdge(height - blankout.w - xy.y, overlap.w, blePow, lumCon, gamma)					
							:	texture2DRect(Tex0, xy) *		//leftBlankout
						drawSmoothEdge(xy.x - blankout.x, overlap.x, blePow, lumCon, gamma)
		:	(width - blankout.y - overlap.y < xy.x) ? 
				(width - blankout.y < xy.x) ? 
					blank						//rightBlankout
				:	(blankout.z + overlap.z > xy.y) ?
						(blankout.z > xy.y) ? 
							blank 				//rightBottomBlankout
						: 	texture2DRect(Tex0, xy) 	*	//rightBottomBlend
							drawSmoothEdge(width - blankout.y - xy.x, overlap.y, blePow, lumCon, gamma) *
							drawSmoothEdge(xy.y - blankout.z, overlap.z, blePow, lumCon, gamma)
					:	(height - blankout.w - overlap.w < xy.y) ?
							(height - blankout.w < xy.y) ? 
								blank 			//rightTopBlankout
							:	texture2DRect(Tex0, xy) * //rightTopBlend
								drawSmoothEdge(width - blankout.y - xy.x, overlap.y, blePow, lumCon, gamma) * 
								drawSmoothEdge(height - blankout.w - xy.y, overlap.w, blePow, lumCon, gamma)
						:	texture2DRect(Tex0, xy) * 	//rightBlend
							drawSmoothEdge(width - blankout.y - xy.x, overlap.y, blePow, lumCon, gamma)
			:	(blankout.z + overlap.z > xy.y) ?
					(blankout.z > xy.y) ? 
						blank 					//BottomBlankout
					: 	texture2DRect(Tex0, xy) * 		//BottomBlend
						drawSmoothEdge(xy.y - blankout.z, overlap.z, blePow, lumCon, gamma)
				:	(height - blankout.w - overlap.w < xy.y) ?
						(height - blankout.w < xy.y) ? 
							blank 				//TopBlankout
						:	texture2DRect(Tex0, xy) * 	//TopBlend
							drawSmoothEdge(height - blankout.w - xy.y, overlap.w, blePow, lumCon, gamma)					
							:	texture2DRect(Tex0, xy);	
	
	return sebImage;
}

vec4 drawSolidEdges(vec4 overlap, vec4 blankout, vec4 color)
{	
	
	vec2 xy = gl_TexCoord[0].xy;
	vec4 blank = vec4(0.0,0.0,0.0,0.0);		
	vec4 edgeImage =
			(blankout.x + overlap.x > xy.x) ?
				(blankout.x > xy.x) ?
					blank				//leftBlankout
				:	(blankout.z + overlap.z > xy.y) ?
						(blankout.z > xy.y) ?
							blank			//leftBottomBlankout
						: 	color			//leftBottomColor
					: 	(height - blankout.w - overlap.w < xy.y) ?
							(height - blankout.w < xy.y) ?
								blank 		//leftTop
							: 	color 		//leftTopColor
						:	color			//leftColor
			:	(width - blankout.y - overlap.y < xy.x) ?
					(width - blankout.y < xy.x) ?
						blank				//rightBlankout
					:	(blankout.z + overlap.z > xy.y) ?
							(blankout.z > xy.y) ?
								blank 		//rightBottomBlankout
							: 	color		//rightBottomColor
						:	(height - blankout.w - overlap.w < xy.y) ?
								(height - blankout.w < xy.y) ? 
									blank 	//rightTopBlankout
								:	color 	//rightToColor
							:	color 		//rightColor
				:	(blankout.z + overlap.z > xy.y) ?
						(blankout.z > xy.y) ? 
							blank 			//BottomBlankout
						: 	color			//BottomColor
					:	(height - blankout.w - overlap.w < xy.y) ?
							(height - blankout.w < xy.y) ? 
								blank 		//TopBlankout
							:	color		//TopColor
						:	texture2DRect(Tex0, xy);		
	return edgeImage;
}

void main (void)
{
	float SLC = SomeLuminanceControl + 0.5;
	vec3 gamma = GammaCorrection;
	vec4 overlap = vec4 (OverlapLeft, OverlapRight, OverlapBottom, OverlapTop);
	vec4 blankout = vec4 (BlackOutLeft, BlackOutRight, BlackOutBottom, BlackOutTop);
	gl_FragData[0] = (SolidEdgeEnable == 1.0) ?
			drawSolidEdges(overlap, blankout, SolidEdgeColor)
		:	drawSmoothEdges(overlap, blankout, BlendPower, SLC, gamma, SolidEdgeColor);
}
