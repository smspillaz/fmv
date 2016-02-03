/*
    Copyright © 2016 Sam Spilsbury <s@polysquare.org>
    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>

#include <array>

#include <iostream>

#include <libvisual/libvisual.h>
#include <libvisual/lv_common.h>
#include <libvisual/lv_input.h>
#include <libvisual/lv_buffer.h>

#include <Magnum/AbstractShaderProgram.h>
#include <Magnum/Buffer.h>
#include <Magnum/Context.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Renderer.h>
#include <Magnum/Shader.h>
#include <Magnum/Version.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Drawable.hpp>
#include <Magnum/SceneGraph/Camera3D.hpp>
#include <Magnum/Framebuffer.h>
#include <Magnum/Timeline.h>
#include "Magnum/Shaders/Generic.h"
#include "Magnum/Texture.h"
#include <Magnum/Image.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/PixelStorage.h>
#include <Magnum/TextureFormat.h>

#include </home/smspillaz/Source/magnum/src/Magnum/Shaders/Implementation/CreateCompatibilityShader.h>


namespace Magnum { namespace Examples {

typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

static Trade::MeshData3D barPrimitive(Vector3 const &d) {
    return Trade::MeshData3D(MeshPrimitive::Triangles, {
         0,  1,  2,  0,  2,  3, /* +Z */
         4,  5,  6,  4,  6,  7, /* +X */
         8,  9, 10,  8, 10, 11, /* +Y */
        12, 13, 14, 12, 14, 15, /* -Z */
        16, 17, 18, 16, 18, 19, /* -Y */
        20, 21, 22, 20, 22, 23  /* -X */
    }, {{
        {0.0f, 0.0f,  0.0f},
        { d.x(), 0.0f, 0.0f},
        { d.x(),  d.y(),  0.0f}, /* +Z */
        {0.0f,  d.y(),  0.0f},

        { d.x(), 0.0f,  0.0f},
        { d.x(), 0.0f, -d.z()},
        { d.x(),  d.y(), -d.z()}, /* +X */
        { d.x(),  d.y(),  0.0f},

        {0.0f,  d.y(),  -d.z() + d.z()},
        { d.x(),  d.y(),  0.0f},
        { d.x(),  d.y(), -d.z()}, /* +Y */
        {0.0f,  d.y(), -d.z()},

        { d.x(), 0.0f, -d.z()},
        {0.0f, 0.0f, -d.z()},
        {0.0f,  d.y(), -d.z()}, /* -Z */
        { d.x(),  d.y(), -d.z()},

        {0.0f, 0.0f, -d.z()},
        { d.x(), 0.0f, -d.z()},
        { d.x(), 0.0f,  0.0f}, /* -Y */
        {0.0f, 0.0f,  0.0f},

        {0.0f, 0.0f, -d.z()},
        {0.0f, 0.0f,  0.0f},
        {0.0f,  d.y(),  0.0f}, /* -X */
        {0.0f,  d.y(), -d.z()}
    }}, {{
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f},
        { 0.0f,  0.0f,  1.0f}, /* +Z */
        { 0.0f,  0.0f,  1.0f},

        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f},
        { 1.0f,  0.0f,  0.0f}, /* +X */
        { 1.0f,  0.0f,  0.0f},

        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f}, /* +Y */
        { 0.0f,  1.0f,  0.0f},

        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f},
        { 0.0f,  0.0f, -1.0f}, /* -Z */
        { 0.0f,  0.0f, -1.0f},

        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f}, /* -Y */
        { 0.0f, -1.0f,  0.0f},

        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f},
        {-1.0f,  0.0f,  0.0f}, /* -X */
        {-1.0f,  0.0f,  0.0f}
    }}, {});
}

