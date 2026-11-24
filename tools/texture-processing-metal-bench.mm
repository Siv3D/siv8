// Siv3D texture-processing research. Standalone macOS diagnostic, not engine code.
// Copyright (c) 2026 Ryo Suzuki. Licensed under the MIT License.
// Build and interpretation: docs/renderer2d/proposals/texture-processing.md

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Pixel = std::array<_Float16, 4>;
using Clock = std::chrono::steady_clock;

static void Check(bool ok, const char* message)
{
    if (!ok) { throw std::runtime_error(message); }
}

static void Complete(id<MTLCommandBuffer> command)
{
    [command commit];
    [command waitUntilCompleted];
    if (command.status != MTLCommandBufferStatusCompleted)
    {
        throw std::runtime_error(command.error.localizedDescription.UTF8String ?: "GPU command failed");
    }
}

static id<MTLTexture> Texture(id<MTLDevice> device, size_t width, size_t height, bool writable)
{
    auto* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA16Float
        width:width height:height mipmapped:NO];
    desc.storageMode = MTLStorageModePrivate;
    desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget
        | (writable ? MTLTextureUsageShaderWrite : 0);
    id<MTLTexture> texture = [device newTextureWithDescriptor:desc];
    Check(texture != nil, "Texture allocation failed");
    return texture;
}

static void Copy(id<MTLCommandBuffer> command, id<MTLTexture> source, id<MTLTexture> destination)
{
    id<MTLBlitCommandEncoder> encoder = [command blitCommandEncoder];
    [encoder copyFromTexture:source sourceSlice:0 sourceLevel:0 sourceOrigin:MTLOriginMake(0, 0, 0)
        sourceSize:MTLSizeMake(source.width, source.height, 1)
        toTexture:destination destinationSlice:0 destinationLevel:0 destinationOrigin:MTLOriginMake(0, 0, 0)];
    [encoder endEncoding];
}

struct Images
{
    id<MTLTexture> source, rasterTemp, rasterOut, computeTemp, computeOut, ordinaryOut;
};

static Images MakeImages(id<MTLDevice> device, id<MTLCommandQueue> queue,
    size_t width, size_t height, std::vector<Pixel>& pixels)
{
    pixels.resize(width * height);
    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            // Deterministic PMA data with nonuniform alpha, edges, and high frequencies.
            const float alpha = float((x * 7 + y * 11) % 251) / 250.0f;
            pixels[y * width + x] = { _Float16(alpha * float(x % 17) / 16.0f),
                _Float16(alpha * float(y % 13) / 12.0f),
                _Float16(alpha * (((x / 3 + y / 5) % 2) ? 0.8f : 0.1f)), _Float16(alpha) };
        }
    }
    Images result{ Texture(device, width, height, false), Texture(device, width, height, false),
        Texture(device, width, height, false), Texture(device, width, height, true),
        Texture(device, width, height, true), Texture(device, width, height, false) };
    id<MTLBuffer> buffer = [device newBufferWithBytes:pixels.data() length:pixels.size() * sizeof(Pixel)
        options:MTLResourceStorageModeShared];
    Check(buffer != nil, "Upload allocation failed");
    id<MTLCommandBuffer> command = [queue commandBuffer];
    id<MTLBlitCommandEncoder> encoder = [command blitCommandEncoder];
    [encoder copyFromBuffer:buffer sourceOffset:0 sourceBytesPerRow:width * sizeof(Pixel)
        sourceBytesPerImage:pixels.size() * sizeof(Pixel) sourceSize:MTLSizeMake(width, height, 1)
        toTexture:result.source destinationSlice:0 destinationLevel:0 destinationOrigin:MTLOriginMake(0, 0, 0)];
    [encoder endEncoding];
    Copy(command, result.source, result.rasterTemp);
    Copy(command, result.source, result.rasterOut);
    Complete(command);
    return result;
}

