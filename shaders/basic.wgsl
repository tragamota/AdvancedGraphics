@group(0) @binding(0) var accumulatorTexture: texture_2d<f32>;
@group(0) @binding(1) var accumulatorSampler: sampler;

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

    out.vertexPosition = vec4<f32>(vertexInput.vert.xy, 1.0, 1.0);
    out.texturePosition = vertexInput.uv;

    return out;
}

@fragment
fn frag_main(texturePosition: VertexOutput) -> @location(0) vec4<f32> {
    return textureSample(accumulatorTexture, accumulatorSampler, texturePosition.texturePosition);
}