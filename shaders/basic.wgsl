


@vertex
fn vert_main(@location(0) p : vec4<f32>, @location(1) t : vec2<f32>) -> @builtin(position) vec4<f32> {
    return p;
}

@fragment
fn frag_main() {

}