static std::vector<Pixel> Read(id<MTLDevice> device, id<MTLCommandQueue> queue, id<MTLTexture> texture)
{
    std::vector<Pixel> pixels(texture.width * texture.height);
    id<MTLBuffer> buffer = [device newBufferWithLength:pixels.size() * sizeof(Pixel)
        options:MTLResourceStorageModeShared];
    Check(buffer != nil, "Readback allocation failed");
    id<MTLCommandBuffer> command = [queue commandBuffer];
    id<MTLBlitCommandEncoder> encoder = [command blitCommandEncoder];
    [encoder copyFromTexture:texture sourceSlice:0 sourceLevel:0 sourceOrigin:MTLOriginMake(0, 0, 0)
        sourceSize:MTLSizeMake(texture.width, texture.height, 1) toBuffer:buffer destinationOffset:0
        destinationBytesPerRow:texture.width * sizeof(Pixel) destinationBytesPerImage:pixels.size() * sizeof(Pixel)];
    [encoder endEncoding];
    Complete(command);
    std::copy_n(static_cast<const Pixel*>(buffer.contents), pixels.size(), pixels.begin());
    return pixels;
}

static std::vector<Pixel> Reference(const std::vector<Pixel>& source, int width, int height,
    const std::vector<double>& weights, bool roundIntermediate)
{
    std::vector<std::array<double, 4>> temp(source.size());
    std::vector<Pixel> result(source.size());
    const int radius = int(weights.size()) - 1;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < 4; ++c)
            {
                double value = 0;
                for (int k = -radius; k <= radius; ++k)
                    value += double(source[y * width + std::clamp(x + k, 0, width - 1)][c]) * weights[std::abs(k)];
                temp[y * width + x][c] = roundIntermediate ? double(_Float16(value)) : value;
            }
        }
    }
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            for (int c = 0; c < 4; ++c)
            {
                double value = 0;
                for (int k = -radius; k <= radius; ++k)
                    value += temp[std::clamp(y + k, 0, height - 1) * width + x][c] * weights[std::abs(k)];
                result[y * width + x][c] = _Float16(value);
            }
        }
    }
    return result;
}

struct Pipelines
{
    id<MTLRenderPipelineState> rasterCopy, rasterBlur;
    id<MTLComputePipelineState> computeCopy, computeBlur, tiledBlur, stripBlur;
    MPSImageGaussianBlur* mps;
};