class ZoomBlurShader :
    public AbstractShaderProgram
{
public:
    Int intensityUniform;
    Int dimensionsUniform;
    Int textureUniform;

    ZoomBlurShader & setIntensity(float intensity) {
        setUniform(intensityUniform, intensity);
        return *this;
    }

    ZoomBlurShader & setTexture(Magnum::Texture2D &texture) {
        texture.bind(0);
        return *this;
    }

    explicit ZoomBlurShader() :
        intensityUniform(0)
    {
        const Version version = Context::current().supportedVersion({Version::GL320, Version::GL310, Version::GL300, Version::GL210});

        const char *vss = "attribute vec4 position;\n" \
                          "attribute vec2 texCoord;\n" \
                          "varying vec2 vTexCoord;\n" \
                          "void main() {\n" \
                          "    vTexCoord = texCoord;\n" \
                          "    gl_Position = position;\n" \
                          "}\n";
        const char *fss = "uniform sampler2D textureUnit;\n" \
                          "uniform float intensity;\n" \
                          "uniform vec2 dimensions;\n" \
                          "varying vec2 vTexCoord;\n" \
                          "void main() {\n" \
                          "    vec2 center = dimensions / 2.0;\n" \
                          "    vec2 vectorPointingToCenter = vec2(center - vTexCoord * dimensions);\n" \
                          "    vec4 total = vec4(0, 0, 0, 0);\n"
                          "    for (float i = 0.0; i < 40.0; i += 1.0) {\n" \
                          "        float pc = i / 40.0;\n" \
                          "        vec2 offset = ((pc * vectorPointingToCenter) / dimensions) * intensity;\n" \
                          "        vec4 sample = texture2D(textureUnit, vTexCoord + offset);\n" \
                          "        sample.rgb *= sample.a;\n" \
                          "        total += (sample * 4.0 * (pc - pc * pc)) / 40.0f;\n" \
                          "    }\n"
                          "    gl_FragColor = total;\n" \
                          "    gl_FragColor.rgb /= (total.a + 0.0000001);\n"
                          "}\n";

        Shader vertex(version, Shader::Type::Vertex);
        Shader fragment(version, Shader::Type::Fragment);

        vertex.addSource(vss);
        fragment.addSource(fss);

        CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vertex, fragment}));

        attachShaders({vertex, fragment});

        CORRADE_INTERNAL_ASSERT_OUTPUT(link());

        bindAttributeLocation(Shaders::Generic3D::Position::Location, "position");
        bindAttributeLocation(Shaders::Generic3D::TextureCoordinates::Location, "texCoord");

        intensityUniform = uniformLocation("intensity");
        dimensionsUniform = uniformLocation("dimensions");
        textureUniform = uniformLocation("textureUnit");

        setUniform(dimensionsUniform, Vector2(800, 600));
        setUniform(textureUniform, 0);
    }
};

