#version 450 core

#define FXAA_EDGE_THRESHOLD_MAX       (1.0 / 8.0)  // The minimum amount of local contrast required to apply algorithm
#define FXAA_EDGE_THRESHOLD_MIN      (1.0 / 16.0) // Trims the algorithm from processing darks
#define FXAA_SUBPIX_QUALITY           (3.0 / 4.0)
#define FXAA_ITERATIONS              12
#define FXAA_SEARCH_ACCELERATION     2

layout (binding = 0) uniform sampler2D frame_tex;
uniform vec2 inv_scren_size;
uniform int is_debug;

in vec2 frag_uv;

out vec4 frag_output;

// References:
// Presentation slides: http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// Implementation: http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
// Original Source code: https://gist.github.com/kosua20/0c506b81b3812ac900048059d2383126
float luminance(vec3 rgb)
{
    return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

void main()
{
	vec3 center = texture(frame_tex,frag_uv).rgb;
	
	// Luma at the current fragment
	float luma_center = luminance(center);
	
	// Luma at the four direct neighbours of the current fragment.
	float luma_down  = luminance(textureOffset(frame_tex, frag_uv, ivec2( 0,-1)).rgb);
	float luma_up 	 = luminance(textureOffset(frame_tex, frag_uv, ivec2( 0, 1)).rgb);
	float luma_left  = luminance(textureOffset(frame_tex, frag_uv, ivec2(-1, 0)).rgb); 
	float luma_right = luminance(textureOffset(frame_tex, frag_uv, ivec2( 1, 0)).rgb);
  	
	// Find the maximum and minimum luma around the current fragment.  
	float range_min = min(luma_center,min(min(luma_down,luma_up),min(luma_left,luma_right )));
	float range_max = max(luma_center,max(max(luma_down,luma_up),max(luma_left,luma_right)));
	
	// Compute the delta.
	float range = range_max - range_min;
	
	// If the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA.
	if(range < max(FXAA_EDGE_THRESHOLD_MIN,range_max*FXAA_EDGE_THRESHOLD_MAX)){
		frag_output = vec4(center, 1.0);
		return;
	}
    if (is_debug == 1)
    {
        frag_output = vec4(1, 0, 0, 1);
        return;
    }
	
	// Query the 4 remaining corners lumas.
	float luma_down_left  = luminance(textureOffset(frame_tex, frag_uv, ivec2(-1,-1)).rgb);
	float luma_up_right   = luminance(textureOffset(frame_tex, frag_uv, ivec2( 1, 1)).rgb);
	float luma_up_left 	  = luminance(textureOffset(frame_tex, frag_uv, ivec2(-1, 1)).rgb);
	float luma_down_right = luminance(textureOffset(frame_tex, frag_uv, ivec2( 1,-1)).rgb);
 	
	// Combine the four edges lumas (using  intermediary variables for future computations with the same values).
	float luma_down_up    = luma_down + luma_up; 
	float luma_left_right = luma_left + luma_right;
	
	// Same for corners
	float luma_lefts  = luma_down_left + luma_up_left;
	float luma_downs  = luma_down_left + luma_down_right;
	float luma_rights = luma_down_right + luma_up_right; 
	float luma_ups    = luma_up_right + luma_up_left;
 	
	// Compute an estimation of the gradient along the horizontal and vertical axis. 
	float edge_horz = abs(-2.0 * luma_left + luma_lefts)	+ abs(-2.0 * luma_center + luma_down_up ) * 2.0	+ abs(-2.0 * luma_right + luma_rights);
	float edge_vert   = abs(-2.0 * luma_up + luma_ups   )		+ abs(-2.0 * luma_center + luma_left_right) * 2.0	+ abs(-2.0 * luma_down + luma_downs);
	
	// Is the local edge horizontal or vertical ?
	bool is_horz = (edge_horz >= edge_vert);
	
	// Choose the step size (one pixel) accordingly.
	float step_length = is_horz ? inv_scren_size.y : inv_scren_size.x;
 	
	// Select the two neighboring texels lumas in the  opposite direction to the local edge.
	float luma1 = is_horz ? luma_down : luma_left;
	float luma2 = is_horz ? luma_up : luma_right;
	// Compute gradients in this direction.
	float gradient1 = luma1 - luma_center;
	float gradient2 = luma2 - luma_center;
	
	// Which direction is the steepest ?
	bool steepest = abs(gradient1) >= abs(gradient2);
	
	// Gradient in the corresponding direction, normalized.
	float gradient_scale = 0.25*max(abs(gradient1),abs(gradient2));
	
	// Average luma in the correct direction.
	float avg_luma = 0.0;
	if(steepest){
		// Switch the direction
		step_length = - step_length;
		avg_luma = 0.5*(luma1 + luma_center);
	} else {
		avg_luma = 0.5*(luma2 + luma_center);
	}
	
	// Shift UV in the correct direction by half a pixel.
	vec2 uv = frag_uv;
	if(is_horz){
		uv.y += step_length * 0.5;
	} else {
		uv.x += step_length * 0.5;
	}
	
	// Compute offset (for each iteration step) in the right direction.
	vec2 offset = is_horz ? vec2(inv_scren_size.x,0.0) : vec2(0.0,inv_scren_size.y);
	// Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
	vec2 uv_neg = uv - offset;
	vec2 uv_pos = uv + offset;
	
	// Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
	float luma_neg = luminance(texture(frame_tex, uv_neg).rgb) - avg_luma;
	float luma_pos = luminance(texture(frame_tex, uv_pos).rgb) - avg_luma;
	
	// If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
	bool done_neg = abs(luma_neg) >= gradient_scale;
	bool done_pos = abs(luma_pos) >= gradient_scale;
	
	// If the side is not reached, we continue to explore in this direction.
	if(!done_neg) uv_neg -= offset;
	if(!done_pos) uv_pos += offset;
	
	// If both sides have not been reached, continue to explore.
		
    for(int i = 2; i < FXAA_ITERATIONS; i++)
    {
        // If needed, read luma in negative direction, compute delta.
        if(!done_neg)
            luma_neg = luminance(texture(frame_tex, uv_neg).rgb) - avg_luma;
        // If needed, read luma in psoitive direction, compute delta.
        if(!done_pos)
            luma_pos = luminance(texture(frame_tex, uv_pos).rgb) - avg_luma;
        // If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
        done_neg = abs(luma_neg) >= gradient_scale;
        done_pos = abs(luma_pos) >= gradient_scale;
        
         if (done_neg && done_pos)
            break;
		
        // If the side is not reached, we continue to explore in this direction, with a variable quality.
        if(!done_neg) uv_neg -= offset * FXAA_SEARCH_ACCELERATION;
        if(!done_pos) uv_pos += offset * FXAA_SEARCH_ACCELERATION;
    }
        

	
	// Compute the distances to each side edge of the edge (!).
	float dist_neg = is_horz ? (frag_uv.x - uv_neg.x) : (frag_uv.y - uv_neg.y);
	float dist_pos = is_horz ? (uv_pos.x - frag_uv.x) : (uv_pos.y - frag_uv.y);
	
	// In which direction is the side of the edge closer ?
	bool is_direction_neg = dist_neg < dist_pos;
	float dist = min(dist_neg, dist_pos);
	
	// Thickness of the edge.
	float edge_length = (dist_neg + dist_pos);
	
	// Is the luma at center smaller than the local average ?
	bool is_luma_smaller = luma_center < avg_luma;
	// If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
    bool correction = ((is_direction_neg ? luma_neg : luma_pos) < 0.0) != is_luma_smaller;
	
	// UV offset: read in the direction of the closest side of the edge.
	// If the luma variation is incorrect, do not offset.
	float final_offset = correction ? - dist / edge_length + 0.5 : 0.0;
	
	// Sub-pixel shifting
	// Full weighted average of the luma over the 3x3 neighborhood.
	avg_luma = (1.0/12.0) * (2.0 * (luma_down_up + luma_left_right) + luma_lefts + luma_rights);
	// Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
	float tmp = clamp(abs(avg_luma - luma_center)/range,0.0,1.0);
	tmp = (-2.0 * tmp + 3.0) * tmp * tmp;
	// Compute a sub-pixel offset based on this delta.
	float sub_pixel_offset_final = tmp * tmp * FXAA_SUBPIX_QUALITY;
	
	// Pick the biggest of the two offsets.
	final_offset = max(final_offset,sub_pixel_offset_final);
	
	// Compute the final UV coordinates.
	vec2 final_uv = frag_uv;
	if(is_horz){
		final_uv.y += final_offset * step_length;
	} else {
		final_uv.x += final_offset * step_length;
	}
	
	// Read the color at the new UV coordinates, and use it.
	vec3 finalColor = texture(frame_tex, final_uv).rgb;
	frag_output = vec4(finalColor, 1.0);
    // frag_output = vec4(1,0,0,1);
	
}