static Pipelines MakePipelines(id<MTLDevice> device, const std::vector<double>& weights, double sigma)
{
    std::ostringstream code;
    code << "#include <metal_stdlib>\nusing namespace metal;\nconstant int R = " << (weights.size() - 1)
        << ";\nconstant float W[] = {" << std::setprecision(17);
    for (double w : weights) { code << w << "f,"; }
    code << R"(
};
struct Varying { float4 position [[position]]; float2 uv; };
vertex Varying vertexMain(uint id [[vertex_id]]) {
    Varying o;
    o.position = float4(id == 2 ? 3.0 : -1.0, id == 1 ? 3.0 : -1.0, 0, 1);
    o.uv = float2(id == 2 ? 2.0 : 0.0, id == 1 ? -1.0 : 1.0);
    return o;
}
constexpr sampler linearClamp(coord::normalized, address::clamp_to_edge, filter::linear);
float4 blur(texture2d<float> src, float2 uv, uint axis) {
    float2 step = axis == 0 ? float2(1.0 / src.get_width(), 0) : float2(0, 1.0 / src.get_height());
    float4 sum = src.sample(linearClamp, uv, level(0)) * W[0];
    for (int k = 1; k <= R; k += 2) {
        float w1 = W[k];
        float w2 = k < R ? W[k + 1] : 0;
        float total = w1 + w2;
        float2 delta = step * (float(k) + w2 / total);
        sum += total * (src.sample(linearClamp, uv + delta, level(0)) + src.sample(linearClamp, uv - delta, level(0)));
    }
    return sum;
}
fragment float4 copyPS(Varying v [[stage_in]], texture2d<float> src [[texture(0)]]) {
    return src.read(uint2(v.position.xy));
}
fragment float4 blurPS(Varying v [[stage_in]], texture2d<float> src [[texture(0)]], constant uint& axis [[buffer(0)]]) {
    return blur(src, v.uv, axis);
}
kernel void copyCS(texture2d<float, access::read> src [[texture(0)]], texture2d<float, access::write> dst [[texture(1)]], uint2 p [[thread_position_in_grid]]) {
    if (p.x < dst.get_width() && p.y < dst.get_height()) dst.write(src.read(p), p);
}
kernel void blurCS(texture2d<float> src [[texture(0)]], texture2d<float, access::write> dst [[texture(1)]], constant uint& axis [[buffer(0)]], uint2 p [[thread_position_in_grid]]) {
    if (p.x < dst.get_width() && p.y < dst.get_height()) dst.write(blur(src, (float2(p) + 0.5) / float2(src.get_width(), src.get_height()), axis), p);
}
kernel void tiledCS(texture2d<float, access::read> src [[texture(0)]], texture2d<float, access::write> dst [[texture(1)]], uint2 group [[threadgroup_position_in_grid]], uint2 local [[thread_position_in_threadgroup]]) {
    constexpr int T = 8, S = T + 2 * R;
    threadgroup float4 tile[S * S];
    threadgroup float4 horizontal[S * T];
    int lane = int(local.y * T + local.x);
    int2 base = int2(group * T) - R;
    int2 limit = int2(src.get_width(), src.get_height()) - 1;
    for (int i = lane; i < S * S; i += T * T)
        tile[i] = src.read(uint2(clamp(base + int2(i % S, i / S), int2(0), limit)));
    threadgroup_barrier(mem_flags::mem_threadgroup);
    for (int i = lane; i < S * T; i += T * T) {
        int center = (i / T) * S + (i % T) + R;
        float4 sum = tile[center] * W[0];
        for (int k = 1; k <= R; ++k) sum += (tile[center - k] + tile[center + k]) * W[k];
        horizontal[i] = sum;
    }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    uint2 p = group * T + local;
    if (p.x >= dst.get_width() || p.y >= dst.get_height()) return;
    int center = (int(local.y) + R) * T + int(local.x);
    float4 sum = horizontal[center] * W[0];
    for (int k = 1; k <= R; ++k) sum += (horizontal[center - k * T] + horizontal[center + k * T]) * W[k];
    dst.write(sum, p);
}
// Cache horizontal results for a taller output strip. This reduces halo overhead
// compared with the small two-dimensional tile while retaining one dispatch.
kernel void stripCS(texture2d<float> src [[texture(0)]], texture2d<float, access::write> dst [[texture(1)]], uint2 group [[threadgroup_position_in_grid]], uint2 local [[thread_position_in_threadgroup]]) {
    constexpr int X = 8, Y = 32, H = Y + 2 * R;
    threadgroup float4 horizontal[H * X];
    int lane = int(local.y * X + local.x);
    int2 base = int2(group.x * X, group.y * Y);
    float2 dimensions(src.get_width(), src.get_height());
    for (int i = lane; i < H * X; i += 64) {
        int2 p = base + int2(i % X, i / X - R);
        horizontal[i] = blur(src, (float2(p) + 0.5) / dimensions, 0);
    }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    for (int i = lane; i < Y * X; i += 64) {
        uint2 p = uint2(base + int2(i % X, i / X));
        if (p.x >= dst.get_width() || p.y >= dst.get_height()) continue;
        int center = (i / X + R) * X + i % X;
        float4 sum = horizontal[center] * W[0];
        for (int k = 1; k <= R; ++k) sum += (horizontal[center - k * X] + horizontal[center + k * X]) * W[k];
        dst.write(sum, p);
    }
}
)";
    NSError* error = nil;
    auto* options = [MTLCompileOptions new];
    options.mathMode = MTLMathModeSafe;
    id<MTLLibrary> library = [device newLibraryWithSource:[NSString stringWithUTF8String:code.str().c_str()]
        options:options error:&error];
    if (!library) { throw std::runtime_error(error.localizedDescription.UTF8String); }
    auto raster = [&](NSString* name) {
        auto* desc = [MTLRenderPipelineDescriptor new];
        desc.vertexFunction = [library newFunctionWithName:@"vertexMain"];
        desc.fragmentFunction = [library newFunctionWithName:name];
        desc.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA16Float;
        id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:desc error:&error];
        if (!pipeline) { throw std::runtime_error(error.localizedDescription.UTF8String); }
        return pipeline;
    };
    auto compute = [&](NSString* name) {
        id<MTLComputePipelineState> pipeline = [device newComputePipelineStateWithFunction:[library newFunctionWithName:name] error:&error];
        if (!pipeline) { throw std::runtime_error(error.localizedDescription.UTF8String); }
        Check(pipeline.maxTotalThreadsPerThreadgroup >= 64, "64-thread groups unsupported");
        Check(pipeline.staticThreadgroupMemoryLength <= device.maxThreadgroupMemoryLength, "Insufficient threadgroup memory");
        return pipeline;
    };
    auto* mps = [[MPSImageGaussianBlur alloc] initWithDevice:device sigma:float(sigma)];
    Check(mps != nil, "MPS Gaussian construction failed");
    mps.edgeMode = MPSImageEdgeModeClamp;
    return { raster(@"copyPS"), raster(@"blurPS"), compute(@"copyCS"), compute(@"blurCS"),
        compute(@"tiledCS"), compute(@"stripCS"), mps };
}