class BarShader :
    public AbstractShaderProgram
{
public:
    Int colorUniform;
    Int lightUniform;
    Int modelviewUniform;
    Int projectionUniform;
    Int normalUniform;

    typedef typename Shaders::Generic<3>::Position Position;
    typedef Attribute<4, Vector3> Barycentric;

    BarShader & setColor(Color4 color) {
        setUniform(colorUniform, color);
        return *this;
    }

    BarShader & setLightPosition(Vector3 position) {
        setUniform(lightUniform, position);
        return *this;
    }

    BarShader & setModelviewMatrix(const MatrixTypeFor<3, Float>& matrix) {
        setUniform(modelviewUniform, matrix);
        return *this;
    }

    BarShader & setNormalMatrix(const Matrix3x3 &matrix) {
        setUniform(normalUniform, matrix);
        return *this;
    }

    BarShader & setProjectionMatrix(const MatrixTypeFor<3, Float> &matrix) {
        setUniform(projectionUniform, matrix);
        return *this;
    }

    explicit BarShader() {
        const Version version = Context::current().supportedVersion({Version::GL320, Version::GL310, Version::GL300, Version::GL210});

        Shader vert(version, Shader::Type::Vertex);
        Shader frag(version, Shader::Type::Fragment);

        const char *fragment_shader_source = \
                "varying vec4 varyingColor;\n" \
                "varying vec4 varyingPosition;\n" \
                "varying vec3 varyingBarycentric;\n" \
                "void main() {\n" \
                "    /* Determine how far away we are from 0.5, 0.5 and multiply varyingColor by that */\n" \
                "    float distance = varyingPosition.y;\n" \
                "    if (any(lessThan(vec3(varyingBarycentric.xz, 1), vec3(0.02)))) {\n" \
                "        gl_FragColor = varyingColor;\n"
                "    } else {\n" \
                "        vec4 mixFactor = varyingColor * mix((1.0 - distance), 1.0, varyingColor.a * 0.2);\n" \
                "        vec4 highlight = varyingColor * 0.2;\n" \
                "        gl_FragColor = mixFactor + highlight;\n" \
                "    }\n"
                "}\n";

        const char *vertex_shader_source = \
                "uniform mat4 modelview;\n" \
                "uniform mat4 projection;\n" \
                "uniform mat3 normalMatrix;\n" \
                "uniform vec4 color;\n"\
                "uniform vec3 lightPosition;\n" \
                "attribute vec4 position;\n" \
                "attribute vec3 normal;\n" \
                "attribute vec3 barycentric;\n"
                "varying vec4 varyingColor;\n" \
                "varying vec4 varyingPosition;\n" \
                "varying vec3 varyingBarycentric;\n" \
                "void main() {\n" \
                "    /* Transform normal to eye co-ordinates */\n" \
                "    vec3 eyeNormal = normalMatrix * normal;\n" \
                "    /* Transform position to eye co-ordinates, but don't project it yet */\n" \
                "    vec4 eyePosition = modelview * position;\n" \
                "    vec3 eyePositionHomogenized = eyePosition.xyz / eyePosition.w;\n" \
                "    /* Get difference from light position to eye position - this gives us the direction the light is facing */\n" \
                "    vec3 lightDirection = normalize(lightPosition - eyePositionHomogenized);\n" \
                "    /* Now get the intensity of the light - this will just be the dot product of the eye normal and light direction *\n" \
                "     * If it is facing in the same direction, the light will be at its most intense. We can't have negative lighting so *\n" \
                "     * cap it at a minimum of zero */\n" \
                "    float intensity = max(0.0, dot(eyeNormal, lightDirection));\n" \
                "    /* Calculate specular component, by getting the reflection direction between *\n" \
                "     * the surface and the viewing angle and computing the dot product between the *\n" \
                "     * two. The dot product represents the cosine of the angle between the two, so the *\n" \
                "     * smaller it is, the more shiny this surface will be */\n" \
                "    vec3 reflection = reflect(-lightDirection, eyeNormal);\n" \
                "    float reflectionAngle = max(0.0, dot(reflection, eyeNormal));\n" \
                "    /* Calculate the diffuse lighting and add ambient and specular components */\n" \
                "    varyingColor.xyz = (color.xyz * intensity * 3 + color.xyz * 0.2 + pow(reflectionAngle, 128) * vec3(1.0f)) * color.a;\n" \
                "    varyingColor.a = color.a;\n" \
                "    varyingPosition = position;\n" \
                "    varyingBarycentric = barycentric;\n" \
                "    gl_Position = projection * eyePosition;\n" \
                "}\n";

        vert.addSource(vertex_shader_source);
        frag.addSource(fragment_shader_source);

        CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

        attachShaders({vert, frag});

        bindAttributeLocation(Shaders::Generic<3>::Position::Location, "position");
        bindAttributeLocation(Shaders::Generic<3>::Normal::Location, "normal");
        bindAttributeLocation(Barycentric::Location, "barycentric");

        CORRADE_INTERNAL_ASSERT_OUTPUT(link());

        modelviewUniform = uniformLocation("modelview");
        projectionUniform = uniformLocation("projection");
        colorUniform = uniformLocation("color");
        lightUniform = uniformLocation("lightPosition");
        normalUniform = uniformLocation("normalMatrix");
    }
};

class PostprocessingLayer
{
public:
    Mesh mesh;
    Buffer vertexBuffer;
    Framebuffer captureBuffer;
    Texture2D colorAttachment, depthAttachment;

public:

    PostprocessingLayer() :
        mesh(MeshPrimitive::TriangleStrip),
        captureBuffer(defaultFramebuffer.viewport())
    {
        Trade::MeshData3D rect(MeshPrimitive::TriangleStrip, {{}}, {{
                                   { -1.0f, -1.0f, 0.0f },
                                   { -1.0f, 1.0f, 0.0f },
                                   { 1.0f, -1.0f, 0.0f },
                                   { 1.0f, 1.0f, 0.0f }
                               }}, {{}}, {{
                                   { 0.0f, 0.0f },
                                   { 0.0f, 1.0f },
                                   { 1.0f, 0.0f },
                                   { 1.0f, 1.0f }
                               }});
        vertexBuffer.setData(MeshTools::interleave(rect.positions(0),
                                                   rect.textureCoords2D(0)),
                             BufferUsage::StaticDraw);

        mesh.setCount(rect.positions(0).size())
            .addVertexBuffer(vertexBuffer,
                             0,
                             Shaders::Generic3D::Position{},
                             Shaders::Generic3D::TextureCoordinates{});

        colorAttachment.setMinificationFilter(Sampler::Filter::Nearest)
                       .setMagnificationFilter(Sampler::Filter::Nearest)
                       .setWrapping(Sampler::Wrapping::ClampToEdge)
                       .setStorage(1, TextureFormat::RGBA16F, defaultFramebuffer.viewport().size());

        depthAttachment.setMinificationFilter(Sampler::Filter::Nearest)
                       .setMagnificationFilter(Sampler::Filter::Nearest)
                       .setWrapping(Sampler::Wrapping::ClampToEdge)
                       .setStorage(1, TextureFormat::Depth24Stencil8, defaultFramebuffer.viewport().size());

        captureBuffer.attachTexture(Framebuffer::BufferAttachment::Depth,
                                    depthAttachment,
                                    0)
                     .attachTexture(Framebuffer::ColorAttachment(0),
                                    colorAttachment,
                                    0);
    }

    template<typename Renderer> Texture2D &
    capture(Renderer &&func) {
        class GuarunteedFramebufferBind
        {
        private:
            Framebuffer &fb;
        public:
            GuarunteedFramebufferBind(Framebuffer &fb) :
                fb(fb)
            {
                fb.bind();
            }

            ~GuarunteedFramebufferBind()
            {
                defaultFramebuffer.bind();
            }

            GuarunteedFramebufferBind(GuarunteedFramebufferBind const &) = delete;
            GuarunteedFramebufferBind(GuarunteedFramebufferBind &&) = delete;
            GuarunteedFramebufferBind & operator=(GuarunteedFramebufferBind const &) = delete;
            GuarunteedFramebufferBind & operator=(GuarunteedFramebufferBind &&) = delete;
        };

        GuarunteedFramebufferBind bind(captureBuffer);
        captureBuffer.clear(FramebufferClear::Color | FramebufferClear::Depth);
        func();

        return colorAttachment;
    }

    void draw(AbstractShaderProgram &shader) {
        mesh.draw(shader);
    }
};

class Bar:
    public Object3D,
    public SceneGraph::Drawable3D
{
public:
    Mesh &mesh;
    BarShader &shader;
    Color3 color;
    std::array<float, 500> const &heightMap;
    float &intensity;
    size_t heightMapIndex;
    Timeline timeline;

    Bar(Object3D &parent,
        Mesh &mesh,
        BarShader &shader,
        Color3 const &color,
        SceneGraph::DrawableGroup3D &group,
        std::array<float, 500> const &heightMap,
        float &intensity,
        size_t heightMapIndex);

    void draw(Matrix4 const &transformationMatrix,
              SceneGraph::Camera3D &camera) override;
};

Bar::Bar(Object3D &parent,
         Mesh &mesh,
         BarShader &shader,
         Color3 const &color,
         SceneGraph::DrawableGroup3D &group,
         std::array<float, 500> const &heightMap,
         float &intensity,
         size_t heightMapIndex):
    Object3D(&parent),
    SceneGraph::Drawable3D(*this, &group),
    mesh(mesh),
    shader(shader),
    color(color),
    heightMap(heightMap),
    intensity(intensity),
    heightMapIndex(heightMapIndex)
{
    timeline.start();
}

