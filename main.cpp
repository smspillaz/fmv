/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <cstdlib>

#include <array>

#include <Magnum/Buffer.h>
#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Renderer.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Drawable.hpp>
#include <Magnum/SceneGraph/Camera3D.hpp>

namespace Magnum { namespace Examples {

typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

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

        Shaders::Phong _shader;

        std::array<float, 100> heights;
};

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

class Bar:
    public Object3D,
    public SceneGraph::Drawable3D
{
public:
    Mesh &mesh;
    Shaders::Phong &shader;
    Color3 color;
    std::array<float, 100> const &heightMap;
    size_t heightMapIndex;

    Bar(Object3D &parent,
        Mesh &mesh,
        Shaders::Phong &shader,
        Color3 const &color,
        SceneGraph::DrawableGroup3D &group,
        std::array<float, 100> const &heightMap,
        size_t heightMapIndex);

    void draw(Matrix4 const &transformationMatrix,
              SceneGraph::Camera3D &camera) override;
};

Bar::Bar(Object3D &parent,
         Mesh &mesh,
         Shaders::Phong &shader,
         Color3 const &color,
         SceneGraph::DrawableGroup3D &group,
         std::array<float, 100> const &heightMap,
         size_t heightMapIndex):
    Object3D(&parent),
    SceneGraph::Drawable3D(*this, &group),
    mesh(mesh),
    shader(shader),
    color(color),
    heightMap(heightMap),
    heightMapIndex(heightMapIndex)
{
}

void Bar::draw(Matrix4 const &transformationMatrix,
               SceneGraph::Camera3D &camera)
{
    Matrix4 transform(Matrix4::translation(Vector3(-0.5f, -0.5f, -1.2f * 2)) *
                      transformationMatrix *
                      Matrix4::scaling(Vector3(1.0f, heightMap[heightMapIndex], 1.0f)));

    shader.setLightColor(Color3{1.0f})
          .setDiffuseColor(color)
          .setAmbientColor(Color3::fromHSV(color.hue(), 1.0f, 0.3f))
          .setTransformationMatrix(transform)
          .setNormalMatrix(transform.rotationScaling())
          .setProjectionMatrix(camera.projectionMatrix())
          .setLightPosition({0.5f, 1.0f, -0.5f});

    mesh.draw(shader);
}

Floor::Floor(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}.setTitle("Floor Music Visualiser")},
    cameraObject(&scene),
    camera(cameraObject)
{
    Renderer::enable(Renderer::Feature::DepthTest);
    Renderer::enable(Renderer::Feature::FaceCulling);

    Trade::MeshData3D cube = barPrimitive(Vector3(1.0f, 1.0f, 1.0f));

    _vertexBuffer.setData(MeshTools::interleave(cube.positions(0), cube.normals(0)), BufferUsage::StaticDraw);

    Containers::Array<char> indexData;
    Mesh::IndexType indexType;
    UnsignedInt indexStart, indexEnd;
    std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(cube.indices());
    _indexBuffer.setData(indexData, BufferUsage::StaticDraw);

    _mesh.setPrimitive(cube.primitive())
        .setCount(cube.indices().size())
        .addVertexBuffer(_vertexBuffer, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
        .setIndexBuffer(_indexBuffer, 0, indexType, indexStart, indexEnd);

    auto step = 18;
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            (new Bar(cameraObject,
                     _mesh,
                     _shader,
                     Color3::fromHSV(Deg(j * step + i * step * 0.1), 1.0f, 1.0f),
                     drawables,
                     heights,
                     i * 10 + j))
                ->scale(Vector3(0.1, 1.0, 0.1))
                .translate(Vector3(0.1 * j, 0.0, -0.1 * i));
            heights[j + i * 10] = 0.1;
        }
    }

    camera.setProjectionMatrix(Matrix4::perspectiveProjection(20.0_degf, 1.0f, 0.01f, 100.0f))
          .setViewport(defaultFramebuffer.viewport().size());

    setSwapInterval(1);
    setMinimalLoopPeriod(16);
}

void Floor::drawEvent() {
    defaultFramebuffer.clear(FramebufferClear::Color|FramebufferClear::Depth);

    camera.draw(drawables);

    swapBuffers();
    redraw();

    /* Prepare height-map - first row is all random, the next rows follow on
     * from the previous height */
    for (size_t i = 0; i < 10; ++i) {
        heights[i] = heights[i] * 0.9f + ((rand() % 40) / 100.0f) * 0.1f;
    }

    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 1; j < 10; ++j) {
            heights[j * 10 + i] = (heights[j * 10 + i] / 2.0f) + (heights[(j - 1) * 10 + i] / 2.0f);
        }
    }
}

}}

int main(int argc, char **argv)
{
    Magnum::Examples::Floor app(Magnum::Examples::Floor::Arguments(argc, argv));
    return app.exec();
}