static void Raster(id<MTLCommandBuffer> command, id<MTLRenderPipelineState> pipeline,
    id<MTLTexture> source, id<MTLTexture> destination, uint32_t axis, bool load = false)
{
    auto* desc = [MTLRenderPassDescriptor renderPassDescriptor];
    desc.colorAttachments[0].texture = destination;
    desc.colorAttachments[0].loadAction = load ? MTLLoadActionLoad : MTLLoadActionDontCare;
    desc.colorAttachments[0].storeAction = MTLStoreActionStore;
    id<MTLRenderCommandEncoder> encoder = [command renderCommandEncoderWithDescriptor:desc];
    [encoder setRenderPipelineState:pipeline];
    [encoder setFragmentTexture:source atIndex:0];
    [encoder setFragmentBytes:&axis length:sizeof(axis) atIndex:0];
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
    [encoder endEncoding];
}

static void Compute(id<MTLCommandBuffer> command, id<MTLComputePipelineState> pipeline,
    id<MTLTexture> source, id<MTLTexture> destination, uint32_t axis, size_t tileHeight = 8)
{
    id<MTLComputeCommandEncoder> encoder = [command computeCommandEncoder];
    [encoder setComputePipelineState:pipeline];
    [encoder setTexture:source atIndex:0];
    [encoder setTexture:destination atIndex:1];
    [encoder setBytes:&axis length:sizeof(axis) atIndex:0];
    [encoder dispatchThreadgroups:MTLSizeMake((destination.width + 7) / 8, (destination.height + tileHeight - 1) / tileHeight, 1)
        threadsPerThreadgroup:MTLSizeMake(8, 8, 1)];
    [encoder endEncoding];
}

static constexpr std::array<const char*, 12> Modes{ "copy-native", "copy-raster", "copy-compute",
    "blur-raster-2pass", "blur-compute-2pass", "blur-compute-tiled", "blur-tiled-plus-copy", "blur-mps-approx",
    "blur-compute-strip", "blur-strip-plus-copy", "blur-raster-writable", "blur-raster-load" };