void Bar::draw(Matrix4 const &transformationMatrix,
               SceneGraph::Camera3D &camera)
{
    Matrix4 transform(Matrix4::translation(Vector3(-0.5f, -0.5f, -1.4f * 2)) *
                      Matrix4::rotationX(Deg(10)) *
                      Matrix4::rotationX(Deg((static_cast<float>(sin(intensity + timeline.previousFrameTime() * 0.2f)) + 1.0f) * 5.0f)) *
                      Matrix4::translation(Vector3(0.0f,
                                                   (static_cast<float>(-sin(intensity)) - 1.0f) * 0.1f,
                                                   (static_cast<float>(-sin(intensity)) + 1.0f) * 0.3f)) *
                      transformationMatrix *
                      Matrix4::scaling(Vector3(1.0f, heightMap[heightMapIndex], 1.0f)));

    shader.setModelviewMatrix(transform)
          .setProjectionMatrix(camera.projectionMatrix())
          .setNormalMatrix(transform.rotationScaling())
          .setLightPosition({0.5f, 0.5f, 1.0f})
          .setColor(Color4(color, heightMap[heightMapIndex] * (1.0f - (heightMapIndex / 10) / 50.0f)));

    timeline.nextFrame();

    mesh.draw(shader);
}

class Floor: public Platform::Application {
    public:
        explicit Floor(const Arguments& arguments);

    private:
        void drawEvent() override;

        Scene3D scene;
        SceneGraph::DrawableGroup3D drawables;
        Object3D cameraObject;
        SceneGraph::Camera3D camera;

        Buffer _vertexBuffer, _indexBuffer;
        Mesh _mesh;

        BarShader _shader;

        std::array<float, 500> heights;

        float intensity;

        LV::InputPtr visualizerInput;

        PostprocessingLayer postprocessing;
        ZoomBlurShader zoomBlur;
};

Floor::Floor(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}.setTitle("FMV")},
    cameraObject(&scene),
    camera(cameraObject),
    intensity(0.0f),
    visualizerInput(LV::Input::load("mplayer"))
{
    visualizerInput->realize();
    Renderer::enable(Renderer::Feature::DepthTest);
    Renderer::enable(Renderer::Feature::Blending);
    Renderer::disable(Renderer::Feature::FaceCulling);
    Renderer::setBlendFunction(Renderer::BlendFunction::One, Renderer::BlendFunction::OneMinusSourceAlpha);
    Renderer::setClearColor(Color4(0.0, 0.0, 0.0, 1.0));

    Trade::MeshData3D cube = barPrimitive(Vector3(1.0f, 1.0f, 1.0f));

    std::array<Vector3, 24> barycentric = {{
                                   {1.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f},
                                   {0.0f, 0.0f, 1.0f},
                                   {0.0f, 1.0f, 0.0f},

                                   {1.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f},
                                   {0.0f, 0.0f, 1.0f},
                                   {0.0f, 1.0f, 0.0f},

                                   {1.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f},
                                   {0.0f, 0.0f, 1.0f},
                                   {0.0f, 1.0f, 0.0f},

                                   {1.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f},
                                   {0.0f, 0.0f, 1.0f},
                                   {0.0f, 1.0f, 0.0f},

                                   {1.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f},
                                   {0.0f, 0.0f, 1.0f},
                                   {0.0f, 1.0f, 0.0f},

                                   {1.0f, 0.0f, 0.0f},
                                   {0.0f, 1.0f, 0.0f},
                                   {0.0f, 0.0f, 1.0f},
                                   {0.0f, 1.0f, 0.0f}
                               }};

    _vertexBuffer.setData(MeshTools::interleave(cube.positions(0), cube.normals(0), barycentric), BufferUsage::StaticDraw);

    Containers::Array<char> indexData;
    Mesh::IndexType indexType;
    UnsignedInt indexStart, indexEnd;
    std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(cube.indices());
    _indexBuffer.setData(indexData, BufferUsage::StaticDraw);

    _mesh.setPrimitive(cube.primitive())
        .setCount(cube.indices().size())
        .addVertexBuffer(_vertexBuffer, 0, Shaders::Generic<3>::Position{}, Shaders::Generic<3>::Normal{}, BarShader::Barycentric{})
        .setIndexBuffer(_indexBuffer, 0, indexType, indexStart, indexEnd);

    auto step = 18;
    for (size_t i = 0; i < 50; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            (new Bar(cameraObject,
                     _mesh,
                     _shader,
                     Color3::fromHSV(Deg(j * step + i * step * 0.1), 1.0f, 1.0f),
                     drawables,
                     heights,
                     intensity,
                     i * 10 + j))
                ->scale(Vector3(0.1, 1.0, 0.1))
                .translate(Vector3(0.1 * j, 0.0, -0.1 * i));
            heights[j + i * 10] = 0.1;
        }
    }

    camera.setProjectionMatrix(Matrix4::perspectiveProjection(20.0_degf, 1.0f, 0.01f, 50.0f))
          .setViewport(defaultFramebuffer.viewport().size());

    setSwapInterval(1);
    setMinimalLoopPeriod(16);
}

