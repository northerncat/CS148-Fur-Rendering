/* CS148 Summer 2014
 * Final Project
 * furDemo.frag
 *
 * Name: Tsao-Wei (Brad) Huang
 * ---------------------------
 * This pair of shaders (furDemo.vert and furDemo.frag) is used to present the 
 * effect of making fur segments closer to the skin darker.
 */

// varying variables
varying vec2 furCoord;			// fur texture coordinate of the vertex
varying float currLayer;		// the index of layer the current vertex is in

// unifrom variables
uniform sampler2D furImage;		// fur texture with random cyan dots 
uniform sampler2D colorTexture;	// tiger color texture 

void main()
{
		vec4 fur = texture2D(furImage, furCoord);

		if (currLayer == 0.0) {
			// black background in the skin layer
			fur = vec4(0.0, 0.0, 0.0, 1.0);
		} else if (fur.r < 1.0) {
			// make fur opaque
			fur.a = 1.0;
		} else {
			// make non-fur area transparent
			fur.a = 0.0;
		}

		// create a gradient lightest in the outermost layer, and darkest in the innermost layer
		/*if (currLayer != 0.0) {
			fur.rgb *= sqrt(currLayer);
		}*/

		gl_FragColor = fur;
}