static id<MTLTexture> Encode(int mode, id<MTLCommandBuffer> command, const Pipelines& pipelines, const Images& images)
{
    switch (mode)
    {
    case 0: Copy(command, images.source, images.ordinaryOut); return images.ordinaryOut;
    case 1: Raster(command, pipelines.rasterCopy, images.source, images.rasterOut, 0); return images.rasterOut;
    case 2: Compute(command, pipelines.computeCopy, images.source, images.computeOut, 0); return images.computeOut;
    case 3:
        Raster(command, pipelines.rasterBlur, images.source, images.rasterTemp, 0);
        Raster(command, pipelines.rasterBlur, images.rasterTemp, images.rasterOut, 1); return images.rasterOut;
    case 4:
        Compute(command, pipelines.computeBlur, images.source, images.computeTemp, 0);
        Compute(command, pipelines.computeBlur, images.computeTemp, images.computeOut, 1); return images.computeOut;
    case 5: Compute(command, pipelines.tiledBlur, images.source, images.computeOut, 0); return images.computeOut;
    case 6:
        Compute(command, pipelines.tiledBlur, images.source, images.computeOut, 0);
        Copy(command, images.computeOut, images.ordinaryOut); return images.ordinaryOut;
    case 7:
        [pipelines.mps encodeToCommandBuffer:command sourceTexture:images.source destinationTexture:images.computeOut];
        return images.computeOut;
    case 8: Compute(command, pipelines.stripBlur, images.source, images.computeOut, 0, 32); return images.computeOut;
    case 9:
        Compute(command, pipelines.stripBlur, images.source, images.computeOut, 0, 32);
        Copy(command, images.computeOut, images.ordinaryOut); return images.ordinaryOut;
    case 10:
        Raster(command, pipelines.rasterBlur, images.source, images.computeTemp, 0);
        Raster(command, pipelines.rasterBlur, images.computeTemp, images.computeOut, 1); return images.computeOut;
    default:
        // MakeImages initialized these attachments; all pixels are overwritten.
        Raster(command, pipelines.rasterBlur, images.source, images.rasterTemp, 0, true);
        Raster(command, pipelines.rasterBlur, images.rasterTemp, images.rasterOut, 1, true); return images.rasterOut;
    }
}

static void Verify(id<MTLDevice> device, id<MTLCommandQueue> queue, const Pipelines& pipelines,
    const std::vector<double>& weights)
{
    // Odd sizes and one-pixel axes exercise partial threadgroups and clamp boundaries.
    for (const auto size : { std::array<int, 2>{ 67, 53 }, { 1, 9 }, { 11, 1 } })
    {
        std::vector<Pixel> source;
        const Images images = MakeImages(device, queue, size[0], size[1], source);
        const auto twoPass = Reference(source, size[0], size[1], weights, true);
        const auto fused = Reference(source, size[0], size[1], weights, false);
        for (int mode = 0; mode < int(Modes.size()); ++mode)
        {
            id<MTLCommandBuffer> command = [queue commandBuffer];
            id<MTLTexture> output = Encode(mode, command, pipelines, images);
            Complete(command);
            const auto actual = Read(device, queue, output);
            const auto& expected = mode < 3 ? source : ((mode < 5 || mode >= 10) ? twoPass : fused);
            double maximum = 0, squared = 0;
            for (size_t i = 0; i < actual.size(); ++i)
            {
                for (int c = 0; c < 4; ++c)
                {
                    const double error = std::abs(double(actual[i][c]) - double(expected[i][c]));
                    Check(std::isfinite(error), "Non-finite output");
                    maximum = std::max(maximum, error);
                    squared += error * error;
                }
            }
            std::printf("check,%dx%d,%s,max=%.8f,rms=%.8f\n", size[0], size[1], Modes[mode], maximum,
                std::sqrt(squared / (actual.size() * 4)));
            // MPS is a different approximation; report its error without asserting equivalence.
            if (mode != 7) Check(maximum <= (mode < 3 ? 0.0 : 0.002), "CPU reference comparison failed");
        }
    }
}

static double Percentile(std::vector<double> values, double fraction)
{
    std::sort(values.begin(), values.end());
    return values[size_t(fraction * (values.size() - 1))];
}