constexpr static int sample(size_t index) {
    return static_cast<int>(std::exp((index / 10.0) * log(255.0)));
}

constexpr static const std::array<int, 10> SampleRanges = {
    sample(1),
    sample(2),
    sample(3),
    sample(4),
    sample(5),
    sample(6),
    sample(7),
    sample(8),
    sample(9),
    sample(10)
};

void Floor::drawEvent() {
    Renderer::setClearColor(Color4(0.0f, 0.0f, 0.0f, 1.0f));
    defaultFramebuffer.clear(FramebufferClear::Depth | FramebufferClear::Color);

    auto &texture = postprocessing.capture([this]() {
        camera.draw(drawables);
    });

    zoomBlur.setTexture(texture)
            .setIntensity(0.1f);

    Renderer::disable(Renderer::Feature::DepthTest);
    postprocessing.draw(zoomBlur);
    Renderer::enable(Renderer::Feature::DepthTest);

    swapBuffers();
    redraw();

    visualizerInput->run();
    auto const &audio = visualizerInput->get_audio();
    auto pcm_buffer = LV::Buffer::create(512 * sizeof(float));
    auto freq_buffer = LV::Buffer::create(256 * sizeof(float));
    const_cast<LV::Audio *>(&audio)->get_sample_mixed_simple(pcm_buffer,
                                                             2,
                                                             VISUAL_AUDIO_CHANNEL_LEFT,
                                                             VISUAL_AUDIO_CHANNEL_RIGHT,
                                                             nullptr);
    LV::Audio::get_spectrum_for_sample(freq_buffer, pcm_buffer, true);

    float *frequenciesData = static_cast<float *>(freq_buffer->get_data());
    float currentIntensity = 0.0f;

    /* Prepare height-map - first row is all random, the next rows follow on
     * from the previous height */
    for (size_t i = 0; i < 10; ++i) {
        /* Go through the sample ranges until we reach the frequency with the
         * highest amplitude in the current sample range */
        float highestAmplitudeInRange = 0.0f;
        const int sampleRangeStart = SampleRanges[i];
        const int sampleRangeEnd = SampleRanges[i + 1];

        for (int currentSampleFrequency = sampleRangeStart;
             currentSampleFrequency <= sampleRangeEnd;
             currentSampleFrequency++) {
            if (frequenciesData[currentSampleFrequency] > highestAmplitudeInRange) {
                highestAmplitudeInRange = frequenciesData[currentSampleFrequency];
            }
        }

        heights[i] = heights[i] * 0.6f + (highestAmplitudeInRange * 0.4f) + 0.05f;

        currentIntensity += heights[i] / 10.0f;
    }

    intensity = intensity * 0.7f + currentIntensity * 0.3f;

    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 1; j < 50; ++j) {
            heights[j * 10 + i] = (heights[j * 10 + i] / 2.0f) + (heights[(j - 1) * 10 + i] / 2.0f);
        }
    }
}

}}

int main(int argc, char **argv)
{
    LV::System::init(argc, argv);
    Magnum::Examples::Floor app(Magnum::Examples::Floor::Arguments(argc, argv));
    int code = app.exec();
    LV::System::destroy();

    return code;
}
