struct VertexOutput {
    @builtin(position) vertexPosition: vec4<f32>,
    @location(0) texturePosition: vec2<f32>
}

@vertex
fn vert_main(@location(0) vert: vec3<f32>, @location(0) uv : vec2<f32>) -> @builtin(position) vec4<f32> {
    var out = VertexOutput();

    out.
    out.texturePosition = uv;

    return out;
}

@fragment
fn frag_main(@location(0) texturePosition) -> f32 {

}