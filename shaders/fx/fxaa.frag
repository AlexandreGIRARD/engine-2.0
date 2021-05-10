#version 450 core

#define FXAA_EDGE_THRESHOLD_MAX       (1.0 / 8.0)  // The minimum amount of local contrast required to apply algorithm
#define FXAA_EDGE_THRESHOLD_MIN      (1.0 / 16.0) // Trims the algorithm from processing darks
#define FXAA_SUBPIX 1
#define FXAA_SUBPIX_TRIM              (1.0 / 4.0) // Controls removal of sub-pixel aliasing
#define FXAA_SUBPIX_TRIM_SCALE       1
#define FXAA_SUBPIX_TRIM_CAP          (7.0 / 8.0) // Insures fine detail is not completely removed.
#define FXAA_ITERATIONS              12
#define FXAA_SEARCH_ACCELERATION     2

layout (binding = 0) uniform sampler2D frame_tex;
uniform vec2 inv_scren_size;

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
    vec3 center = texture(frame_tex, frag_uv).rgb;

    // Local Contrast Check
    float luma_center = luminance(center);
    float luma_up    = luminance(textureOffset(frame_tex, frag_uv, ivec2( 0, 1)).rgb);
    float luma_left  = luminance(textureOffset(frame_tex, frag_uv, ivec2(-1, 0)).rgb);
    float luma_down  = luminance(textureOffset(frame_tex, frag_uv, ivec2( 0,-1)).rgb);
    float luma_right = luminance(textureOffset(frame_tex, frag_uv, ivec2( 1, 0)).rgb);

    float range_min  = min(luma_center, min(min(luma_up, luma_right), min(luma_down, luma_left)));
    float range_max  = max(luma_center, max(max(luma_up, luma_right), max(luma_down, luma_left)));
    float range      = range_max - range_min;
    
    // Early exit
    if (range < max(FXAA_EDGE_THRESHOLD_MIN, range_max * FXAA_EDGE_THRESHOLD_MAX))
    {
        frag_output= vec4(center, 1.0);
        return;
    }
    
    float luma_up_left    = luminance(textureOffset(frame_tex, frag_uv, ivec2(-1, 1)).rgb);
    float luma_up_right   = luminance(textureOffset(frame_tex, frag_uv, ivec2( 1, 1)).rgb);
    float luma_down_left  = luminance(textureOffset(frame_tex, frag_uv, ivec2(-1,-1)).rgb);
    float luma_down_right = luminance(textureOffset(frame_tex, frag_uv, ivec2( 1,-1)).rgb);
    float luma_down_up    = luma_down + luma_up;
    float luma_left_right = luma_left + luma_right;
    float luma_lefts      = luma_down_left + luma_up_left;
    float luma_rights     = luma_down_right + luma_up_right;
    float luma_ups        = luma_up_left + luma_up_right;
    float luma_downs      = luma_down_left + luma_down_right;
    // vec3 low_pass = (up + left + mid + down + right + up_left + up_right + down_left + down_right) / 9.f;

    // Edge Detection = Sobel filtering like
    float edge_vert = abs(-2 * luma_left + luma_lefts) + abs(-2 * luma_center + luma_down_up) * 2.0 + abs(-2 * luma_right + luma_rights);
    float edge_horz = abs(-2 * luma_up + luma_ups) + abs(-2 * luma_center + luma_left_right) * 2.0 + abs(-2 * luma_down + luma_downs);
    bool is_horz = edge_horz >= edge_vert;

    // Compute Gradient
    // Select the two neighboring texels lumas in the opposite direction to the local edge.
    float luma1 = is_horz ? luma_down : luma_left;
    float luma2 = is_horz ? luma_up : luma_right;
    // Compute gradients in this direction.
    float gradient1 = luma1 - luma_center;
    float gradient2 = luma2 - luma_center;

    bool steepest = abs(gradient1) >= abs(gradient2);
    float gradient_scale = 0.25 * max(abs(gradient1), abs(gradient2));
    float step_length = is_horz ? inv_scren_size.y : inv_scren_size.x; 

    float avg_luma = 0.f;
    if (steepest)
    { // Switch direction
        step_length = -step_length;
        avg_luma = 0.5 * (luma1 + luma_center);
    }
    else
    {
        avg_luma = 0.5 * (luma2 + luma_center);
    }
    // Shift uv
    vec2 uv = frag_uv;
    if (is_horz)
        uv.x += step_length * 0.5;
    else
        uv.y += step_length * 0.5;


    // First iteration along gradient
    vec2 offset = is_horz ? vec2(inv_scren_size.x, 0) : vec2(0, inv_scren_size.y);

    vec2 uv_neg = uv - offset;
    vec2 uv_pos = uv + offset;

    float luma_neg = luminance(texture(frame_tex, uv_neg).rgb) - avg_luma;
    float luma_pos = luminance(texture(frame_tex, uv_pos).rgb) - avg_luma;

    bool done_neg = abs(luma_neg) >= gradient_scale;
    bool done_pos = abs(luma_pos) >= gradient_scale;
    bool done_both = done_neg && done_pos;

    if (!done_neg) uv_neg -= offset;
    if (!done_pos) uv_pos += offset;

    for (int i = 2; i < FXAA_ITERATIONS; i++)
    {
        if (!done_neg) 
            luma_neg = luminance(texture(frame_tex, uv_neg).rgb) - avg_luma;
        if (!done_pos) 
            luma_pos = luminance(texture(frame_tex, uv_pos).rgb) - avg_luma;
        
        // Check extremities
        done_neg = done_neg || abs(luma_neg) >= gradient_scale;
        done_pos = done_pos || abs(luma_pos) >= gradient_scale;
        
        if (done_neg && done_pos)
            break;

        if (!done_neg) uv_neg -= offset * FXAA_SEARCH_ACCELERATION;
        if (!done_pos) uv_pos += offset * FXAA_SEARCH_ACCELERATION;
    }

    float dist_neg = is_horz ? uv.x - uv_neg.x : uv.y - uv_neg.y;
    float dist_pos = is_horz ? uv.x - uv_pos.x : uv.y - uv_pos.y;

    float edge_length = dist_neg + dist_pos;
    float pixel_offset = - min(dist_neg, dist_pos) / edge_length + 0.5;

    avg_luma = (1.f/12.f) * (2 * (luma_down_up + luma_left_right) + luma_lefts + luma_rights);
    float sub_pixel_offset1 = clamp(abs(avg_luma - luma_center) / range, 0.0, 1.0);
    float sub_pixel_offset2 = (-2 * sub_pixel_offset1 + 3.0) * sub_pixel_offset1 * sub_pixel_offset1;

    float sub_pixel_offset = sub_pixel_offset2 * sub_pixel_offset2; // * QUALITY
    float final_offset = max(pixel_offset, sub_pixel_offset);

    vec2 final_uv = frag_uv;
    if (is_horz)
        final_uv.y += final_offset * step_length;
    else
        final_uv.x += final_offset * step_length;

    
    frag_output = vec4(texture(frame_tex, final_uv).rgb, 1.0);
}