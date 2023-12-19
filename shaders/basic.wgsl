struct VertexInput {
    @location(0) vert: vec2<f32>,
    @location(1) uv : vec2<f32>
}

struct VertexOutput {
    @builtin(position) vertexPosition: vec4<f32>,
    @location(0) texturePosition: vec2<f32>
}

@vertex
fn vert_main(vertexInput: VertexInput) -> VertexOutput {
    var out: VertexOutput;

    out.vertexPosition = vec4<f32>(vertexInput.vert.xy, 0, 0);
    out.texturePosition = vertexInput.uv;

    return out;
}

@fragment
fn frag_main(texturePosition: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(0.8, 0.3, 0.2, 1.0);
}