int main(int argc, const char* argv[])
{
    @autoreleasepool
    {
        try
        {
            if (argc != 1 && argc != 7)
            {
                std::fprintf(stderr, "Usage: %s [width height radius sigma iterations samples]\n", argv[0]);
                return 2;
            }
            const int width = argc == 7 ? std::stoi(argv[1]) : 1920;
            const int height = argc == 7 ? std::stoi(argv[2]) : 1080;
            const int radius = argc == 7 ? std::stoi(argv[3]) : 4;
            const double sigma = argc == 7 ? std::stod(argv[4]) : 1.6;
            const int iterations = argc == 7 ? std::stoi(argv[5]) : 8;
            const int samples = argc == 7 ? std::stoi(argv[6]) : 13;
            Check(width > 0 && height > 0 && width <= 8192 && height <= 8192 && radius >= 1 && radius <= 16
                && std::isfinite(sigma) && sigma >= 0.5 && sigma <= 16 && iterations > 0 && iterations <= 256
                && samples >= 3 && samples <= 101, "Invalid dimensions, kernel, or sample count");
            std::vector<double> weights(radius + 1);
            double total = 0;
            for (int k = 0; k <= radius; ++k)
            {
                weights[k] = std::exp(-double(k * k) / (2 * sigma * sigma));
                total += weights[k] * (k ? 2 : 1);
            }
            for (double& w : weights) { w /= total; }
            // Avoid paired coefficients that underflow in the generated float shader.
            Check(weights.back() >= 1e-30, "Kernel tail too small; use a smaller radius");
            id<MTLDevice> device = MTLCreateSystemDefaultDevice();
            Check(device != nil && MPSSupportsMTLDevice(device), "Metal/MPS device unavailable");
            id<MTLCommandQueue> queue = [device newCommandQueue];
            Check(queue != nil, "Command queue creation failed");
            const Pipelines pipelines = MakePipelines(device, weights, sigma);
            std::printf("device,%s\nconfig,%dx%d,RGBA16Float,radius=%d,sigma=%.3f,iterations=%d,samples=%d\n",
                device.name.UTF8String, width, height, radius, sigma, iterations, samples);
            Verify(device, queue, pipelines, weights);
            std::vector<Pixel> source;
            const Images images = MakeImages(device, queue, width, height, source);
            std::array<std::vector<double>, Modes.size()> gpu, cpu;
            for (int round = -3; round < samples; ++round)
            {
                for (int i = 0; i < int(Modes.size()); ++i)
                {
                    @autoreleasepool
                    {
                        const int mode = (i + round + int(Modes.size())) % int(Modes.size());
                        const auto start = Clock::now();
                        id<MTLCommandBuffer> command = [queue commandBuffer];
                        for (int j = 0; j < iterations; ++j) Encode(mode, command, pipelines, images);
                        [command commit];
                        const auto end = Clock::now();
                        [command waitUntilCompleted];
                        Check(command.status == MTLCommandBufferStatusCompleted, "Timed GPU command failed");
                        const double elapsed = command.GPUEndTime - command.GPUStartTime;
                        Check(elapsed > 0, "GPU timestamps unavailable");
                        if (round >= 0)
                        {
                            gpu[mode].push_back(elapsed * 1e6 / iterations);
                            cpu[mode].push_back(std::chrono::duration<double, std::micro>(end - start).count() / iterations);
                        }
                    }
                }
            }
            std::puts("mode,gpu_median_us,gpu_p10_us,gpu_p90_us,cpu_encode_commit_median_us");
            for (size_t i = 0; i < Modes.size(); ++i)
                std::printf("%s,%.3f,%.3f,%.3f,%.3f\n", Modes[i], Percentile(gpu[i], 0.5),
                    Percentile(gpu[i], 0.1), Percentile(gpu[i], 0.9), Percentile(cpu[i], 0.5));
            return 0;
        }
        catch (const std::exception& e)
        {
            std::fprintf(stderr, "error: %s\n", e.what());
            return 1;
        }
    }
}
